package main

import (
	"crypto/ecdsa"
	"crypto/sha256"
	"encoding/asn1"
	"encoding/hex"
	"fmt"
	"log"
	"math/big"

	"sync"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/mpc"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"
	"github.com/btcsuite/btcd/btcec/v2"
	btcecEcdsa "github.com/btcsuite/btcd/btcec/v2/ecdsa"
)

func main() {
	fmt.Println("\n=== CB-MPC ECDSA 2PC Example ===")

	// Example: Complete ECDSA 2PC workflow (key generation + signing)
	// Initialize the secp256k1 curve implementation. Remember to release the
	// underlying native resources when it is no longer needed.
	curveObj, err := curve.NewSecp256k1()
	if err != nil {
		log.Fatalf("failed to initialize curve: %v", err)
	}
	defer curveObj.Free()

	fmt.Println("## Running ECDSA 2PC key generation only")

	keyGenResponses, err := keyGenWithMockNet(curveObj)
	if err != nil {
		log.Fatalf("Key generation failed: %v", err)
	}

	fmt.Printf("✅ Key generation completed\n")
	fmt.Printf("Generated %d key shares for future use\n", len(keyGenResponses))

	printKeyShares("Initial key shares", keyGenResponses)

	// === Signing round 1 ===
	message1 := []byte("Hello, CB-MPC!")
	digest1 := sha256.Sum256(message1)
	fmt.Println("\n## Running first collaborative signing round")
	firstSigResponses, err := signWithMockNet([]byte("session-1"), digest1[:], keyGenResponses)
	if err != nil {
		log.Fatalf("Signing round 1 failed: %v", err)
	}
	printSignatures(message1, firstSigResponses)
	if err := verifyExampleSignature(keyGenResponses[0].KeyShare, firstSigResponses[0].Signature, digest1[:]); err != nil {
		fmt.Printf("❌ Signature verification FAILED: %v\n", err)
	} else {
		fmt.Println("✅ Signature verification PASSED")
	}

	// === Refresh ===
	fmt.Println("\n## Running key refresh (re-share)")
	refreshResponses, err := refreshWithMockNet(keyGenResponses)
	if err != nil {
		log.Fatalf("Refresh failed: %v", err)
	}
	fmt.Println("✅ Refresh completed – parties now hold new key shares")
	printKeyShares("Refreshed key shares", refreshResponses)

	// === Signing round 2 ===
	message2 := []byte("Fresh signing after refresh!")
	digest2 := sha256.Sum256(message2)
	fmt.Println("\n## Running second collaborative signing round")
	secondSigResponses, err := signWithMockNet([]byte("session-2"), digest2[:], refreshResponses)
	if err != nil {
		log.Fatalf("Signing round 2 failed: %v", err)
	}
	printSignatures(message2, secondSigResponses)
	if err := verifyExampleSignature(refreshResponses[0].KeyShare, secondSigResponses[0].Signature, digest2[:]); err != nil {
		fmt.Printf("❌ Signature verification FAILED: %v\n", err)
	} else {
		fmt.Println("✅ Signature verification PASSED")
	}

	fmt.Println("🎉 ECDSA 2PC example completed successfully!")
}

// keyGenWithMockNet is a small helper used solely by this example to run the
// distributed key-generation protocol using the in-memory mock network. It
// relies only on the public cb-mpc-go API and therefore avoids importing any
// internal packages.
func keyGenWithMockNet(curveObj curve.Curve) ([]*mpc.ECDSA2PCKeyGenResponse, error) {
	if curveObj == nil {
		return nil, fmt.Errorf("curve must be provided")
	}

	const nParties = 2
	messengers := mocknet.NewMockNetwork(nParties)
	partyNames := []string{"party_0", "party_1"}

	responses := make([]*mpc.ECDSA2PCKeyGenResponse, nParties)
	var wg sync.WaitGroup
	var firstErr error

	for i := 0; i < nParties; i++ {
		wg.Add(1)
		go func(idx int) {
			defer wg.Done()

			// Create a Job2P for this party
			jp, err := mpc.NewJob2P(messengers[idx], idx, partyNames)
			if err != nil {
				firstErr = err
				return
			}
			defer jp.Free()

			resp, err := mpc.ECDSA2PCKeyGen(jp, &mpc.ECDSA2PCKeyGenRequest{Curve: curveObj})
			if err != nil {
				firstErr = err
				return
			}
			responses[idx] = resp
		}(i)
	}

	wg.Wait()

	if firstErr != nil {
		return nil, firstErr
	}

	return responses, nil
}

// printKeyShares prints the role index, x-share, and public key Q for each party.
func printKeyShares(title string, keyGenResponses []*mpc.ECDSA2PCKeyGenResponse) {
	fmt.Printf("\n### %s\n", title)
	for _, resp := range keyGenResponses {
		fmt.Printf("KeyShare: %+v\n", resp.KeyShare)
		share := resp.KeyShare
		roleIdx, _ := share.RoleIndex()
		fmt.Printf("RoleIndex: %d\n", roleIdx)

		x, _ := share.XShare()
		fmt.Printf("Party %d: x_i = %s\n", roleIdx, x)
		Q, _ := share.Q()
		fmt.Printf("Party %d: Q    = %s\n", roleIdx, Q)
		Q.Free()
	}
}

