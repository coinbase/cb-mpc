package mpc

import (
	"bytes"
	"context"
	"crypto/ed25519"
	"crypto/sha256"
	"encoding/asn1"
	"fmt"
	"math/big"
	"sync"
	"testing"
	"time"

	"github.com/coinbase/cb-mpc-go/pkg/mpctest"
)

func TestECDSA2PCFlow(t *testing.T) {
	ctx := testContext(t)
	sessions := mpctest.NewMockNetwork(2)
	jobs := test2PJobs("api/ecdsa-2p", sessions)
	t.Cleanup(func() {
		for _, session := range sessions {
			_ = session.Close()
		}
	})

	keyBlobs := make([][]byte, 2)
	runPerParty(t, sessions, func(idx int) error {
		keyBlob, err := ECDSA2PDKG(ctx, jobs[idx], CurveSecp256k1)
		if err != nil {
			return fmt.Errorf("dkg: %w", err)
		}
		keyBlobs[idx] = keyBlob
		return nil
	})

	pubKeys := make([][]byte, len(keyBlobs))
	for i, keyBlob := range keyBlobs {
		pubKey, err := ECDSA2PGetPublicKeyCompressed(keyBlob)
		if err != nil {
			t.Fatalf("get pubkey before refresh for party %d: %v", i, err)
		}
		pubKeys[i] = pubKey
	}
	originalPubKey := requireAllEqualNonEmptyBytes(t, "ecdsa public key", pubKeys)

	hash := sha256.Sum256([]byte("ecdsa-flow"))
	signatures := make([][]byte, 2)
	runPerParty(t, sessions, func(idx int) error {
		_, sigDER, err := ECDSA2PSign(ctx, jobs[idx], keyBlobs[idx], hash[:], nil)
		if err != nil {
			return fmt.Errorf("sign: %w", err)
		}
		signatures[idx] = sigDER
		return nil
	})

	sigDER := requireAtLeastOneNonEmptyEqualBytes(t, "ecdsa signature", signatures)
	requireValidECDSADER(t, sigDER)

	refreshed := make([][]byte, 2)
	runPerParty(t, sessions, func(idx int) error {
		keyBlob, err := ECDSA2PRefresh(ctx, jobs[idx], keyBlobs[idx])
		if err != nil {
			return fmt.Errorf("refresh: %w", err)
		}
		refreshed[idx] = keyBlob
		return nil
	})

	refreshedPubKeys := make([][]byte, len(refreshed))
	for i, keyBlob := range refreshed {
		pubKey, err := ECDSA2PGetPublicKeyCompressed(keyBlob)
		if err != nil {
			t.Fatalf("get refreshed pubkey for party %d: %v", i, err)
		}
		refreshedPubKeys[i] = pubKey
	}
	if !bytes.Equal(originalPubKey, requireAllEqualNonEmptyBytes(t, "refreshed ecdsa public key", refreshedPubKeys)) {
		t.Fatalf("ecdsa public key changed across refresh")
	}
}

func TestSchnorr2PCFlow(t *testing.T) {
	ctx := testContext(t)
	sessions := mpctest.NewMockNetwork(2)
	jobs := test2PJobs("api/schnorr-2p", sessions)
	t.Cleanup(func() {
		for _, session := range sessions {
			_ = session.Close()
		}
	})

	keyBlobs := make([][]byte, 2)
	runPerParty(t, sessions, func(idx int) error {
		keyBlob, err := Schnorr2PDKG(ctx, jobs[idx], CurveSecp256k1)
		if err != nil {
			return fmt.Errorf("dkg: %w", err)
		}
		keyBlobs[idx] = keyBlob
		return nil
	})

	xOnlyKeys := make([][]byte, len(keyBlobs))
	for i, keyBlob := range keyBlobs {
		xOnly, err := Schnorr2PExtractPublicKeyXOnly(keyBlob)
		if err != nil {
			t.Fatalf("extract x-only pubkey for party %d: %v", i, err)
		}
		xOnlyKeys[i] = xOnly
	}
	requireAllEqualNonEmptyBytes(t, "schnorr x-only pubkey", xOnlyKeys)

	msgHash := sha256.Sum256([]byte("schnorr-flow"))
	sigs := make([][]byte, 2)
	runPerParty(t, sessions, func(idx int) error {
		sig, err := Schnorr2PSign(ctx, jobs[idx], keyBlobs[idx], msgHash[:])
		if err != nil {
			return fmt.Errorf("sign: %w", err)
		}
		sigs[idx] = sig
		return nil
	})

	requireAtLeastOneNonEmptyEqualBytes(t, "schnorr signature", sigs)
}

