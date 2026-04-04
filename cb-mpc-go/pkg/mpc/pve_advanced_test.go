package mpc

import (
	"bytes"
	"crypto/ecdh"
	"crypto/rand"
	"crypto/rsa"
	"crypto/sha256"
	"fmt"
	"testing"
)

func TestPVEECIESSoftwareFlow(t *testing.T) {
	ek, dk, err := PVEGenerateBasePKEECIESP256Keypair()
	if err != nil {
		t.Fatalf("keygen: %v", err)
	}

	label := []byte("pve-ecies-software-label")
	plaintext := bytes.Repeat([]byte{0x91}, 32)
	ciphertext, err := PVEEncrypt(nil, CurveP256, ek, label, plaintext)
	if err != nil {
		t.Fatalf("encrypt: %v", err)
	}

	q, err := PVEGetQ(ciphertext)
	if err != nil {
		t.Fatalf("get q: %v", err)
	}
	gotLabel, err := PVEGetLabel(ciphertext)
	if err != nil {
		t.Fatalf("get label: %v", err)
	}
	if !bytes.Equal(gotLabel, label) {
		t.Fatalf("ciphertext label mismatch: got=%q want=%q", gotLabel, label)
	}
	if err := PVEVerify(nil, CurveP256, ek, ciphertext, q, label); err != nil {
		t.Fatalf("verify: %v", err)
	}

	recovered, err := PVEDecrypt(nil, CurveP256, dk, ek, ciphertext, label)
	if err != nil {
		t.Fatalf("decrypt: %v", err)
	}
	if !bytes.Equal(recovered, plaintext) {
		t.Fatalf("plaintext mismatch: got=%x want=%x", recovered, plaintext)
	}

	rows := [][]byte{
		bytes.Repeat([]byte{0xA1}, 32),
		bytes.Repeat([]byte{0xA2}, 32),
		bytes.Repeat([]byte{0xA3}, 32),
	}
	batchCiphertext, err := PVEBatchEncrypt(nil, CurveP256, ek, label, rows)
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
	if err := PVEBatchVerify(nil, CurveP256, ek, batchCiphertext, qs, label); err != nil {
		t.Fatalf("batch verify: %v", err)
	}
	recoveredRows, err := PVEBatchDecrypt(nil, CurveP256, dk, ek, batchCiphertext, label)
	if err != nil {
		t.Fatalf("batch decrypt: %v", err)
	}
	requireEqualRows(t, "software ecies batch rows", recoveredRows, rows)
}

