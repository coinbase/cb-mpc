package mpc

import (
	"bytes"
	"crypto/ed25519"
	"crypto/sha256"
	"fmt"
	"testing"

	secp256k1 "github.com/decred/dcrd/dcrec/secp256k1/v4"
	secp256k1ecdsa "github.com/decred/dcrd/dcrec/secp256k1/v4/ecdsa"

	"github.com/coinbase/cb-mpc-go/pkg/mpctest"
)

func TestECDSAMPACFlow(t *testing.T) {
	ctx := testContext(t)
	sessions := mpctest.NewMockNetwork(3)
	partyNames := testPartyNames("ac/ecdsa-mp", len(sessions))
	access, quorumPartyIndices := thresholdAccessStructure(partyNames)
	quorumPartyNames := mustPartyNamesFromIndices(t, partyNames, quorumPartyIndices)
	jobs := jobsMP(sessions, partyNames)
	t.Cleanup(func() { closeMockSessions(t, sessions) })

	acKeyBlobs := make([][]byte, len(sessions))
	sidOuts := make([][]byte, len(sessions))
	runPerParty(t, sessions, func(idx int) error {
		acKeyBlob, sidOut, err := ECDSAMPDKGAC(ctx, jobs[idx], CurveSecp256k1, nil, access, quorumPartyNames)
		if err != nil {
			return fmt.Errorf("dkg ac: %w", err)
		}
		acKeyBlobs[idx] = acKeyBlob
		sidOuts[idx] = sidOut
		return nil
	})
	requireAllEqualNonEmptyBytes(t, "ecdsa mp ac sid", sidOuts)

	pubKeys := make([][]byte, len(acKeyBlobs))
	publicShares := make([][]byte, len(acKeyBlobs))
	privateScalars := make([][]byte, len(acKeyBlobs))
	for i, keyBlob := range acKeyBlobs {
		pubKey, err := ECDSAMPGetPublicKeyCompressed(keyBlob)
		if err != nil {
			t.Fatalf("get public key for party %d: %v", i, err)
		}
		publicShare, privateScalar := requireExtractECDSAMPShareMaterial(t, keyBlob, i)
		pubKeys[i] = pubKey
		publicShares[i] = publicShare
		privateScalars[i] = privateScalar
	}
	originalPubKey := requireAllEqualNonEmptyBytes(t, "ecdsa mp ac public key", pubKeys)

	signSessions := mpctest.NewMockNetwork(len(quorumPartyIndices))
	signJobs := jobsMP(signSessions, quorumPartyNames)
	t.Cleanup(func() { closeMockSessions(t, signSessions) })

	hash := sha256.Sum256([]byte("ecdsa-mp-ac-flow"))
	sigs := make([][]byte, len(signSessions))
	runPerParty(t, signSessions, func(idx int) error {
		sig, err := ECDSAMPSignAC(ctx, signJobs[idx], acKeyBlobs[idx], access, hash[:], 0)
		if err != nil {
			return fmt.Errorf("sign ac: %w", err)
		}
		sigs[idx] = sig
		return nil
	})
	requireValidECDSADER(t, requireSignatureOnlyOnReceiver(t, "ecdsa mp ac signature", sigs, 0))

	refreshedKeyBlobs := make([][]byte, len(sessions))
	refreshedSIDOuts := make([][]byte, len(sessions))
	runPerParty(t, sessions, func(idx int) error {
		sidOut, refreshedKeyBlob, err := ECDSAMPRefreshAC(ctx, jobs[idx], sidOuts[idx], acKeyBlobs[idx], access, quorumPartyNames)
		if err != nil {
			return fmt.Errorf("refresh ac: %w", err)
		}
		refreshedSIDOuts[idx] = sidOut
		refreshedKeyBlobs[idx] = refreshedKeyBlob
		return nil
	})
	requireAllEqualNonEmptyBytes(t, "ecdsa mp ac refresh sid", refreshedSIDOuts)

	refreshedPubKeys := make([][]byte, len(refreshedKeyBlobs))
	refreshedPublicShares := make([][]byte, len(refreshedKeyBlobs))
	refreshedPrivateScalars := make([][]byte, len(refreshedKeyBlobs))
	for i, keyBlob := range refreshedKeyBlobs {
		pubKey, err := ECDSAMPGetPublicKeyCompressed(keyBlob)
		if err != nil {
			t.Fatalf("get refreshed public key for party %d: %v", i, err)
		}
		publicShare, privateScalar := requireExtractECDSAMPShareMaterial(t, keyBlob, i)
		refreshedPubKeys[i] = pubKey
		refreshedPublicShares[i] = publicShare
		refreshedPrivateScalars[i] = privateScalar
	}
	if !bytes.Equal(originalPubKey, requireAllEqualNonEmptyBytes(t, "refreshed ecdsa mp ac public key", refreshedPubKeys)) {
		t.Fatalf("ecdsa mp ac public key changed across refresh")
	}
	for i := range refreshedKeyBlobs {
		if bytes.Equal(publicShares[i], refreshedPublicShares[i]) {
			t.Fatalf("party %d public share did not change across refresh", i)
		}
		if bytes.Equal(privateScalars[i], refreshedPrivateScalars[i]) {
			t.Fatalf("party %d private scalar did not change across refresh", i)
		}
	}

	sigs = make([][]byte, len(signSessions))
	runPerParty(t, signSessions, func(idx int) error {
		sig, err := ECDSAMPSignAC(ctx, signJobs[idx], refreshedKeyBlobs[idx], access, hash[:], 0)
		if err != nil {
			return fmt.Errorf("sign refreshed ac: %w", err)
		}
		sigs[idx] = sig
		return nil
	})
	requireValidECDSADER(t, requireSignatureOnlyOnReceiver(t, "refreshed ecdsa mp ac signature", sigs, 0))
}