func TestEdDSA2PCFlow(t *testing.T) {
	ctx := testContext(t)
	sessions := mpctest.NewMockNetwork(2)
	jobs := test2PJobs("api/eddsa-2p", sessions)
	t.Cleanup(func() {
		for _, session := range sessions {
			_ = session.Close()
		}
	})

	keyBlobs := make([][]byte, 2)
	runPerParty(t, sessions, func(idx int) error {
		keyBlob, err := EdDSA2PDKG(ctx, jobs[idx], CurveEd25519)
		if err != nil {
			return fmt.Errorf("dkg: %w", err)
		}
		keyBlobs[idx] = keyBlob
		return nil
	})

	pubKeys := make([][]byte, len(keyBlobs))
	for i, keyBlob := range keyBlobs {
		pubKey, err := EdDSA2PGetPublicKeyCompressed(keyBlob)
		if err != nil {
			t.Fatalf("get EdDSA pubkey for party %d: %v", i, err)
		}
		pubKeys[i] = pubKey
	}
	pubKey := requireAllEqualNonEmptyBytes(t, "eddsa public key", pubKeys)
	if len(pubKey) != ed25519.PublicKeySize {
		t.Fatalf("unexpected Ed25519 public key size: got %d want %d", len(pubKey), ed25519.PublicKeySize)
	}

	msg := []byte("eddsa-flow")
	sigs := make([][]byte, 2)
	runPerParty(t, sessions, func(idx int) error {
		sig, err := EdDSA2PSign(ctx, jobs[idx], keyBlobs[idx], msg)
		if err != nil {
			return fmt.Errorf("sign: %w", err)
		}
		sigs[idx] = sig
		return nil
	})

	sig := requireAtLeastOneNonEmptyEqualBytes(t, "eddsa signature", sigs)
	if !ed25519.Verify(ed25519.PublicKey(pubKey), msg, sig) {
		t.Fatalf("Ed25519 signature failed verification")
	}
}

func TestTDH2Flow(t *testing.T) {
	ctx := testContext(t)
	const parties = 3
	sessions := mpctest.NewMockNetwork(parties)
	partyNames := testPartyNames("api/tdh2-additive", parties)
	jobs := jobsMP(sessions, partyNames)
	t.Cleanup(func() {
		for _, session := range sessions {
			_ = session.Close()
		}
	})

	publicKeys := make([][]byte, parties)
	publicShares := make([][][]byte, parties)
	privateShares := make([][]byte, parties)
	runPerParty(t, sessions, func(idx int) error {
		publicKey, shares, privateShare, _, err := TDH2DKGAdditive(ctx, jobs[idx], CurveP256)
		if err != nil {
			return fmt.Errorf("dkg: %w", err)
		}
		publicKeys[idx] = publicKey
		publicShares[idx] = shares
		privateShares[idx] = privateShare
		return nil
	})

	plaintext := []byte("tdh2-test-payload")
	label := []byte("tdh2-test-label")
	ciphertext, err := TDH2Encrypt(publicKeys[0], plaintext, label)
	if err != nil {
		t.Fatalf("encrypt: %v", err)
	}
	if err := TDH2Verify(publicKeys[0], ciphertext, label); err != nil {
		t.Fatalf("verify: %v", err)
	}

	partials := make([][]byte, parties)
	runPerParty(t, sessions, func(idx int) error {
		partial, err := TDH2PartialDecrypt(privateShares[idx], ciphertext, label)
		if err != nil {
			return fmt.Errorf("partial decrypt: %w", err)
		}
		partials[idx] = partial
		return nil
	})

	recovered, err := TDH2CombineAdditive(publicKeys[0], publicShares[0], label, partials, ciphertext)
	if err != nil {
		t.Fatalf("combine: %v", err)
	}
	if !bytes.Equal(recovered, plaintext) {
		t.Fatalf("recovered mismatch: got=%q want=%q", recovered, plaintext)
	}
}

