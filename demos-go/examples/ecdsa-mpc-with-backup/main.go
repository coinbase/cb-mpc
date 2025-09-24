package main

import (
	"bytes"
	"crypto/ecdsa"
	"crypto/rand"
	"crypto/rsa"
	"crypto/sha256"
	"crypto/x509"
	"encoding/asn1"
	"encoding/hex"
	"fmt"
	"io"
	"log"
	"math/big"
	"runtime"
	"unsafe"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/mpc"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"
	"github.com/btcsuite/btcd/btcec/v2"
	"golang.org/x/crypto/hkdf"
	"golang.org/x/sync/errgroup"
)

// Deterministic reader for RSA OAEP derived from rho (seed)
type ctrRand struct {
	seed    [32]byte
	counter uint64
	buf     []byte
	off     int
}

func newCTRRand(seed []byte) *ctrRand {
	var s [32]byte
	copy(s[:], seed)
	return &ctrRand{seed: s}
}

func (r *ctrRand) refill() {
	ctrBytes := make([]byte, 8)
	for i := 0; i < 8; i++ {
		ctrBytes[7-i] = byte(r.counter >> (8 * i))
	}
	h := sha256.Sum256(append(ctrBytes, r.seed[:]...))
	r.buf = h[:]
	r.off = 0
	r.counter++
}

func (r *ctrRand) Read(p []byte) (int, error) {
	n := 0
	for n < len(p) {
		if r.off >= len(r.buf) {
			r.refill()
		}
		m := copy(p[n:], r.buf[r.off:])
		r.off += m
		n += m
	}
	return n, nil
}

// rsaDemoKEM implements the mpc KEM interface with RSA OAEP and deterministic encapsulation
// so that PVE verification is reproducible.
type rsaDemoKEM struct{}

const kemLabel = "demo-rsa-kem" // wire label; do not change without versioning
const kemDS = "rsa-demo-kem:v1" // domain-sep string for KDF derivations

func (rsaDemoKEM) Generate() ([]byte, []byte, error) {
	k, err := rsa.GenerateKey(rand.Reader, 2048)
	if err != nil {
		return nil, nil, err
	}
	k.Precompute()

	prv := x509.MarshalPKCS1PrivateKey(k)
	pub := x509.MarshalPKCS1PublicKey(&k.PublicKey)
	return prv, pub, nil
}

func (rsaDemoKEM) Encapsulate(ek []byte, rho [32]byte) ([]byte, []byte, error) {
	pub, err := x509.ParsePKCS1PublicKey(ek)
	if err != nil {
		return nil, nil, err
	}
	if pub.Size() != 256 {
		return nil, nil, fmt.Errorf("invalid RSA modulus size: got %d bits", pub.Size()*8)
	}
	if pub.E != 65537 {
		return nil, nil, fmt.Errorf("unsupported RSA public exponent")
	}

	// --- Derive independent materials from rho, bound to key N and DS ---
	salt := pub.N.Bytes() // binds derivations to recipient key

	// Derive the OAEP seed (exactly Hash.Size() bytes for SHA-256)
	var oaepSeed [32]byte
	if _, err := io.ReadFull(hkdf.New(sha256.New, rho[:], salt, []byte(kemDS+"|oaep-seed")), oaepSeed[:]); err != nil {
		return nil, nil, fmt.Errorf("hkdf: %w", err)
	}

	// Derive the shared secret independently from the OAEP seed
	ss := make([]byte, 32)
	if _, err := io.ReadFull(hkdf.New(sha256.New, rho[:], salt, []byte(kemDS+"|ss")), ss); err != nil {
		mpc.SecureWipe(oaepSeed[:])
		return nil, nil, fmt.Errorf("hkdf: %w", err)
	}

	// Deterministic OAEP randomness: rsa.EncryptOAEP reads exactly Hash.Size() bytes.
	r := bytes.NewReader(oaepSeed[:])
	ct, err := rsa.EncryptOAEP(sha256.New(), r, pub, ss, []byte(kemLabel))
	mpc.SecureWipe(oaepSeed[:]) // best-effort wipe
	if err != nil {
		mpc.SecureWipe(ss)
		return nil, nil, err
	}

	// Deterministic: (pub, rho) -> (ct, ss)
	return ct, ss, nil
}