func TestECDSAMPACFlowSupportsDifferentSigningQuorums(t *testing.T) {
	ctx := testContext(t)
	sessions := mpctest.NewMockNetwork(3)
	partyNames := testPartyNames("ac/ecdsa-mp-flex-sign", len(sessions))
	access, dkgQuorumIndices := thresholdAccessStructure(partyNames)
	dkgQuorumPartyNames := mustPartyNamesFromIndices(t, partyNames, dkgQuorumIndices)
	jobs := jobsMP(sessions, partyNames)
	t.Cleanup(func() { closeMockSessions(t, sessions) })

	acKeyBlobs := make([][]byte, len(sessions))
	runPerParty(t, sessions, func(idx int) error {
		acKeyBlob, _, err := ECDSAMPDKGAC(ctx, jobs[idx], CurveSecp256k1, nil, access, dkgQuorumPartyNames)
		if err != nil {
			return fmt.Errorf("dkg ac: %w", err)
		}
		acKeyBlobs[idx] = acKeyBlob
		return nil
	})

	pubKeys := make([][]byte, len(acKeyBlobs))
	for i, keyBlob := range acKeyBlobs {
		pubKey, err := ECDSAMPGetPublicKeyCompressed(keyBlob)
		if err != nil {
			t.Fatalf("get public key for party %d: %v", i, err)
		}
		pubKeys[i] = pubKey
	}
	publicKey := requireAllEqualNonEmptyBytes(t, "ecdsa mp ac flexible sign public key", pubKeys)

	testCases := []struct {
		name          string
		quorumIndices []int
	}{
		{name: "parties-0-1", quorumIndices: []int{0, 1}},
		{name: "parties-0-2", quorumIndices: []int{0, 2}},
		{name: "parties-1-2", quorumIndices: []int{1, 2}},
	}
	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			signSessions := mpctest.NewMockNetwork(len(tc.quorumIndices))
			signPartyNames := mustPartyNamesFromIndices(t, partyNames, tc.quorumIndices)
			signJobs := jobsMP(signSessions, signPartyNames)
			defer closeMockSessions(t, signSessions)

			hash := sha256.Sum256([]byte("ecdsa-mp-ac-flex-sign/" + tc.name))
			sigs := make([][]byte, len(signSessions))
			runPerParty(t, signSessions, func(idx int) error {
				fullPartyIdx := tc.quorumIndices[idx]
				sig, err := ECDSAMPSignAC(ctx, signJobs[idx], acKeyBlobs[fullPartyIdx], access, hash[:], 0)
				if err != nil {
					return fmt.Errorf("sign ac: %w", err)
				}
				sigs[idx] = sig
				return nil
			})

			sig := requireSignatureOnlyOnReceiver(t, "ecdsa mp ac flexible sign signature", sigs, 0)
			requireValidECDSADER(t, sig)
			requireValidSecp256k1ECDSASignature(t, publicKey, hash[:], sig)
		})
	}
}