func TestPVEBatchFlow(t *testing.T) {
	ek, dk, err := PVEGenerateBasePKERSAKeypair()
	if err != nil {
		t.Fatalf("keygen: %v", err)
	}

	label := []byte("pve-batch-label")
	plainRows := [][]byte{
		bytes.Repeat([]byte{0x01}, 32),
		bytes.Repeat([]byte{0x02}, 32),
		bytes.Repeat([]byte{0x03}, 32),
	}

	ciphertext, err := PVEBatchEncrypt(nil, CurveP256, ek, label, plainRows)
	if err != nil {
		t.Fatalf("encrypt: %v", err)
	}

	qs, err := PVEBatchGetQs(ciphertext)
	if err != nil {
		t.Fatalf("get qs: %v", err)
	}
	if err := PVEBatchVerify(nil, CurveP256, ek, ciphertext, qs, label); err != nil {
		t.Fatalf("verify: %v", err)
	}

	recovered, err := PVEBatchDecrypt(nil, CurveP256, dk, ek, ciphertext, label)
	if err != nil {
		t.Fatalf("decrypt: %v", err)
	}
	for i := range plainRows {
		if !bytes.Equal(plainRows[i], recovered[i]) {
			t.Fatalf("row %d mismatch: got=%q want=%q", i, recovered[i], plainRows[i])
		}
	}
}

func TestECDSAMPAdditiveFlow(t *testing.T) {
	ctx := testContext(t)
	const parties = 3
	sessions := mpctest.NewMockNetwork(parties)
	partyNames := testPartyNames("api/ecdsa-mp-additive", parties)
	jobs := jobsMP(sessions, partyNames)
	t.Cleanup(func() {
		for _, session := range sessions {
			_ = session.Close()
		}
	})

	keyBlobs := make([][]byte, parties)
	sidOuts := make([][]byte, parties)
	runPerParty(t, sessions, func(idx int) error {
		keyBlob, sidOut, err := ECDSAMPDKGAdditive(ctx, jobs[idx], CurveSecp256k1)
		if err != nil {
			return fmt.Errorf("dkg: %w", err)
		}
		keyBlobs[idx] = keyBlob
		sidOuts[idx] = sidOut
		return nil
	})

	pubKeys := make([][]byte, len(keyBlobs))
	for i, keyBlob := range keyBlobs {
		pubKey, err := ECDSAMPGetPublicKeyCompressed(keyBlob)
		if err != nil {
			t.Fatalf("get MP ECDSA pubkey for party %d: %v", i, err)
		}
		pubKeys[i] = pubKey
	}
	originalPubKey := requireAllEqualNonEmptyBytes(t, "mp ecdsa public key", pubKeys)

	hash := sha256.Sum256([]byte("ecdsa-mp-flow"))
	const sigReceiver = 0
	sigs := make([][]byte, parties)
	runPerParty(t, sessions, func(idx int) error {
		sig, err := ECDSAMPSignAdditive(ctx, jobs[idx], keyBlobs[idx], hash[:], sigReceiver)
		if err != nil {
			return fmt.Errorf("sign: %w", err)
		}
		sigs[idx] = sig
		return nil
	})

	if len(sigs[sigReceiver]) == 0 {
		t.Fatal("expected signature on receiver party")
	}
	for i := range sigs {
		if i != sigReceiver && len(sigs[i]) != 0 {
			t.Fatalf("party %d should not have received signature", i)
		}
	}
	requireValidECDSADER(t, sigs[sigReceiver])

	refreshed := make([][]byte, parties)
	runPerParty(t, sessions, func(idx int) error {
		_, keyBlob, err := ECDSAMPRefreshAdditive(ctx, jobs[idx], sidOuts[idx], keyBlobs[idx])
		if err != nil {
			return fmt.Errorf("refresh: %w", err)
		}
		refreshed[idx] = keyBlob
		return nil
	})

	if len(refreshed[0]) == 0 {
		t.Fatal("expected refreshed key blob")
	}

	refreshedPubKeys := make([][]byte, len(refreshed))
	for i, keyBlob := range refreshed {
		pubKey, err := ECDSAMPGetPublicKeyCompressed(keyBlob)
		if err != nil {
			t.Fatalf("get refreshed MP ECDSA pubkey for party %d: %v", i, err)
		}
		refreshedPubKeys[i] = pubKey
	}
	if !bytes.Equal(originalPubKey, requireAllEqualNonEmptyBytes(t, "refreshed mp ecdsa public key", refreshedPubKeys)) {
		t.Fatalf("mp ecdsa public key changed across refresh")
	}
}

