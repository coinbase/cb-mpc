package mpc

import (
	"bytes"
	"fmt"
	"testing"
)

func TestPVECustomBasePKEFlows(t *testing.T) {
	basePKE := toyBasePKECallbacks()
	ek := []byte("custom-ek")
	dk := []byte("custom-dk")
	label := []byte("custom-base-pke-label")
	plaintext := bytes.Repeat([]byte{0x21}, 32)

	ciphertext, err := PVEEncrypt(basePKE, CurveSecp256k1, ek, label, plaintext)
	if err != nil {
		t.Fatalf("single encrypt: %v", err)
	}

	q, err := PVEGetQ(ciphertext)
	if err != nil {
		t.Fatalf("single get q: %v", err)
	}
	gotLabel, err := PVEGetLabel(ciphertext)
	if err != nil {
		t.Fatalf("single get label: %v", err)
	}
	if !bytes.Equal(gotLabel, label) {
		t.Fatalf("single label mismatch: got=%q want=%q", gotLabel, label)
	}
	if err := PVEVerify(basePKE, CurveSecp256k1, ek, ciphertext, q, label); err != nil {
		t.Fatalf("single verify: %v", err)
	}
	recovered, err := PVEDecrypt(basePKE, CurveSecp256k1, dk, ek, ciphertext, label)
	if err != nil {
		t.Fatalf("single decrypt: %v", err)
	}
	if !bytes.Equal(recovered, plaintext) {
		t.Fatalf("single plaintext mismatch: got=%x want=%x", recovered, plaintext)
	}

	rows := [][]byte{
		bytes.Repeat([]byte{0x31}, 32),
		bytes.Repeat([]byte{0x32}, 32),
		bytes.Repeat([]byte{0x33}, 32),
	}
	batchCiphertext, err := PVEBatchEncrypt(basePKE, CurveSecp256k1, ek, label, rows)
	if err != nil {
		t.Fatalf("batch encrypt: %v", err)
	}
	count, err := PVEBatchGetCount(batchCiphertext)
	if err != nil {
		t.Fatalf("batch get count: %v", err)
	}
	if count != len(rows) {
		t.Fatalf("batch count mismatch: got %d want %d", count, len(rows))
	}
	batchLabel, err := PVEBatchGetLabel(batchCiphertext)
	if err != nil {
		t.Fatalf("batch get label: %v", err)
	}
	if !bytes.Equal(batchLabel, label) {
		t.Fatalf("batch label mismatch: got=%q want=%q", batchLabel, label)
	}
	qs, err := PVEBatchGetQs(batchCiphertext)
	if err != nil {
		t.Fatalf("batch get qs: %v", err)
	}
	if err := PVEBatchVerify(basePKE, CurveSecp256k1, ek, batchCiphertext, qs, label); err != nil {
		t.Fatalf("batch verify: %v", err)
	}
	recoveredRows, err := PVEBatchDecrypt(basePKE, CurveSecp256k1, dk, ek, batchCiphertext, label)
	if err != nil {
		t.Fatalf("batch decrypt: %v", err)
	}
	requireEqualRows(t, "custom base pke batch rows", recoveredRows, rows)

	access := customBasePKEAccessStructure()
	leafNames := []string{"leaf0", "leaf1", "leaf2"}
	leafEKs := [][]byte{
		[]byte("leaf-ek-0"),
		[]byte("leaf-ek-1"),
		[]byte("leaf-ek-2"),
	}
	leafDKs := [][]byte{
		[]byte("leaf-dk-0"),
		[]byte("leaf-dk-1"),
		[]byte("leaf-dk-2"),
	}
	acCiphertext, err := PVEACEncrypt(basePKE, CurveSecp256k1, access, leafNames, leafEKs, label, rows)
	if err != nil {
		t.Fatalf("ac encrypt: %v", err)
	}
	acCount, err := PVEACGetCount(acCiphertext)
	if err != nil {
		t.Fatalf("ac get count: %v", err)
	}
	if acCount != len(rows) {
		t.Fatalf("ac count mismatch: got %d want %d", acCount, len(rows))
	}
	acQs, err := PVEACGetQs(acCiphertext)
	if err != nil {
		t.Fatalf("ac get qs: %v", err)
	}
	if err := PVEACVerify(basePKE, CurveSecp256k1, access, leafNames, leafEKs, acCiphertext, acQs, label); err != nil {
		t.Fatalf("ac verify: %v", err)
	}

	share0, err := PVEACPartyDecryptRow(basePKE, CurveSecp256k1, access, acCiphertext, 0, leafNames[0], leafDKs[0], label)
	if err != nil {
		t.Fatalf("ac partial decrypt leaf0: %v", err)
	}
	share1, err := PVEACPartyDecryptRow(basePKE, CurveSecp256k1, access, acCiphertext, 0, leafNames[1], leafDKs[1], label)
	if err != nil {
		t.Fatalf("ac partial decrypt leaf1: %v", err)
	}
	acRecoveredRows, err := PVEACAggregateToRestoreRow(basePKE, CurveSecp256k1, access, []string{leafNames[0], leafNames[1]}, [][]byte{share0, share1}, acCiphertext, 0, label)
	if err != nil {
		t.Fatalf("ac aggregate: %v", err)
	}
	requireEqualRows(t, "custom base pke ac rows", acRecoveredRows, rows)
}