func TestEdDSAMPACFlow(t *testing.T) {
	ctx := testContext(t)
	sessions := mpctest.NewMockNetwork(3)
	partyNames := testPartyNames("ac/eddsa-mp", len(sessions))
	access, quorumPartyIndices := thresholdAccessStructure(partyNames)
	quorumPartyNames := mustPartyNamesFromIndices(t, partyNames, quorumPartyIndices)
	jobs := jobsMP(sessions, partyNames)
	t.Cleanup(func() { closeMockSessions(t, sessions) })

	acKeyBlobs := make([][]byte, len(sessions))
	sidOuts := make([][]byte, len(sessions))
	runPerParty(t, sessions, func(idx int) error {
		acKeyBlob, sidOut, err := EdDSAMPDKGAC(ctx, jobs[idx], CurveEd25519, nil, access, quorumPartyNames)
		if err != nil {
			return fmt.Errorf("dkg ac: %w", err)
		}
		acKeyBlobs[idx] = acKeyBlob
		sidOuts[idx] = sidOut
		return nil
	})
	requireAllEqualNonEmptyBytes(t, "eddsa mp ac sid", sidOuts)

	pubKeys := make([][]byte, len(acKeyBlobs))
	for i, keyBlob := range acKeyBlobs {
		pubKey, err := EdDSAMPGetPublicKeyCompressed(keyBlob)
		if err != nil {
			t.Fatalf("get public key for party %d: %v", i, err)
		}
		pubKeys[i] = pubKey
	}
	pubKey := requireAllEqualNonEmptyBytes(t, "eddsa mp ac public key", pubKeys)

	signSessions := mpctest.NewMockNetwork(len(quorumPartyIndices))
	signJobs := jobsMP(signSessions, quorumPartyNames)
	t.Cleanup(func() { closeMockSessions(t, signSessions) })

	msg := []byte("eddsa-mp-ac-flow")
	sigs := make([][]byte, len(signSessions))
	runPerParty(t, signSessions, func(idx int) error {
		sig, err := EdDSAMPSignAC(ctx, signJobs[idx], acKeyBlobs[idx], access, msg, 0)
		if err != nil {
			return fmt.Errorf("sign ac: %w", err)
		}
		sigs[idx] = sig
		return nil
	})
	sig := requireSignatureOnlyOnReceiver(t, "eddsa mp ac signature", sigs, 0)
	if !ed25519.Verify(ed25519.PublicKey(pubKey), msg, sig) {
		t.Fatalf("EdDSA MP AC signature failed verification")
	}

	refreshedKeyBlobs := make([][]byte, len(sessions))
	refreshedSIDOuts := make([][]byte, len(sessions))
	runPerParty(t, sessions, func(idx int) error {
		sidOut, refreshedKeyBlob, err := EdDSAMPRefreshAC(ctx, jobs[idx], sidOuts[idx], acKeyBlobs[idx], access, quorumPartyNames)
		if err != nil {
			return fmt.Errorf("refresh ac: %w", err)
		}
		refreshedSIDOuts[idx] = sidOut
		refreshedKeyBlobs[idx] = refreshedKeyBlob
		return nil
	})
	requireAllEqualNonEmptyBytes(t, "eddsa mp ac refresh sid", refreshedSIDOuts)

	refreshedPubKeys := make([][]byte, len(refreshedKeyBlobs))
	for i, keyBlob := range refreshedKeyBlobs {
		refreshedPubKey, err := EdDSAMPGetPublicKeyCompressed(keyBlob)
		if err != nil {
			t.Fatalf("get refreshed public key for party %d: %v", i, err)
		}
		refreshedPubKeys[i] = refreshedPubKey
	}
	if !bytes.Equal(pubKey, requireAllEqualNonEmptyBytes(t, "refreshed eddsa mp ac public key", refreshedPubKeys)) {
		t.Fatalf("eddsa mp ac public key changed across refresh")
	}

	sigs = make([][]byte, len(signSessions))
	runPerParty(t, signSessions, func(idx int) error {
		sig, err := EdDSAMPSignAC(ctx, signJobs[idx], refreshedKeyBlobs[idx], access, msg, 0)
		if err != nil {
			return fmt.Errorf("sign refreshed ac: %w", err)
		}
		sigs[idx] = sig
		return nil
	})
	sig = requireSignatureOnlyOnReceiver(t, "refreshed eddsa mp ac signature", sigs, 0)
	if !ed25519.Verify(ed25519.PublicKey(pubKey), msg, sig) {
		t.Fatalf("refreshed EdDSA MP AC signature failed verification")
	}
}