func TestEdDSAMPAdditiveFlow(t *testing.T) {
	ctx := testContext(t)
	const parties = 3
	sessions := mpctest.NewMockNetwork(parties)
	partyNames := testPartyNames("api/eddsa-mp-additive", parties)
	jobs := jobsMP(sessions, partyNames)
	t.Cleanup(func() {
		for _, session := range sessions {
			_ = session.Close()
		}
	})

	keyBlobs := make([][]byte, parties)
	runPerParty(t, sessions, func(idx int) error {
		keyBlob, _, err := EdDSAMPDKGAdditive(ctx, jobs[idx], CurveEd25519)
		if err != nil {
			return fmt.Errorf("dkg: %w", err)
		}
		keyBlobs[idx] = keyBlob
		return nil
	})

	pubKeys := make([][]byte, len(keyBlobs))
	for i, keyBlob := range keyBlobs {
		pubKey, err := EdDSAMPGetPublicKeyCompressed(keyBlob)
		if err != nil {
			t.Fatalf("get MP EdDSA pubkey for party %d: %v", i, err)
		}
		pubKeys[i] = pubKey
	}
	pubKey := requireAllEqualNonEmptyBytes(t, "mp eddsa public key", pubKeys)
	if len(pubKey) != ed25519.PublicKeySize {
		t.Fatalf("unexpected MP Ed25519 public key size: got %d want %d", len(pubKey), ed25519.PublicKeySize)
	}

	const sigReceiver = 0
	msg := []byte("eddsa-mp-flow")
	sigs := make([][]byte, parties)
	runPerParty(t, sessions, func(idx int) error {
		sig, err := EdDSAMPSignAdditive(ctx, jobs[idx], keyBlobs[idx], msg, sigReceiver)
		if err != nil {
			return fmt.Errorf("sign: %w", err)
		}
		sigs[idx] = sig
		return nil
	})

	if len(sigs[sigReceiver]) == 0 {
		t.Fatal("expected signature on receiver party")
	}
	for i := range sigs {
		if i != sigReceiver && len(sigs[i]) != 0 {
			t.Fatalf("party %d should not have received signature", i)
		}
	}
	if !ed25519.Verify(ed25519.PublicKey(pubKey), msg, sigs[sigReceiver]) {
		t.Fatalf("MP Ed25519 signature failed verification")
	}
}

