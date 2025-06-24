package main

import (
	"bytes"
	"crypto/ecdsa"
	"crypto/sha256"
	"encoding/asn1"
	"encoding/hex"
	"fmt"
	"log"
	"math/big"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/mpc"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"
	"github.com/btcsuite/btcd/btcec/v2"
	"golang.org/x/sync/errgroup"
)

func main() {
	fmt.Println("=== ECDSA MPC with Backup Example ===")
	fmt.Println("This example demonstrates:")
	fmt.Println("1. N-party ECDSA key generation and signing")
	fmt.Println("2. Secure backup and recovery of key shares using PVE")
	fmt.Println()

	// Configuration
	nParties := 4
	messengers := mocknet.NewMockNetwork(nParties)
	partyNames := make([]string, nParties)
	for i := range nParties {
		partyNames[i] = fmt.Sprintf("p%d", i)
	}
	secp, err := curve.NewSecp256k1()
	if err != nil {
		log.Fatal(fmt.Errorf("failed to create secp256k1 curve: %v", err))
	}
	signatureReceiver := 0
	inputMessage := []byte("This is a message for ECDSA MPC with backup")
	hash := sha256.Sum256(inputMessage)

	// Step 1: Run N-party ECDSA key generation and signing
	fmt.Println("## Step 1: N-Party ECDSA Key Generation and Signing")
	eg := errgroup.Group{}
	dkgResp := make([]*mpc.ECDSAMPCKeyGenResponse, nParties)
	dkgRespChan := make(chan struct {
		idx  int
		resp *mpc.ECDSAMPCKeyGenResponse
	}, nParties)
	for i := range nParties {
		eg.Go(func() error {
			jp, err := mpc.NewJobMP(messengers[i], nParties, i, partyNames)
			if err != nil {
				return err
			}
			defer jp.Free()

			resp, err := mpc.ECDSAMPCKeyGen(jp, &mpc.ECDSAMPCKeyGenRequest{Curve: secp})
			if err != nil {
				return err
			}

			dkgRespChan <- struct {
				idx  int
				resp *mpc.ECDSAMPCKeyGenResponse
			}{i, resp}
			return nil
		})
	}

	if err := eg.Wait(); err != nil {
		log.Fatal(fmt.Errorf("ECDSA keygen failed: %v", err))
	}

	for range nParties {
		r := <-dkgRespChan
		dkgResp[r.idx] = r.resp
	}

	fmt.Printf("✅ Generated %d-party ECDSA key shares\n", nParties)
	Q, err := dkgResp[0].KeyShare.Q()
	if err != nil {
		log.Fatal(fmt.Errorf("failed to get public key: %v", err))
	}
	fmt.Printf("• Public Key: %v\n", Q)

	// Step 2: Backup the key shares using many to many PVE
	fmt.Println("## Step 2: Backing Up Key Shares with PVE")

	// Step 2.1: create the access structure for backing up the keys
	root := mpc.And("")
	root.Children = []*mpc.AccessNode{mpc.Leaf("leaf1"), mpc.Threshold("th", 2)}
	root.Children[1].Children = []*mpc.AccessNode{mpc.Leaf("leaf2"), mpc.Leaf("leaf3"), mpc.Leaf("leaf4")}
	ac := mpc.AccessStructure{
		Root:  root,
		Curve: secp,
	}

	// Step 2.2: create encryption keys that will be used to encrypt each of the subshares at the leaves.
	//           this demo uses ECIES keys but both ECIES and RSA are supported by the cpp library
	leafCount := 4
	pubKeys := make(map[string]mpc.BaseEncPublicKey)
	privateKeys := make(map[string]mpc.BaseEncPrivateKey)
	for i := range leafCount {
		pubKeys[fmt.Sprintf("leaf%d", i+1)], privateKeys[fmt.Sprintf("leaf%d", i+1)], err = mpc.GenerateBaseEncKeypair()
		if err != nil {
			log.Fatal(fmt.Errorf("failed to generate base encryption key pair: %v", err))
		}
	}

	// Step 2.3: choose a human readable label. This will be cryptographically bound to the backup data
	inputLabel := "demo-data"

	// Step 2.4: extract the data in a list
	dataCount := nParties
	xs := make([]*curve.Scalar, dataCount)
	Xs := make([]*curve.Point, dataCount)
	for i := range dataCount {
		xs[i], err = dkgResp[i].KeyShare.XShare()
		if err != nil {
			log.Fatal(fmt.Errorf("failed to get X share: %v", err))
		}
		Qis, err := dkgResp[i].KeyShare.Qis()
		if err != nil {
			log.Fatal(fmt.Errorf("failed to get Qis: %v", err))
		}
		Xs[i] = Qis[partyNames[i]]
	}

	// Step 2.5: create a publicly verifiable backup the secret data using all the above
	pveEncResp, err := mpc.PVEEncrypt(&mpc.PVEEncryptRequest{
		AccessStructure: &ac,
		PublicKeys:      pubKeys,
		PrivateValues:   xs,
		Label:           inputLabel,
	})
	if err != nil {
		log.Fatal(fmt.Errorf("failed to encrypt: %v", err))
	}

	// Step 2.6: verify the backup
	verifyResp, err := mpc.PVEVerify(&mpc.PVEVerifyRequest{
		AccessStructure: &ac,
		EncryptedBundle: pveEncResp.EncryptedBundle,
		PublicKeys:      pubKeys,
		PublicShares:    Xs,
		Label:           inputLabel,
	})
	if err != nil {
		log.Fatal(fmt.Errorf("failed to verify: %v", err))
	}
	if !verifyResp.Valid {
		log.Fatal(fmt.Errorf("PVE verification failed"))
	}
	fmt.Printf("✅ PVE verification passed\n")

	// Step 2.7: IMPORTANT: the above PVE only backs up the private keyshare. But an ECDSA key struct has,
	//           a couple of other important "public" data that needs to be backed up separately including the
	//           public key, all the public shares, and party indices.
	//           Since this is just for demonstration purposes, we are only focusing on the private keyshare

	// Step 2.8: to restore, run PVE many to many to restore each of the key shares
	pveDecResp, err := mpc.PVEDecrypt(&mpc.PVEDecryptRequest{
		AccessStructure: &ac,
		PublicKeys:      pubKeys,
		PrivateKeys:     privateKeys,
		EncryptedBundle: pveEncResp.EncryptedBundle,
		PublicShares:    Xs,
		Label:           inputLabel,
	})
	if err != nil {
		log.Fatal(fmt.Errorf("failed to decrypt: %v", err))
	}

	// convert the data back to the proper type
	// As described in Step 6 above, this is not a complete backup solution. Therefore, we simply assert that the decrypted
	// values match the inputs.
	for i := range dataCount {
		if !bytes.Equal(pveDecResp.PrivateValues[i].Bytes, xs[i].Bytes) {
			log.Fatal(fmt.Errorf("decrypted value does not match the original value"))
		}
	}

	// Step 4: Sign using the recovered keyshares
	fmt.Println("## Step 1: N-Party ECDSA Key Generation and Signing")
	eg = errgroup.Group{}
	signResp := make([]*mpc.ECDSAMPCSignResponse, nParties)
	signRespChan := make(chan struct {
		idx  int
		resp *mpc.ECDSAMPCSignResponse
	}, nParties)
	for i := range nParties {
		eg.Go(func() error {
			jp, err := mpc.NewJobMP(messengers[i], nParties, i, partyNames)
			if err != nil {
				return err
			}
			defer jp.Free()

			resp, err := mpc.ECDSAMPCSign(jp, &mpc.ECDSAMPCSignRequest{
				KeyShare:          dkgResp[i].KeyShare,
				Message:           hash[:],
				SignatureReceiver: signatureReceiver,
			})
			if err != nil {
				return err
			}

			signRespChan <- struct {
				idx  int
				resp *mpc.ECDSAMPCSignResponse
			}{i, resp}
			return nil
		})
	}

	if err := eg.Wait(); err != nil {
		log.Fatal(fmt.Errorf("ECDSA sign failed: %v", err))
	}

	for range nParties {
		r := <-signRespChan
		signResp[r.idx] = r.resp
	}

	fmt.Printf("• Signature: %s\n", hex.EncodeToString(signResp[signatureReceiver].Signature))
	fmt.Println()

	// Verifying the signature
	// Extract X and Y coordinates from the MPC public key
	xBytes := Q.GetX()
	yBytes := Q.GetY()

	x := new(big.Int).SetBytes(xBytes)
	y := new(big.Int).SetBytes(yBytes)

	goPubKey := &ecdsa.PublicKey{
		Curve: btcec.S256(),
		X:     x,
		Y:     y,
	}

	sig := signResp[signatureReceiver].Signature

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
		fmt.Println("✅ Signature verification PASSED")
		fmt.Println("• The signature is valid and matches the message and public key")
	} else {
		fmt.Println("❌ Signature verification FAILED")
		fmt.Println("• The signature does not match the message and public key")
	}

}