func TestSchnorrMPACFlow(t *testing.T) {
	ctx := testContext(t)
	sessions := mpctest.NewMockNetwork(3)
	partyNames := testPartyNames("ac/schnorr-mp", len(sessions))
	access, quorumPartyIndices := thresholdAccessStructure(partyNames)
	quorumPartyNames := mustPartyNamesFromIndices(t, partyNames, quorumPartyIndices)
	jobs := jobsMP(sessions, partyNames)
	t.Cleanup(func() { closeMockSessions(t, sessions) })

	acKeyBlobs := make([][]byte, len(sessions))
	sidOuts := make([][]byte, len(sessions))
	runPerParty(t, sessions, func(idx int) error {
		acKeyBlob, sidOut, err := SchnorrMPDKGAC(ctx, jobs[idx], CurveSecp256k1, nil, access, quorumPartyNames)
		if err != nil {
			return fmt.Errorf("dkg ac: %w", err)
		}
		acKeyBlobs[idx] = acKeyBlob
		sidOuts[idx] = sidOut
		return nil
	})
	requireAllEqualNonEmptyBytes(t, "schnorr mp ac sid", sidOuts)

	xOnlyKeys := make([][]byte, len(acKeyBlobs))
	for i, keyBlob := range acKeyBlobs {
		xOnly, err := SchnorrMPExtractPublicKeyXOnly(keyBlob)
		if err != nil {
			t.Fatalf("extract x-only public key for party %d: %v", i, err)
		}
		xOnlyKeys[i] = xOnly
	}
	xOnlyKey := requireAllEqualNonEmptyBytes(t, "schnorr mp ac x-only public key", xOnlyKeys)

	signSessions := mpctest.NewMockNetwork(len(quorumPartyIndices))
	signJobs := jobsMP(signSessions, quorumPartyNames)
	t.Cleanup(func() { closeMockSessions(t, signSessions) })

	msgHash := sha256.Sum256([]byte("schnorr-mp-ac-flow"))
	sigs := make([][]byte, len(signSessions))
	runPerParty(t, signSessions, func(idx int) error {
		sig, err := SchnorrMPSignAC(ctx, signJobs[idx], acKeyBlobs[idx], access, msgHash[:], 0)
		if err != nil {
			return fmt.Errorf("sign ac: %w", err)
		}
		sigs[idx] = sig
		return nil
	})
	sig := requireSignatureOnlyOnReceiver(t, "schnorr mp ac signature", sigs, 0)
	if len(sig) != 64 {
		t.Fatalf("unexpected Schnorr MP AC signature length: got %d want 64", len(sig))
	}
	if len(xOnlyKey) != 32 {
		t.Fatalf("unexpected Schnorr MP AC x-only public key length: got %d want 32", len(xOnlyKey))
	}

	refreshedKeyBlobs := make([][]byte, len(sessions))
	refreshedSIDOuts := make([][]byte, len(sessions))
	runPerParty(t, sessions, func(idx int) error {
		sidOut, refreshedKeyBlob, err := SchnorrMPRefreshAC(ctx, jobs[idx], sidOuts[idx], acKeyBlobs[idx], access, quorumPartyNames)
		if err != nil {
			return fmt.Errorf("refresh ac: %w", err)
		}
		refreshedSIDOuts[idx] = sidOut
		refreshedKeyBlobs[idx] = refreshedKeyBlob
		return nil
	})
	requireAllEqualNonEmptyBytes(t, "schnorr mp ac refresh sid", refreshedSIDOuts)

	refreshedXOnlyKeys := make([][]byte, len(refreshedKeyBlobs))
	for i, keyBlob := range refreshedKeyBlobs {
		refreshedXOnly, err := SchnorrMPExtractPublicKeyXOnly(keyBlob)
		if err != nil {
			t.Fatalf("extract refreshed x-only public key for party %d: %v", i, err)
		}
		refreshedXOnlyKeys[i] = refreshedXOnly
	}
	if !bytes.Equal(xOnlyKey, requireAllEqualNonEmptyBytes(t, "refreshed schnorr mp ac x-only public key", refreshedXOnlyKeys)) {
		t.Fatalf("schnorr mp ac public key changed across refresh")
	}

	sigs = make([][]byte, len(signSessions))
	runPerParty(t, signSessions, func(idx int) error {
		sig, err := SchnorrMPSignAC(ctx, signJobs[idx], refreshedKeyBlobs[idx], access, msgHash[:], 0)
		if err != nil {
			return fmt.Errorf("sign refreshed ac: %w", err)
		}
		sigs[idx] = sig
		return nil
	})
	sig = requireSignatureOnlyOnReceiver(t, "refreshed schnorr mp ac signature", sigs, 0)
	if len(sig) != 64 {
		t.Fatalf("unexpected refreshed Schnorr MP AC signature length: got %d want 64", len(sig))
	}
}