func TestSchnorrMPAdditiveFlow(t *testing.T) {
	ctx := testContext(t)
	const parties = 3
	sessions := mpctest.NewMockNetwork(parties)
	partyNames := testPartyNames("api/schnorr-mp-additive", parties)
	jobs := jobsMP(sessions, partyNames)
	t.Cleanup(func() {
		for _, session := range sessions {
			_ = session.Close()
		}
	})

	keyBlobs := make([][]byte, parties)
	runPerParty(t, sessions, func(idx int) error {
		keyBlob, _, err := SchnorrMPDKGAdditive(ctx, jobs[idx], CurveSecp256k1)
		if err != nil {
			return fmt.Errorf("dkg: %w", err)
		}
		keyBlobs[idx] = keyBlob
		return nil
	})

	xOnlyKeys := make([][]byte, len(keyBlobs))
	for i, keyBlob := range keyBlobs {
		xOnly, err := SchnorrMPExtractPublicKeyXOnly(keyBlob)
		if err != nil {
			t.Fatalf("extract MP schnorr x-only pubkey for party %d: %v", i, err)
		}
		xOnlyKeys[i] = xOnly
	}
	requireAllEqualNonEmptyBytes(t, "mp schnorr x-only pubkey", xOnlyKeys)

	msgHash := sha256.Sum256([]byte("schnorr-mp-flow"))
	const sigReceiver = 0
	sigs := make([][]byte, parties)
	runPerParty(t, sessions, func(idx int) error {
		sig, err := SchnorrMPSignAdditive(ctx, jobs[idx], keyBlobs[idx], msgHash[:], sigReceiver)
		if err != nil {
			return fmt.Errorf("sign: %w", err)
		}
		sigs[idx] = sig
		return nil
	})

	if len(sigs[sigReceiver]) == 0 {
		t.Fatal("expected signature on receiver party")
	}
	for i := range sigs {
		if i != sigReceiver && len(sigs[i]) != 0 {
			t.Fatalf("party %d should not have received signature", i)
		}
	}
}

func TestPVESingleFlow(t *testing.T) {
	ek, dk, err := PVEGenerateBasePKERSAKeypair()
	if err != nil {
		t.Fatalf("keygen: %v", err)
	}

	label := []byte("pve-single-label")
	plaintext := bytes.Repeat([]byte{0xAB}, 32)

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

	recovered, err := PVEDecrypt(nil, CurveP256, dk, ek, ciphertext, label)
	if err != nil {
		t.Fatalf("decrypt: %v", err)
	}
	if !bytes.Equal(plaintext, recovered) {
		t.Fatalf("mismatch: got=%q want=%q", recovered, plaintext)
	}
}

func TestPVEACFlow(t *testing.T) {
	const leaves = 3
	leafNames := testPartyNames("api/pve-ac", leaves)

	// Build a 2-of-3 threshold access structure.
	access := &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeLeaf, LeafName: leafNames[0]},
			{Type: AccessNodeLeaf, LeafName: leafNames[1]},
			{Type: AccessNodeLeaf, LeafName: leafNames[2]},
			{Type: AccessNodeThreshold, ThresholdK: 2, Children: []int{0, 1, 2}},
		},
		Root: 3,
	}

	eks := make([][]byte, leaves)
	dks := make([][]byte, leaves)
	for i := 0; i < leaves; i++ {
		ek, dk, err := PVEGenerateBasePKERSAKeypair()
		if err != nil {
			t.Fatalf("keygen %d: %v", i, err)
		}
		eks[i] = ek
		dks[i] = dk
	}

	label := []byte("pve-ac-label")
	plainRows := [][]byte{
		bytes.Repeat([]byte{0x01}, 32),
		bytes.Repeat([]byte{0x02}, 32),
	}

	ciphertext, err := PVEACEncrypt(nil, CurveP256, access, leafNames, eks, label, plainRows)
	if err != nil {
		t.Fatalf("encrypt: %v", err)
	}

	qs, err := PVEACGetQs(ciphertext)
	if err != nil {
		t.Fatalf("get qs: %v", err)
	}
	if err := PVEACVerify(nil, CurveP256, access, leafNames, eks, ciphertext, qs, label); err != nil {
		t.Fatalf("verify: %v", err)
	}

	// Use the first two leaves as the quorum (2-of-3).
	// rowIndex refers to the quorum row in the access structure matrix.
	const rowIndex = 0
	quorumLeafNames := []string{leafNames[0], leafNames[1]}
	quorumIndices := []int{0, 1}

	shares := make([][]byte, len(quorumIndices))
	for j, partyIdx := range quorumIndices {
		share, err := PVEACPartyDecryptRow(nil, CurveP256, access, ciphertext, rowIndex, leafNames[partyIdx], dks[partyIdx], label)
		if err != nil {
			t.Fatalf("party %d decrypt row %d: %v", partyIdx, rowIndex, err)
		}
		shares[j] = share
	}

	recovered, err := PVEACAggregateToRestoreRow(nil, CurveP256, access, quorumLeafNames, shares, ciphertext, rowIndex, label)
	if err != nil {
		t.Fatalf("aggregate: %v", err)
	}
	if len(recovered) != len(plainRows) {
		t.Fatalf("recovered %d rows, want %d", len(recovered), len(plainRows))
	}
	for i := range plainRows {
		if !bytes.Equal(recovered[i], plainRows[i]) {
			t.Fatalf("row %d mismatch: got=%q want=%q", i, recovered[i], plainRows[i])
		}
	}
}