// printSignatures displays the signatures obtained by the parties.
func printSignatures(message []byte, signResponses []*mpc.ECDSA2PCSignResponse) {
	fmt.Printf("\nSignatures for message: %q\n", message)
	for i, resp := range signResponses {
		if len(resp.Signature) == 0 {
			fmt.Printf("Party %d: <no signature – contributed to protocol>\n", i)
			continue
		}
		fmt.Printf("Party %d signature: %s\n", i, hex.EncodeToString(resp.Signature))
	}
}

// signWithMockNet runs the collaborative signing protocol using an in-memory
// network and returns each party's response.
func signWithMockNet(sessionID, message []byte, keyGenResponses []*mpc.ECDSA2PCKeyGenResponse) ([]*mpc.ECDSA2PCSignResponse, error) {
	if len(keyGenResponses) != 2 {
		return nil, fmt.Errorf("need exactly 2 key shares, got %d", len(keyGenResponses))
	}
	const nParties = 2
	messengers := mocknet.NewMockNetwork(nParties)
	partyNames := []string{"party_0", "party_1"}

	responses := make([]*mpc.ECDSA2PCSignResponse, nParties)
	var wg sync.WaitGroup
	var firstErr error

	for i := 0; i < nParties; i++ {
		wg.Add(1)
		go func(idx int) {
			defer wg.Done()

			jp, err := mpc.NewJob2P(messengers[idx], idx, partyNames)
			if err != nil {
				firstErr = err
				return
			}
			defer jp.Free()

			msgCopy := append([]byte(nil), message...)
			resp, err := mpc.ECDSA2PCSign(jp, &mpc.ECDSA2PCSignRequest{
				SessionID: sessionID,
				KeyShare:  keyGenResponses[idx].KeyShare,
				Message:   msgCopy,
			})
			if err != nil {
				firstErr = err
				return
			}
			responses[idx] = resp
		}(i)
	}

	wg.Wait()

	if firstErr != nil {
		return nil, firstErr
	}
	return responses, nil
}

// refreshWithMockNet performs the re-share protocol for the provided key
// shares and returns the refreshed shares.
func refreshWithMockNet(oldKeyGenResponses []*mpc.ECDSA2PCKeyGenResponse) ([]*mpc.ECDSA2PCKeyGenResponse, error) {
	if len(oldKeyGenResponses) != 2 {
		return nil, fmt.Errorf("need exactly 2 key shares, got %d", len(oldKeyGenResponses))
	}
	const nParties = 2
	messengers := mocknet.NewMockNetwork(nParties)
	partyNames := []string{"party_0", "party_1"}

	newResponses := make([]*mpc.ECDSA2PCKeyGenResponse, nParties)
	var wg sync.WaitGroup
	var firstErr error

	for i := 0; i < nParties; i++ {
		wg.Add(1)
		go func(idx int) {
			defer wg.Done()

			jp, err := mpc.NewJob2P(messengers[idx], idx, partyNames)
			if err != nil {
				firstErr = err
				return
			}
			defer jp.Free()

			resp, err := mpc.ECDSA2PCRefresh(jp, &mpc.ECDSA2PCRefreshRequest{
				KeyShare: oldKeyGenResponses[idx].KeyShare,
			})
			if err != nil {
				firstErr = err
				return
			}
			newResponses[idx] = &mpc.ECDSA2PCKeyGenResponse{KeyShare: resp.NewKeyShare}
		}(i)
	}

	wg.Wait()

	if firstErr != nil {
		return nil, firstErr
	}
	return newResponses, nil
}

// verifyExampleSignature verifies a DER-encoded ECDSA signature using the public key Q from the key share
func verifyExampleSignature(key mpc.ECDSA2PCKey, derSig []byte, digest []byte) error {
	Q, err := key.Q()
	if err != nil {
		return fmt.Errorf("failed to get public key Q: %v", err)
	}
	defer Q.Free()

	// Prefer native verification (matches signing backend)
	if c, err := key.Curve(); err == nil {
		resp := &mpc.ECDSA2PCSignResponse{Signature: derSig}
		if err := resp.Verify(Q, digest, c); err == nil {
			return nil
		}
	}

	x := new(big.Int).SetBytes(Q.GetX())
	y := new(big.Int).SetBytes(Q.GetY())
	pubKey := &ecdsa.PublicKey{Curve: btcec.S256(), X: x, Y: y}

	type ecdsaSignature struct{ R, S *big.Int }
	var s ecdsaSignature
	if _, err := asn1.Unmarshal(derSig, &s); err != nil {
		return fmt.Errorf("failed to parse DER signature: %v", err)
	}
	// Verify with stdlib
	if ecdsa.Verify(pubKey, digest, s.R, s.S) {
		return nil
	}
	// Fallback to btcec ecdsa verification using parsed DER and SEC1-encoded pubkey
	if pk, err := btcec.ParsePubKey(Q.Bytes()); err == nil {
		if sig, err := btcecEcdsa.ParseDERSignature(derSig); err == nil {
			if sig.Verify(digest, pk) {
				return nil
			}
		}
	}
	return fmt.Errorf("invalid signature")
}
