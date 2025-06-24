package main

import (
	"fmt"
	"log"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/zk"
)

func main() {
	fmt.Println("\n=== CB-MPC Zero-Knowledge Discrete Logarithm Example ===")

	// Use the secp256k1 curve for this demo (any supported curve works).
	c, err := curve.NewSecp256k1()
	if err != nil {
		log.Fatalf("creating curve failed: %v", err)
	}
	defer c.Free()

	// Generate a random key pair (w, W = w·G)
	w, W, err := c.RandomKeyPair()
	if err != nil {
		log.Fatalf("key generation failed: %v", err)
	}
	fmt.Printf("Generated key pair on %s – witness length: %d bytes\n", c.String(), len(w.Bytes))

	// Create a proof
	sessionID := []byte("example-session")
	auxiliary := uint64(2025)

	pr, err := zk.ZKUCDLProve(&zk.ZKUCDLProveRequest{PublicKey: W, Witness: w, SessionID: sessionID, Auxiliary: auxiliary})
	if err != nil {
		log.Fatalf("proof generation failed: %v", err)
	}
	fmt.Printf("Proof generated – %d bytes\n", len(pr.Proof))

	// Verify the proof
	vr, err := zk.ZKUCDLVerify(&zk.ZKUCDLVerifyRequest{PublicKey: W, Proof: pr.Proof, SessionID: sessionID, Auxiliary: auxiliary})
	if err != nil {
		log.Fatalf("verification failed: %v", err)
	}
	if !vr.Valid {
		log.Fatalf("❌ proof verification failed")
	}

	fmt.Println("✅ Proof verified successfully")
}