func runPerParty[T any](t *testing.T, parties []T, fn func(idx int) error) {
	t.Helper()

	errCh := make(chan error, len(parties))
	var wg sync.WaitGroup
	for i := range parties {
		wg.Add(1)
		go func(idx int) {
			defer wg.Done()
			if err := fn(idx); err != nil {
				errCh <- fmt.Errorf("party %d: %w", idx, err)
			}
		}(i)
	}

	wg.Wait()
	close(errCh)
	for err := range errCh {
		t.Fatal(err)
	}
}

func testContext(t *testing.T) context.Context {
	t.Helper()
	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	t.Cleanup(cancel)
	return ctx
}

type ecdsaDERSignature struct {
	R *big.Int
	S *big.Int
}

func requireAllEqualNonEmptyBytes(t *testing.T, label string, values [][]byte) []byte {
	t.Helper()
	if len(values) == 0 {
		t.Fatalf("no %s values provided", label)
	}
	ref := values[0]
	if len(ref) == 0 {
		t.Fatalf("%s[0] is empty", label)
	}
	for i := 1; i < len(values); i++ {
		if len(values[i]) == 0 {
			t.Fatalf("%s[%d] is empty", label, i)
		}
		if !bytes.Equal(ref, values[i]) {
			t.Fatalf("%s[%d] does not match %s[0]", label, i, label)
		}
	}
	return ref
}

func requireAtLeastOneNonEmptyEqualBytes(t *testing.T, label string, values [][]byte) []byte {
	t.Helper()
	var ref []byte
	for i, value := range values {
		if len(value) == 0 {
			continue
		}
		if ref == nil {
			ref = value
			continue
		}
		if !bytes.Equal(ref, value) {
			t.Fatalf("%s[%d] does not match earlier non-empty %s", label, i, label)
		}
	}
	if len(ref) == 0 {
		t.Fatalf("expected at least one non-empty %s", label)
	}
	return ref
}

func requireValidECDSADER(t *testing.T, sigDER []byte) {
	t.Helper()
	var sig ecdsaDERSignature
	rest, err := asn1.Unmarshal(sigDER, &sig)
	if err != nil {
		t.Fatalf("failed to parse ECDSA DER signature: %v", err)
	}
	if len(rest) != 0 {
		t.Fatalf("unexpected trailing bytes in ECDSA DER signature: %d", len(rest))
	}
	if sig.R == nil || sig.S == nil || sig.R.Sign() <= 0 || sig.S.Sign() <= 0 {
		t.Fatalf("invalid ECDSA signature integers")
	}
}