func TestPVEAdvancedHSMAndKEMFlows(t *testing.T) {
	t.Run("rsa-modulus-hsm", func(t *testing.T) {
		privateKey, err := rsa.GenerateKey(rand.Reader, 2048)
		if err != nil {
			t.Fatalf("generate RSA key: %v", err)
		}
		ek, err := PVEBasePKERSAEKFromModulus(paddedBigEndianBytes(privateKey.N.Bytes(), 256))
		if err != nil {
			t.Fatalf("build RSA ek from modulus: %v", err)
		}
		callbacks := newRSAOAEPHSMCallbacks(privateKey)
		label := []byte("pve-rsa-hsm-label")
		plaintext := bytes.Repeat([]byte{0xB1}, 32)

		ciphertext, err := PVEEncrypt(nil, CurveP256, ek, label, plaintext)
		if err != nil {
			t.Fatalf("encrypt: %v", err)
		}
		q, err := PVEGetQ(ciphertext)
		if err != nil {
			t.Fatalf("get q: %v", err)
		}
		if err := PVEVerify(nil, CurveP256, ek, ciphertext, q, label); err != nil {
			t.Fatalf("verify: %v", err)
		}
		recovered, err := PVEDecryptRSAOAEPHSM(CurveP256, []byte("rsa-handle"), ek, ciphertext, label, callbacks)
		if err != nil {
			t.Fatalf("hsm decrypt: %v", err)
		}
		if !bytes.Equal(recovered, plaintext) {
			t.Fatalf("plaintext mismatch: got=%x want=%x", recovered, plaintext)
		}

		rows := [][]byte{
			bytes.Repeat([]byte{0xB2}, 32),
			bytes.Repeat([]byte{0xB3}, 32),
		}
		batchCiphertext, err := PVEBatchEncrypt(nil, CurveP256, ek, label, rows)
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
		if err := PVEBatchVerify(nil, CurveP256, ek, batchCiphertext, qs, label); err != nil {
			t.Fatalf("batch verify: %v", err)
		}
		recoveredRows, err := PVEBatchDecryptRSAOAEPHSM(CurveP256, []byte("rsa-handle"), ek, batchCiphertext, label, callbacks)
		if err != nil {
			t.Fatalf("batch hsm decrypt: %v", err)
		}
		requireEqualRows(t, "rsa hsm batch rows", recoveredRows, rows)
	})

	t.Run("ecies-oct-hsm", func(t *testing.T) {
		privateKey, err := ecdh.P256().GenerateKey(rand.Reader)
		if err != nil {
			t.Fatalf("generate ECIES P-256 key: %v", err)
		}
		pubKeyOct := privateKey.PublicKey().Bytes()
		ek, err := PVEBasePKEECIESP256EKFromOCT(pubKeyOct)
		if err != nil {
			t.Fatalf("build ECIES ek from OCT: %v", err)
		}
		callbacks := newECIESP256HSMCallbacks(privateKey)
		label := []byte("pve-ecies-hsm-label")
		plaintext := bytes.Repeat([]byte{0xC1}, 32)

		ciphertext, err := PVEEncrypt(nil, CurveP256, ek, label, plaintext)
		if err != nil {
			t.Fatalf("encrypt: %v", err)
		}
		q, err := PVEGetQ(ciphertext)
		if err != nil {
			t.Fatalf("get q: %v", err)
		}
		if err := PVEVerify(nil, CurveP256, ek, ciphertext, q, label); err != nil {
			t.Fatalf("verify: %v", err)
		}
		recovered, err := PVEDecryptECIESP256HSM(CurveP256, []byte("ecies-handle"), ek, ciphertext, label, callbacks)
		if err != nil {
			t.Fatalf("hsm decrypt: %v", err)
		}
		if !bytes.Equal(recovered, plaintext) {
			t.Fatalf("plaintext mismatch: got=%x want=%x", recovered, plaintext)
		}

		rows := [][]byte{
			bytes.Repeat([]byte{0xC2}, 32),
			bytes.Repeat([]byte{0xC3}, 32),
		}
		batchCiphertext, err := PVEBatchEncrypt(nil, CurveP256, ek, label, rows)
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
		if err := PVEBatchVerify(nil, CurveP256, ek, batchCiphertext, qs, label); err != nil {
			t.Fatalf("batch verify: %v", err)
		}
		recoveredRows, err := PVEBatchDecryptECIESP256HSM(CurveP256, []byte("ecies-handle"), ek, batchCiphertext, label, callbacks)
		if err != nil {
			t.Fatalf("batch hsm decrypt: %v", err)
		}
		requireEqualRows(t, "ecies hsm batch rows", recoveredRows, rows)
	})

	t.Run("custom-kem", func(t *testing.T) {
		kemA := toyKEMCallbacks(0x00)
		kemB := toyKEMCallbacks(0xFF)
		ek := []byte("ek")
		dk := []byte("dk")
		label := []byte("pve-kem-label")
		plaintext := bytes.Repeat([]byte{0xD1}, 32)

		ciphertext, err := PVEEncryptWithKEM(CurveSecp256k1, kemA, ek, label, plaintext)
		if err != nil {
			t.Fatalf("encrypt with KEM: %v", err)
		}
		q, err := PVEGetQ(ciphertext)
		if err != nil {
			t.Fatalf("get q: %v", err)
		}
		if err := PVEVerifyWithKEM(CurveSecp256k1, kemA, ek, ciphertext, q, label); err != nil {
			t.Fatalf("verify with matching KEM: %v", err)
		}
		if err := PVEVerifyWithKEM(CurveSecp256k1, kemB, ek, ciphertext, q, label); err == nil {
			t.Fatalf("expected verify failure with mismatched KEM callbacks")
		}
		recovered, err := PVEDecryptWithKEM(CurveSecp256k1, kemA, dk, ek, ciphertext, label)
		if err != nil {
			t.Fatalf("decrypt with KEM: %v", err)
		}
		if !bytes.Equal(recovered, plaintext) {
			t.Fatalf("plaintext mismatch: got=%x want=%x", recovered, plaintext)
		}
	})

	t.Run("rsa-modulus-ac-hsm", func(t *testing.T) {
		leafNames := testPartyNames("pve-advanced/rsa-ac-hsm", 3)
		access, quorumPartyIndices := thresholdAccessStructure(leafNames)
		leafEKs := make([][]byte, len(leafNames))
		leafKeys := make([]*rsa.PrivateKey, len(leafNames))
		for i := range leafNames {
			privateKey, err := rsa.GenerateKey(rand.Reader, 2048)
			if err != nil {
				t.Fatalf("generate RSA leaf key %d: %v", i, err)
			}
			leafKeys[i] = privateKey
			leafEKs[i], err = PVEBasePKERSAEKFromModulus(paddedBigEndianBytes(privateKey.N.Bytes(), 256))
			if err != nil {
				t.Fatalf("build RSA leaf ek %d: %v", i, err)
			}
		}

		label := []byte("pve-ac-rsa-hsm-label")
		rows := [][]byte{
			bytes.Repeat([]byte{0xE1}, 32),
			bytes.Repeat([]byte{0xE2}, 32),
		}
		ciphertext, err := PVEACEncrypt(nil, CurveP256, access, leafNames, leafEKs, label, rows)
		if err != nil {
			t.Fatalf("encrypt: %v", err)
		}
		count, err := PVEACGetCount(ciphertext)
		if err != nil {
			t.Fatalf("get count: %v", err)
		}
		if count != len(rows) {
			t.Fatalf("count mismatch: got %d want %d", count, len(rows))
		}
		qs, err := PVEACGetQs(ciphertext)
		if err != nil {
			t.Fatalf("get qs: %v", err)
		}
		if err := PVEACVerify(nil, CurveP256, access, leafNames, leafEKs, ciphertext, qs, label); err != nil {
			t.Fatalf("verify: %v", err)
		}

		shares := make([][]byte, len(quorumPartyIndices))
		for i, partyIdx := range quorumPartyIndices {
			callbacks := newRSAOAEPHSMCallbacks(leafKeys[partyIdx])
			share, err := PVEACPartyDecryptRowRSAOAEPHSM(CurveP256, access, ciphertext, 0, leafNames[partyIdx], []byte("rsa-handle"), leafEKs[partyIdx], label, callbacks)
			if err != nil {
				t.Fatalf("partial decrypt party %d: %v", partyIdx, err)
			}
			shares[i] = share
		}

		recoveredRows, err := PVEACAggregateToRestoreRow(nil, CurveP256, access, partyNamesFromIndices(leafNames, quorumPartyIndices), shares, ciphertext, 0, label)
		if err != nil {
			t.Fatalf("aggregate: %v", err)
		}
		requireEqualRows(t, "rsa ac hsm rows", recoveredRows, rows)
	})

	t.Run("ecies-oct-ac-hsm", func(t *testing.T) {
		leafNames := testPartyNames("pve-advanced/ecies-ac-hsm", 3)
		access, quorumPartyIndices := thresholdAccessStructure(leafNames)
		leafEKs := make([][]byte, len(leafNames))
		leafKeys := make([]*ecdh.PrivateKey, len(leafNames))
		for i := range leafNames {
			privateKey, err := ecdh.P256().GenerateKey(rand.Reader)
			if err != nil {
				t.Fatalf("generate ECIES leaf key %d: %v", i, err)
			}
			leafKeys[i] = privateKey
			pubKeyOct := privateKey.PublicKey().Bytes()
			leafEKs[i], err = PVEBasePKEECIESP256EKFromOCT(pubKeyOct)
			if err != nil {
				t.Fatalf("build ECIES leaf ek %d: %v", i, err)
			}
		}

		label := []byte("pve-ac-ecies-hsm-label")
		rows := [][]byte{
			bytes.Repeat([]byte{0xF1}, 32),
			bytes.Repeat([]byte{0xF2}, 32),
		}
		ciphertext, err := PVEACEncrypt(nil, CurveP256, access, leafNames, leafEKs, label, rows)
		if err != nil {
			t.Fatalf("encrypt: %v", err)
		}
		count, err := PVEACGetCount(ciphertext)
		if err != nil {
			t.Fatalf("get count: %v", err)
		}
		if count != len(rows) {
			t.Fatalf("count mismatch: got %d want %d", count, len(rows))
		}
		qs, err := PVEACGetQs(ciphertext)
		if err != nil {
			t.Fatalf("get qs: %v", err)
		}
		if err := PVEACVerify(nil, CurveP256, access, leafNames, leafEKs, ciphertext, qs, label); err != nil {
			t.Fatalf("verify: %v", err)
		}

		shares := make([][]byte, len(quorumPartyIndices))
		for i, partyIdx := range quorumPartyIndices {
			callbacks := newECIESP256HSMCallbacks(leafKeys[partyIdx])
			share, err := PVEACPartyDecryptRowECIESP256HSM(CurveP256, access, ciphertext, 0, leafNames[partyIdx], []byte("ecies-handle"), leafEKs[partyIdx], label, callbacks)
			if err != nil {
				t.Fatalf("partial decrypt party %d: %v", partyIdx, err)
			}
			shares[i] = share
		}

		recoveredRows, err := PVEACAggregateToRestoreRow(nil, CurveP256, access, partyNamesFromIndices(leafNames, quorumPartyIndices), shares, ciphertext, 0, label)
		if err != nil {
			t.Fatalf("aggregate: %v", err)
		}
		requireEqualRows(t, "ecies ac hsm rows", recoveredRows, rows)
	})
}