func (rsaDemoKEM) Decapsulate(skHandle unsafe.Pointer, ct []byte) ([]byte, error) {
	// Expect cmem_t pointing to private key bytes
	type cmem_t struct {
		data *byte
		size int32
	}
	cm := (*cmem_t)(skHandle)
	if cm == nil || cm.data == nil || cm.size <= 0 {
		return nil, fmt.Errorf("kem: decapsulation failed") // uniform error
	}
	// Sanity cap to avoid DoS; PKCS#1 DER for 2048-bit is ~1–2KB
	if cm.size < 256 || cm.size > 8192 {
		return nil, fmt.Errorf("kem: decapsulation failed")
	}

	// Copy foreign memory into Go memory before parsing (safer if caller frees it)
	dk := unsafe.Slice((*byte)(unsafe.Pointer(cm.data)), int(cm.size))
	dkCopy := make([]byte, len(dk))
	copy(dkCopy, dk)
	// Ensure cm isn’t GC’d early
	runtime.KeepAlive(cm)

	prv, err := x509.ParsePKCS1PrivateKey(dkCopy)
	mpc.SecureWipe(dkCopy) // best-effort wipe of private key bytes copy
	if err != nil {
		return nil, fmt.Errorf("kem: decapsulation failed")
	}
	// Pin modulus size to 2048
	if prv.Size() != 256 {
		return nil, fmt.Errorf("kem: decapsulation failed")
	}
	// Basic sanity on ciphertext length
	if len(ct) != prv.Size() {
		return nil, fmt.Errorf("kem: decapsulation failed")
	}

	// Decrypt with blinding; output still deterministic wrt inputs
	ss, err := rsa.DecryptOAEP(sha256.New(), rand.Reader, prv, ct, []byte(kemLabel))
	if err != nil {
		return nil, fmt.Errorf("kem: decapsulation failed")
	}
	if len(ss) != 32 {
		// Should not occur if encaps follows the contract, but guard anyway
		return nil, fmt.Errorf("kem: decapsulation failed")
	}

	out := make([]byte, 32)
	copy(out, ss)
	mpc.SecureWipe(ss) // wipe temp
	return out, nil
}

func (rsaDemoKEM) DerivePub(dk []byte) ([]byte, error) {
	prv, err := x509.ParsePKCS1PrivateKey(dk)
	if err != nil {
		return nil, err
	}
	if prv.Size() != 256 {
		return nil, fmt.Errorf("invalid RSA modulus size: got %d bits", prv.Size()*8)
	}
	return x509.MarshalPKCS1PublicKey(&prv.PublicKey), nil
}