func TestTDH2ACFlow(t *testing.T) {
	ctx := testContext(t)
	sessions := mpctest.NewMockNetwork(3)
	partyNames := testPartyNames("ac/tdh2", len(sessions))
	access, quorumPartyIndices := thresholdAccessStructure(partyNames)
	quorumPartyNames := mustPartyNamesFromIndices(t, partyNames, quorumPartyIndices)
	jobs := jobsMP(sessions, partyNames)
	t.Cleanup(func() { closeMockSessions(t, sessions) })

	publicKeys := make([][]byte, len(sessions))
	publicShares := make([][][]byte, len(sessions))
	privateShares := make([][]byte, len(sessions))
	sidOuts := make([][]byte, len(sessions))
	runPerParty(t, sessions, func(idx int) error {
		publicKey, shares, privateShare, sidOut, err := TDH2DKGAC(ctx, jobs[idx], CurveP256, nil, access, quorumPartyNames)
		if err != nil {
			return fmt.Errorf("dkg ac: %w", err)
		}
		publicKeys[idx] = publicKey
		publicShares[idx] = shares
		privateShares[idx] = privateShare
		sidOuts[idx] = sidOut
		return nil
	})
	publicKey := requireAllEqualNonEmptyBytes(t, "tdh2 ac public key", publicKeys)
	requireAllEqualNonEmptyBytes(t, "tdh2 ac sid", sidOuts)
	publicShareSet := requireAllEqualNonEmptyByteMatrices(t, "tdh2 ac public shares", publicShares)

	plaintext := []byte("tdh2-ac-test-payload")
	label := []byte("tdh2-ac-test-label")
	ciphertext, err := TDH2Encrypt(publicKey, plaintext, label)
	if err != nil {
		t.Fatalf("encrypt: %v", err)
	}
	if err := TDH2Verify(publicKey, ciphertext, label); err != nil {
		t.Fatalf("verify: %v", err)
	}

	partials := make([][]byte, len(quorumPartyIndices))
	for i, partyIdx := range quorumPartyIndices {
		partial, err := TDH2PartialDecrypt(privateShares[partyIdx], ciphertext, label)
		if err != nil {
			t.Fatalf("partial decrypt for party %d: %v", partyIdx, err)
		}
		partials[i] = partial
	}

	recovered, err := TDH2CombineAC(access, publicKey, partyNames, publicShareSet, label, quorumPartyNames, partials, ciphertext)
	if err != nil {
		t.Fatalf("combine ac: %v", err)
	}
	if !bytes.Equal(recovered, plaintext) {
		t.Fatalf("recovered mismatch: got=%q want=%q", recovered, plaintext)
	}
}

