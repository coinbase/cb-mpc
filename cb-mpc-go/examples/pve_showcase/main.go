package main

import (
	"bytes"
	"crypto/ecdh"
	"crypto/rand"
	"fmt"
	"log"

	"github.com/coinbase/cb-mpc-go/pkg/mpc"
)

func main() {
	fmt.Println("PVE showcase using only pkg/mpc APIs")

	if err := demoCustomBasePKE(); err != nil {
		log.Fatal(err)
	}
	if err := demoImportedECIESHSM(); err != nil {
		log.Fatal(err)
	}
}

func demoCustomBasePKE() error {
	fmt.Println("\n=== Custom Base PKE Callbacks ===")

	basePKE := toyBasePKECallbacks()
	ek := []byte("custom-ek")
	dk := []byte("custom-dk")
	label := []byte("pve-showcase-custom-base-pke")
	plaintext := bytes.Repeat([]byte{0x21}, 32)

	ciphertext, err := mpc.PVEEncrypt(basePKE, mpc.CurveSecp256k1, ek, label, plaintext)
	if err != nil {
		return fmt.Errorf("single encrypt: %w", err)
	}
	gotLabel, err := mpc.PVEGetLabel(ciphertext)
	if err != nil {
		return fmt.Errorf("single get label: %w", err)
	}
	if !bytes.Equal(gotLabel, label) {
		return fmt.Errorf("single label mismatch: got=%q want=%q", gotLabel, label)
	}
	qCompressed, err := mpc.PVEGetQ(ciphertext)
	if err != nil {
		return fmt.Errorf("single get q: %w", err)
	}
	if err := mpc.PVEVerify(basePKE, mpc.CurveSecp256k1, ek, ciphertext, qCompressed, label); err != nil {
		return fmt.Errorf("single verify: %w", err)
	}
	recovered, err := mpc.PVEDecrypt(basePKE, mpc.CurveSecp256k1, dk, ek, ciphertext, label)
	if err != nil {
		return fmt.Errorf("single decrypt: %w", err)
	}
	fmt.Printf("single decrypt ok? %v\n", bytes.Equal(recovered, plaintext))

	rows := [][]byte{
		bytes.Repeat([]byte{0x31}, 32),
		bytes.Repeat([]byte{0x32}, 32),
		bytes.Repeat([]byte{0x33}, 32),
	}
	batchCiphertext, err := mpc.PVEBatchEncrypt(basePKE, mpc.CurveSecp256k1, ek, label, rows)
	if err != nil {
		return fmt.Errorf("batch encrypt: %w", err)
	}
	count, err := mpc.PVEBatchGetCount(batchCiphertext)
	if err != nil {
		return fmt.Errorf("batch get count: %w", err)
	}
	batchLabel, err := mpc.PVEBatchGetLabel(batchCiphertext)
	if err != nil {
		return fmt.Errorf("batch get label: %w", err)
	}
	if !bytes.Equal(batchLabel, label) {
		return fmt.Errorf("batch label mismatch: got=%q want=%q", batchLabel, label)
	}
	batchQs, err := mpc.PVEBatchGetQs(batchCiphertext)
	if err != nil {
		return fmt.Errorf("batch get qs: %w", err)
	}
	if err := mpc.PVEBatchVerify(basePKE, mpc.CurveSecp256k1, ek, batchCiphertext, batchQs, label); err != nil {
		return fmt.Errorf("batch verify: %w", err)
	}
	recoveredRows, err := mpc.PVEBatchDecrypt(basePKE, mpc.CurveSecp256k1, dk, ek, batchCiphertext, label)
	if err != nil {
		return fmt.Errorf("batch decrypt: %w", err)
	}
	if err := requireEqualRows("custom base pke batch", recoveredRows, rows); err != nil {
		return err
	}
	fmt.Printf("batch decrypt ok? %v (rows=%d)\n", true, count)

	leafNames := []string{"p1", "p2", "p3"}
	leafEKs := [][]byte{
		[]byte("leaf-ek-1"),
		[]byte("leaf-ek-2"),
		[]byte("leaf-ek-3"),
	}
	leafDKs := [][]byte{
		[]byte("leaf-dk-1"),
		[]byte("leaf-dk-2"),
		[]byte("leaf-dk-3"),
	}
	access := threshold2of3AccessStructure(leafNames)
	acCiphertext, err := mpc.PVEACEncrypt(basePKE, mpc.CurveSecp256k1, access, leafNames, leafEKs, label, rows)
	if err != nil {
		return fmt.Errorf("ac encrypt: %w", err)
	}
	acCount, err := mpc.PVEACGetCount(acCiphertext)
	if err != nil {
		return fmt.Errorf("ac get count: %w", err)
	}
	acQs, err := mpc.PVEACGetQs(acCiphertext)
	if err != nil {
		return fmt.Errorf("ac get qs: %w", err)
	}
	if err := mpc.PVEACVerify(basePKE, mpc.CurveSecp256k1, access, leafNames, leafEKs, acCiphertext, acQs, label); err != nil {
		return fmt.Errorf("ac verify: %w", err)
	}
	shareP1, err := mpc.PVEACPartyDecryptRow(basePKE, mpc.CurveSecp256k1, access, acCiphertext, 0, leafNames[0], leafDKs[0], label)
	if err != nil {
		return fmt.Errorf("ac partial decrypt p1: %w", err)
	}
	shareP3, err := mpc.PVEACPartyDecryptRow(basePKE, mpc.CurveSecp256k1, access, acCiphertext, 0, leafNames[2], leafDKs[2], label)
	if err != nil {
		return fmt.Errorf("ac partial decrypt p3: %w", err)
	}
	acRecoveredRows, err := mpc.PVEACAggregateToRestoreRow(basePKE, mpc.CurveSecp256k1, access, []string{leafNames[0], leafNames[2]}, [][]byte{shareP1, shareP3}, acCiphertext, 0, label)
	if err != nil {
		return fmt.Errorf("ac aggregate: %w", err)
	}
	if err := requireEqualRows("custom base pke ac", acRecoveredRows, rows); err != nil {
		return err
	}
	fmt.Printf("access-structure recover ok? %v (rows=%d)\n", true, acCount)
	return nil
}