func TestAccessStructureAndOrConstants(t *testing.T) {
	access := &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeLeaf, LeafName: "leaf0"},
			{Type: AccessNodeLeaf, LeafName: "leaf1"},
			{Type: AccessNodeLeaf, LeafName: "leaf2"},
			{Type: AccessNodeAnd, Children: []int{0, 1}},
			{Type: AccessNodeOr, Children: []int{3, 2}},
		},
		Root: 4,
	}
	leafNames := []string{"leaf0", "leaf1", "leaf2"}
	leafEKs := make([][]byte, len(leafNames))
	for i := range leafNames {
		ek, _, err := PVEGenerateBasePKERSAKeypair()
		if err != nil {
			t.Fatalf("generate leaf key %d: %v", i, err)
		}
		leafEKs[i] = ek
	}

	label := []byte("and-or-access-structure")
	rows := [][]byte{bytes.Repeat([]byte{0x41}, 32)}
	ciphertext, err := PVEACEncrypt(nil, CurveP256, access, leafNames, leafEKs, label, rows)
	if err != nil {
		t.Fatalf("encrypt with AND/OR access structure: %v", err)
	}
	count, err := PVEACGetCount(ciphertext)
	if err != nil {
		t.Fatalf("get count with AND/OR access structure: %v", err)
	}
	if count != len(rows) {
		t.Fatalf("row count mismatch: got %d want %d", count, len(rows))
	}
	qs, err := PVEACGetQs(ciphertext)
	if err != nil {
		t.Fatalf("get qs with AND/OR access structure: %v", err)
	}
	if err := PVEACVerify(nil, CurveP256, access, leafNames, leafEKs, ciphertext, qs, label); err != nil {
		t.Fatalf("verify with AND/OR access structure: %v", err)
	}
}

func toyBasePKECallbacks() *PVEBasePKECallbacks {
	const prefix = "tbpk"
	return &PVEBasePKECallbacks{
		Encrypt: func(_ []byte, _ []byte, plain []byte, rho []byte) ([]byte, error) {
			if len(rho) > 255 {
				return nil, fmt.Errorf("rho too long: %d", len(rho))
			}
			ciphertext := make([]byte, 0, len(prefix)+1+len(rho)+len(plain))
			ciphertext = append(ciphertext, prefix...)
			ciphertext = append(ciphertext, byte(len(rho)))
			ciphertext = append(ciphertext, rho...)
			ciphertext = append(ciphertext, plain...)
			return ciphertext, nil
		},
		Decrypt: func(_ []byte, _ []byte, ciphertext []byte) ([]byte, error) {
			if len(ciphertext) < len(prefix)+1 || !bytes.Equal(ciphertext[:len(prefix)], []byte(prefix)) {
				return nil, fmt.Errorf("unexpected ciphertext format")
			}
			rhoLen := int(ciphertext[len(prefix)])
			if len(ciphertext) < len(prefix)+1+rhoLen {
				return nil, fmt.Errorf("truncated ciphertext")
			}
			return append([]byte(nil), ciphertext[len(prefix)+1+rhoLen:]...), nil
		},
	}
}

func customBasePKEAccessStructure() *AccessStructure {
	return &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeLeaf, LeafName: "leaf0"},
			{Type: AccessNodeLeaf, LeafName: "leaf1"},
			{Type: AccessNodeLeaf, LeafName: "leaf2"},
			{Type: AccessNodeThreshold, ThresholdK: 2, Children: []int{0, 1, 2}},
		},
		Root: 3,
	}
}