func thresholdAccessStructure(partyNames []string) (*AccessStructure, []int) {
	if len(partyNames) != 3 {
		panic("thresholdAccessStructure requires exactly 3 party names")
	}
	return &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeLeaf, LeafName: partyNames[0]},
			{Type: AccessNodeLeaf, LeafName: partyNames[1]},
			{Type: AccessNodeLeaf, LeafName: partyNames[2]},
			{Type: AccessNodeThreshold, ThresholdK: 2, Children: []int{0, 1, 2}},
		},
		Root: 3,
	}, []int{0, 1}
}

func closeMockSessions(t *testing.T, sessions []*mpctest.MockTransport) {
	t.Helper()
	for _, session := range sessions {
		_ = session.Close()
	}
}

func requireSignatureOnlyOnReceiver(t *testing.T, label string, sigs [][]byte, receiver int) []byte {
	t.Helper()
	if receiver < 0 || receiver >= len(sigs) {
		t.Fatalf("receiver %d out of range [0, %d)", receiver, len(sigs))
	}
	if len(sigs[receiver]) == 0 {
		t.Fatalf("expected non-empty %s on receiver %d", label, receiver)
	}
	for i, sig := range sigs {
		if i == receiver {
			continue
		}
		if len(sig) != 0 {
			t.Fatalf("expected empty %s on non-receiver %d", label, i)
		}
	}
	return sigs[receiver]
}

func requireExtractECDSAMPShareMaterial(t *testing.T, keyBlob []byte, partyIndex int) ([]byte, []byte) {
	t.Helper()
	publicShare, err := ECDSAMPGetPublicShareCompressed(keyBlob)
	if err != nil {
		t.Fatalf("get public share for party %d: %v", partyIndex, err)
	}
	if len(publicShare) == 0 {
		t.Fatalf("public share for party %d is empty", partyIndex)
	}
	_, privateScalar, err := ECDSAMPDetachPrivateScalar(keyBlob)
	if err != nil {
		t.Fatalf("detach private scalar for party %d: %v", partyIndex, err)
	}
	if len(privateScalar) == 0 {
		t.Fatalf("private scalar for party %d is empty", partyIndex)
	}
	return publicShare, privateScalar
}

func requireAllEqualNonEmptyByteMatrices(t *testing.T, label string, values [][][]byte) [][]byte {
	t.Helper()
	if len(values) == 0 {
		t.Fatalf("no %s values provided", label)
	}
	ref := values[0]
	if len(ref) == 0 {
		t.Fatalf("%s[0] is empty", label)
	}
	for i, refValue := range ref {
		if len(refValue) == 0 {
			t.Fatalf("%s[0][%d] is empty", label, i)
		}
	}
	for i := 1; i < len(values); i++ {
		if len(values[i]) != len(ref) {
			t.Fatalf("%s[%d] length mismatch: got %d want %d", label, i, len(values[i]), len(ref))
		}
		for j := range ref {
			if len(values[i][j]) == 0 {
				t.Fatalf("%s[%d][%d] is empty", label, i, j)
			}
			if !bytes.Equal(ref[j], values[i][j]) {
				t.Fatalf("%s[%d][%d] does not match %s[0][%d]", label, i, j, label, j)
			}
		}
	}
	return ref
}

func requireEqualRows(t *testing.T, label string, got [][]byte, want [][]byte) {
	t.Helper()
	if len(got) != len(want) {
		t.Fatalf("%s row count mismatch: got %d want %d", label, len(got), len(want))
	}
	for i := range want {
		if !bytes.Equal(got[i], want[i]) {
			t.Fatalf("%s row %d mismatch: got=%x want=%x", label, i, got[i], want[i])
		}
	}
}

func requireValidSecp256k1ECDSASignature(t *testing.T, publicKeyCompressed []byte, msgHash []byte, sigDER []byte) {
	t.Helper()
	publicKey, err := secp256k1.ParsePubKey(publicKeyCompressed)
	if err != nil {
		t.Fatalf("failed to parse secp256k1 public key: %v", err)
	}
	signature, err := secp256k1ecdsa.ParseDERSignature(sigDER)
	if err != nil {
		t.Fatalf("failed to parse ECDSA DER signature: %v", err)
	}
	if !signature.Verify(msgHash, publicKey) {
		t.Fatal("ECDSA signature verification failed")
	}
}