func paddedBigEndianBytes(in []byte, size int) []byte {
	if len(in) > size {
		return append([]byte(nil), in[len(in)-size:]...)
	}
	out := make([]byte, size)
	copy(out[size-len(in):], in)
	return out
}

func newRSAOAEPHSMCallbacks(privateKey *rsa.PrivateKey) *PVERSAOAEPHSMDecapCallbacks {
	return &PVERSAOAEPHSMDecapCallbacks{
		Decap: func(_ []byte, kemCT []byte) ([]byte, error) {
			return rsa.DecryptOAEP(sha256.New(), rand.Reader, privateKey, kemCT, nil)
		},
	}
}

func newECIESP256HSMCallbacks(privateKey *ecdh.PrivateKey) *PVEECIESP256HSMECDHCallbacks {
	return &PVEECIESP256HSMECDHCallbacks{
		ECDH: func(_ []byte, kemCT []byte) ([]byte, error) {
			return p256SharedX(privateKey, kemCT)
		},
	}
}

func p256SharedX(privateKey *ecdh.PrivateKey, peerOct []byte) ([]byte, error) {
	peerPublicKey, err := ecdh.P256().NewPublicKey(peerOct)
	if err != nil {
		return nil, fmt.Errorf("invalid P-256 public key octets: %w", err)
	}
	sharedSecret, err := privateKey.ECDH(peerPublicKey)
	if err != nil {
		return nil, fmt.Errorf("failed to derive P-256 shared secret: %w", err)
	}
	if len(sharedSecret) != 32 {
		return nil, fmt.Errorf("unexpected shared secret length: %d", len(sharedSecret))
	}
	return append([]byte(nil), sharedSecret...), nil
}

func toyKEMCallbacks(tag byte) *PVEBaseKEMCallbacks {
	return &PVEBaseKEMCallbacks{
		Encap: func(_ []byte, rho32 []byte) ([]byte, []byte, error) {
			if len(rho32) != 32 {
				return nil, nil, fmt.Errorf("unexpected rho length %d", len(rho32))
			}
			kemCT := make([]byte, 32)
			kemSS := make([]byte, 32)
			for i, b := range rho32 {
				v := b ^ tag
				kemCT[i] = v
				kemSS[i] = v
			}
			return kemCT, kemSS, nil
		},
		Decap: func(_ []byte, kemCT []byte) ([]byte, error) {
			if len(kemCT) != 32 {
				return nil, fmt.Errorf("unexpected ciphertext length %d", len(kemCT))
			}
			return append([]byte(nil), kemCT...), nil
		},
	}
}
