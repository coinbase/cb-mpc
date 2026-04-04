package main

import (
	"bytes"
	"fmt"
	"log"

	"github.com/coinbase/cb-mpc-go/pkg/mpc"
)

func main() {
	ek, dk, err := mpc.PVEGenerateBasePKERSAKeypair()
	if err != nil {
		log.Fatal(err)
	}

	label := []byte("pve-batch-label")
	plainRows := [][]byte{
		bytes.Repeat([]byte{0x01}, 32),
		bytes.Repeat([]byte{0x02}, 32),
		bytes.Repeat([]byte{0x03}, 32),
	}

	ciphertext, err := mpc.PVEBatchEncrypt(nil, mpc.CurveP256, ek, label, plainRows)
	if err != nil {
		log.Fatal(err)
	}

	qs, err := mpc.PVEBatchGetQs(ciphertext)
	if err != nil {
		log.Fatal(err)
	}

	if err := mpc.PVEBatchVerify(nil, mpc.CurveP256, ek, ciphertext, qs, label); err != nil {
		log.Fatal(err)
	}

	recovered, err := mpc.PVEBatchDecrypt(nil, mpc.CurveP256, dk, ek, ciphertext, label)
	if err != nil {
		log.Fatal(err)
	}

	for i := range plainRows {
		if !bytes.Equal(plainRows[i], recovered[i]) {
			log.Fatalf("row %d mismatch: got=%q want=%q", i, recovered[i], plainRows[i])
		}
	}

	count, err := mpc.PVEBatchGetCount(ciphertext)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("PVE-batch encrypt/verify/decrypt complete\n")
	fmt.Printf("rows: %d, ciphertext bytes: %d\n", count, len(ciphertext))
}