func main() {
	fmt.Println("=== ECDSA MPC with Backup Example ===")
	fmt.Println("This example demonstrates:")
	fmt.Println("1. N-party ECDSA key generation and signing")
	fmt.Println("2. Secure backup and recovery of key shares using PVE")
	fmt.Println()

	// Configuration
	// The batch size determines the number of signing keys to generate. This is so that a batch of keys is created
	// for each party and the batch backup using PVE can be properly demoed.
	batchSize := 2
	nParties := 4
	messengers := mocknet.NewMockNetwork(nParties)
	partyNames := make([]string, nParties)
	for i := 0; i < nParties; i++ {
		// In production settings, the party name should be tied to the party's identity. For example, hash of the public key.
		partyNames[i] = fmt.Sprintf("p%d", i)
	}
	secp, err := curve.NewSecp256k1()
	if err != nil {
		log.Fatal(fmt.Errorf("failed to create secp256k1 curve: %v", err))
	}
	parties := make([]*Party, nParties)
	for i := 0; i < nParties; i++ {
		parties[i] = &Party{
			Index:      i,
			Messenger:  messengers[i],
			NParties:   nParties,
			PartyNames: partyNames,
			BatchSize:  batchSize,
			dkgResp:    make([]*mpc.ECDSAMPCKeyGenResponse, batchSize),
			signResp:   make([]*mpc.ECDSAMPCSignResponse, batchSize),
		}
	}

	signatureReceiverId := 0

	// Step 1: Run N-party ECDSA key generation and signing
	fmt.Println("## Step 1: N-Party ECDSA Key Generation and Signing")
	eg := errgroup.Group{}
	for i := 0; i < nParties; i++ {
		i := i
		eg.Go(func() error {
			for k := 0; k < batchSize; k++ {
				if err := parties[i].Dkg(secp, k); err != nil {
					return err
				}
			}
			return nil
		})
	}

	if err := eg.Wait(); err != nil {
		log.Fatal(fmt.Errorf("ECDSA keygen failed: %v", err))
	}

	fmt.Printf("Generated %d-party ECDSA key shares\n", nParties)
	// All parties will have received the same Q, using one of the as representative
	for k := 0; k < batchSize; k++ {
		Q := parties[signatureReceiverId].Q(k)
		fmt.Printf("* Public Key[%d]: %v\n", k, Q)
	}

	// Step 2: Backup the key shares using many to many PVE
	fmt.Println("## Step 2: Backing Up Key Shares with PVE")

	// Step 2.1: create the access structure for backing up the keys
	root := mpc.And("")
	root.Children = []*mpc.AccessNode{mpc.Leaf(partyNames[0]), mpc.Threshold("th", 2)}
	root.Children[1].Children = []*mpc.AccessNode{mpc.Leaf(partyNames[1]), mpc.Leaf(partyNames[2]), mpc.Leaf(partyNames[3])}
	ac := mpc.AccessStructure{
		Root:  root,
		Curve: secp,
	}

	// Step 2.2: define a KEM and PVE instance (RSA KEM here)
	for i := 0; i < nParties; i++ {
		parties[i].InitPVE()
	}

	// Step 2.3: create encryption keys for leaves via KEM
	// NOTE: in this demo, the parties are ALSO acting as backup holders but this does not have to be the case.
	// If the backup holders are different, then the RSA keys should be generated by a different group of parties
	// and their public keys should be communicated with the keyshare holders.
	pubKeys := make(map[string]mpc.BaseEncPublicKey)
	for i := 0; i < nParties; i++ {
		// In a production setting, the public keys should be exchanged using PKI or some other secure mechanism
		pubKeys[partyNames[i]] = parties[i].RSAKeygen()
	}

	// Step 2.4: choose a human readable label. This will be cryptographically bound to the backup data
	inputLabel := "demo-data"

	// Step 2.5: create a publicly verifiable backup
	// Each party create a batch backup of all the dkg keys that it has generated
	pveEncResps := make([]*mpc.PVEAcEncryptResponse, nParties)
	for i := 0; i < nParties; i++ {
		pveEncResps[i] = parties[i].Backup(inputLabel, secp, &ac, pubKeys)
	}

	// Step 2.7: verify the backup
	// All the parties verify all the backups that have been generated by themselves and everyone else
	for i := 0; i < nParties; i++ {
		err := parties[i].VerifyAllBackups(pveEncResps, &ac, inputLabel, pubKeys)
		if err != nil {
			log.Fatal(fmt.Errorf("failed to verify: %v", err))
		}
	}
	fmt.Printf("PVE verification passed\n")

	// Step 2.8: restore via interactive quorum PVE
	// Each backup holder party (in this demo the same as dkg parties), uses their RSA decryption keys to
	// partially decrypt each of the backups
	// The partial decryptions are sent to the appropriate recipient and aggregated.
	// IMPORTANT: In production setting it is extremely important that this is done correctly as to
	//            not send the partial decryptions to the incorrect party.
	allPartialDecs := make([][]*mpc.PVEAcPartyDecryptRowResponse, nParties)
	for i := 0; i < nParties; i++ {
		allPartialDecs[i] = parties[i].PartialBackupDecryption(pveEncResps, inputLabel, &ac)
	}

	for i := 0; i < nParties; i++ {
		// Prepare all the partial decryptions made for party i by all parties
		respsForParty := make(map[string]*mpc.PVEAcPartyDecryptRowResponse)
		for j := 0; j < nParties; j++ {
			respsForParty[partyNames[j]] = allPartialDecs[j][i]
		}
		// Party i uses all the partial decryptions to decrypt its own backup
		backup := pveEncResps[i]
		aggResp := parties[i].AggregatePartialDecryptedBackups(respsForParty, backup, inputLabel, &ac)

		// Assert restored values
		for k := 0; k < batchSize; k++ {
			xs, err := parties[i].dkgResp[k].KeyShare.XShare()
			if err != nil {
				log.Fatalf("failed to get share %v", err)
			}
			if !bytes.Equal(aggResp.PrivateValues[k].Bytes, xs.Bytes) {
				log.Fatal("decrypted value does not match the original value")
			}
		}
	}
	fmt.Printf("PVE restore passed\n")

	// Step 4: Sign using the recovered keyshares
	fmt.Println("## Step 1: N-Party ECDSA Key Generation and Signing")

	inputMessage := []byte("This is a message for ECDSA MPC with backup")
	hash := sha256.Sum256(inputMessage)
	eg = errgroup.Group{}
	for i := 0; i < nParties; i++ {
		i := i
		eg.Go(func() error {
			for k := 0; k < batchSize; k++ {
				if err := parties[i].Sign(hash, signatureReceiverId, k); err != nil {
					log.Fatalf("signing failed %v", err)
				}
			}

			return nil
		})
	}

	if err := eg.Wait(); err != nil {
		log.Fatal(fmt.Errorf("ECDSA sign failed: %v", err))
	}

	for k := 0; k < batchSize; k++ {
		sig := parties[signatureReceiverId].signResp[k].Signature
		fmt.Printf("* Signature[%d]: %s\n", k, hex.EncodeToString(sig))
		fmt.Println()

		// Verifying the signature
		// Extract X and Y coordinates from the MPC public key
		Q := parties[signatureReceiverId].Q(k)
		xBytes := Q.GetX()
		yBytes := Q.GetY()

		x := new(big.Int).SetBytes(xBytes)
		y := new(big.Int).SetBytes(yBytes)

		goPubKey := &ecdsa.PublicKey{
			Curve: btcec.S256(),
			X:     x,
			Y:     y,
		}

		// Parse DER-encoded signature
		// DER format: SEQUENCE { r INTEGER, s INTEGER }
		type ecdsaSignature struct {
			R, S *big.Int
		}

		var derSig ecdsaSignature
		_, err = asn1.Unmarshal(sig, &derSig)
		if err != nil {
			log.Fatal(fmt.Errorf("failed to parse DER signature: %v", err))
		}

		r := derSig.R
		s := derSig.S

		valid := ecdsa.Verify(goPubKey, hash[:], r, s)

		if valid {
			fmt.Println("Signature verification PASSED")
			fmt.Println("* The signature is valid and matches the message and public key")
		} else {
			fmt.Println("Signature verification FAILED")
			fmt.Println("* The signature does not match the message and public key")
		}
	}
}