func demoImportedECIESHSM() error {
	fmt.Println("\n=== Imported ECIES(P-256) Public Keys + Simulated HSM ECDH Callback ===")

	hsm := newFakeHSM()
	callbacks := hsm.Callbacks()
	label := []byte("pve-showcase-ecies-hsm")

	const singleHandle = "ecies-single"
	singlePubKeyOct, err := hsm.Generate(singleHandle)
	if err != nil {
		return fmt.Errorf("generate single HSM key: %w", err)
	}
	singleEK, err := mpc.PVEBasePKEECIESP256EKFromOCT(singlePubKeyOct)
	if err != nil {
		return fmt.Errorf("build single imported ek: %w", err)
	}
	plaintext := bytes.Repeat([]byte{0x41}, 32)
	ciphertext, err := mpc.PVEEncrypt(nil, mpc.CurveP256, singleEK, label, plaintext)
	if err != nil {
		return fmt.Errorf("single imported encrypt: %w", err)
	}
	gotLabel, err := mpc.PVEGetLabel(ciphertext)
	if err != nil {
		return fmt.Errorf("single imported get label: %w", err)
	}
	if !bytes.Equal(gotLabel, label) {
		return fmt.Errorf("single imported label mismatch: got=%q want=%q", gotLabel, label)
	}
	qCompressed, err := mpc.PVEGetQ(ciphertext)
	if err != nil {
		return fmt.Errorf("single imported get q: %w", err)
	}
	if err := mpc.PVEVerify(nil, mpc.CurveP256, singleEK, ciphertext, qCompressed, label); err != nil {
		return fmt.Errorf("single imported verify: %w", err)
	}
	recovered, err := mpc.PVEDecryptECIESP256HSM(mpc.CurveP256, []byte(singleHandle), singleEK, ciphertext, label, callbacks)
	if err != nil {
		return fmt.Errorf("single imported HSM decrypt: %w", err)
	}
	fmt.Printf("single HSM decrypt ok? %v\n", bytes.Equal(recovered, plaintext))

	const batchHandle = "ecies-batch"
	batchPubKeyOct, err := hsm.Generate(batchHandle)
	if err != nil {
		return fmt.Errorf("generate batch HSM key: %w", err)
	}
	batchEK, err := mpc.PVEBasePKEECIESP256EKFromOCT(batchPubKeyOct)
	if err != nil {
		return fmt.Errorf("build batch imported ek: %w", err)
	}
	rows := [][]byte{
		bytes.Repeat([]byte{0x51}, 32),
		bytes.Repeat([]byte{0x52}, 32),
		bytes.Repeat([]byte{0x53}, 32),
	}
	batchCiphertext, err := mpc.PVEBatchEncrypt(nil, mpc.CurveP256, batchEK, label, rows)
	if err != nil {
		return fmt.Errorf("batch imported encrypt: %w", err)
	}
	count, err := mpc.PVEBatchGetCount(batchCiphertext)
	if err != nil {
		return fmt.Errorf("batch imported get count: %w", err)
	}
	batchLabel, err := mpc.PVEBatchGetLabel(batchCiphertext)
	if err != nil {
		return fmt.Errorf("batch imported get label: %w", err)
	}
	if !bytes.Equal(batchLabel, label) {
		return fmt.Errorf("batch imported label mismatch: got=%q want=%q", batchLabel, label)
	}
	batchQs, err := mpc.PVEBatchGetQs(batchCiphertext)
	if err != nil {
		return fmt.Errorf("batch imported get qs: %w", err)
	}
	if err := mpc.PVEBatchVerify(nil, mpc.CurveP256, batchEK, batchCiphertext, batchQs, label); err != nil {
		return fmt.Errorf("batch imported verify: %w", err)
	}
	recoveredRows, err := mpc.PVEBatchDecryptECIESP256HSM(mpc.CurveP256, []byte(batchHandle), batchEK, batchCiphertext, label, callbacks)
	if err != nil {
		return fmt.Errorf("batch imported HSM decrypt: %w", err)
	}
	if err := requireEqualRows("imported ecies HSM batch", recoveredRows, rows); err != nil {
		return err
	}
	fmt.Printf("batch HSM decrypt ok? %v (rows=%d)\n", true, count)

	leafNames := []string{"p1", "p2", "p3"}
	access := threshold2of3AccessStructure(leafNames)
	leafHandles := []string{"ecies-ac-p1", "ecies-ac-p2", "ecies-ac-p3"}
	leafEKs := make([][]byte, len(leafHandles))
	for i, handle := range leafHandles {
		pubKeyOct, err := hsm.Generate(handle)
		if err != nil {
			return fmt.Errorf("generate AC HSM key %s: %w", handle, err)
		}
		leafEKs[i], err = mpc.PVEBasePKEECIESP256EKFromOCT(pubKeyOct)
		if err != nil {
			return fmt.Errorf("build AC imported ek %s: %w", handle, err)
		}
	}
	acRows := [][]byte{
		bytes.Repeat([]byte{0x61}, 32),
		bytes.Repeat([]byte{0x62}, 32),
	}
	acCiphertext, err := mpc.PVEACEncrypt(nil, mpc.CurveP256, access, leafNames, leafEKs, label, acRows)
	if err != nil {
		return fmt.Errorf("AC imported encrypt: %w", err)
	}
	acCount, err := mpc.PVEACGetCount(acCiphertext)
	if err != nil {
		return fmt.Errorf("AC imported get count: %w", err)
	}
	acQs, err := mpc.PVEACGetQs(acCiphertext)
	if err != nil {
		return fmt.Errorf("AC imported get qs: %w", err)
	}
	if err := mpc.PVEACVerify(nil, mpc.CurveP256, access, leafNames, leafEKs, acCiphertext, acQs, label); err != nil {
		return fmt.Errorf("AC imported verify: %w", err)
	}
	shareP1, err := mpc.PVEACPartyDecryptRowECIESP256HSM(mpc.CurveP256, access, acCiphertext, 0, leafNames[0], []byte(leafHandles[0]), leafEKs[0], label, callbacks)
	if err != nil {
		return fmt.Errorf("AC imported partial decrypt p1: %w", err)
	}
	shareP3, err := mpc.PVEACPartyDecryptRowECIESP256HSM(mpc.CurveP256, access, acCiphertext, 0, leafNames[2], []byte(leafHandles[2]), leafEKs[2], label, callbacks)
	if err != nil {
		return fmt.Errorf("AC imported partial decrypt p3: %w", err)
	}
	acRecoveredRows, err := mpc.PVEACAggregateToRestoreRow(nil, mpc.CurveP256, access, []string{leafNames[0], leafNames[2]}, [][]byte{shareP1, shareP3}, acCiphertext, 0, label)
	if err != nil {
		return fmt.Errorf("AC imported aggregate: %w", err)
	}
	if err := requireEqualRows("imported ecies HSM AC", acRecoveredRows, acRows); err != nil {
		return err
	}
	fmt.Printf("access-structure HSM recover ok? %v (rows=%d)\n", true, acCount)
	return nil
}

func threshold2of3AccessStructure(leafNames []string) *mpc.AccessStructure {
	return &mpc.AccessStructure{
		Nodes: []mpc.AccessStructureNode{
			{Type: mpc.AccessNodeLeaf, LeafName: leafNames[0]},
			{Type: mpc.AccessNodeLeaf, LeafName: leafNames[1]},
			{Type: mpc.AccessNodeLeaf, LeafName: leafNames[2]},
			{Type: mpc.AccessNodeThreshold, ThresholdK: 2, Children: []int{0, 1, 2}},
		},
		Root: 3,
	}
}

func requireEqualRows(label string, got, want [][]byte) error {
	if len(got) != len(want) {
		return fmt.Errorf("%s row count mismatch: got %d want %d", label, len(got), len(want))
	}
	for i := range want {
		if !bytes.Equal(got[i], want[i]) {
			return fmt.Errorf("%s row %d mismatch", label, i)
		}
	}
	return nil
}

func toyBasePKECallbacks() *mpc.PVEBasePKECallbacks {
	const prefix = "tbpk"
	return &mpc.PVEBasePKECallbacks{
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

type fakeHSM struct {
	keys map[string]*ecdh.PrivateKey
}

func newFakeHSM() *fakeHSM {
	return &fakeHSM{keys: make(map[string]*ecdh.PrivateKey)}
}

func (h *fakeHSM) Generate(handle string) ([]byte, error) {
	if _, exists := h.keys[handle]; exists {
		return nil, fmt.Errorf("duplicate HSM key handle %q", handle)
	}
	privateKey, err := ecdh.P256().GenerateKey(rand.Reader)
	if err != nil {
		return nil, err
	}
	h.keys[handle] = privateKey
	return append([]byte(nil), privateKey.PublicKey().Bytes()...), nil
}

func (h *fakeHSM) Callbacks() *mpc.PVEECIESP256HSMECDHCallbacks {
	return &mpc.PVEECIESP256HSMECDHCallbacks{
		ECDH: func(dkHandle, kemCT []byte) ([]byte, error) {
			return h.ECDHX32(string(dkHandle), kemCT)
		},
	}
}

func (h *fakeHSM) ECDHX32(handle string, peerPublicKey []byte) ([]byte, error) {
	privateKey, ok := h.keys[handle]
	if !ok {
		return nil, fmt.Errorf("unknown HSM key handle %q", handle)
	}
	publicKey, err := ecdh.P256().NewPublicKey(peerPublicKey)
	if err != nil {
		return nil, err
	}
	sharedSecret, err := privateKey.ECDH(publicKey)
	if err != nil {
		return nil, err
	}
	if len(sharedSecret) != 32 {
		return nil, fmt.Errorf("unexpected ECDH output size %d", len(sharedSecret))
	}
	return append([]byte(nil), sharedSecret...), nil
}
