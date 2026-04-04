package main

import (
	"bytes"
	"context"
	"crypto/ed25519"
	"fmt"
	"log"
	"sync"
	"time"

	"github.com/coinbase/cb-mpc-go/pkg/mpc"
	"github.com/coinbase/cb-mpc-go/pkg/mpctest"
)

const (
	signerCount   = 3
	keyCount      = 3
	signReceiver  = 0
	backupRow     = 0
	backupTimeout = 60 * time.Second
)

var signerNames = []string{
	"urn:uuid:2f4b567e-7d58-52c5-9c74-0d0c8b6a3e10",
	"urn:uuid:7a1f5e4a-b8b4-5c67-a4cb-5777f284d64e",
	"urn:uuid:3f5d3b64-51d7-58d6-9d8d-3a7e6b1f4032",
}

type backedUpKey struct {
	partyKeyBlobs [][]byte
	publicKey     []byte
	publicKeyBlob []byte
	publicShare   []byte
}

func main() {
	ctx, cancel := context.WithTimeout(context.Background(), backupTimeout)
	defer cancel()

	// mpctest.NewMockNetwork is for local demos/tests only.
	access, backupLeafNames, backupLeafQuorum := backupAccessStructure()
	backupEKs := make([][]byte, len(backupLeafNames))
	backupDKs := make([][]byte, len(backupLeafNames))
	for i := range backupLeafNames {
		ek, dk, err := mpc.PVEGenerateBasePKERSAKeypair()
		if err != nil {
			log.Fatalf("generate backup key %d: %v", i, err)
		}
		backupEKs[i] = ek
		backupDKs[i] = dk
	}

	keys := make([]backedUpKey, keyCount)
	detachedScalars := make([][]byte, keyCount)
	for keyIdx := 0; keyIdx < keyCount; keyIdx++ {
		key, scalar, err := generateBackupableKey(ctx, keyIdx)
		if err != nil {
			log.Fatalf("generate key %d: %v", keyIdx, err)
		}
		keys[keyIdx] = key
		detachedScalars[keyIdx] = scalar
	}
	sensitiveDetachedScalars := mpc.SensitiveSlices(detachedScalars)
	defer func() { _ = sensitiveDetachedScalars.Close() }()

	label := []byte("cb-mpc-go:eddsa-mp-backup:restored-signer-2f4b567e")
	ciphertext, err := mpc.PVEACEncrypt(nil, mpc.CurveEd25519, access, backupLeafNames, backupEKs, label, sensitiveDetachedScalars.Bytes())
	if err != nil {
		log.Fatalf("encrypt detached scalars: %v", err)
	}
	count, err := mpc.PVEACGetCount(ciphertext)
	if err != nil {
		log.Fatalf("count backup ciphertext rows: %v", err)
	}
	qs, err := mpc.PVEACGetQs(ciphertext)
	if err != nil {
		log.Fatalf("extract backup ciphertext Qs: %v", err)
	}
	if err := mpc.PVEACVerify(nil, mpc.CurveEd25519, access, backupLeafNames, backupEKs, ciphertext, qs, label); err != nil {
		log.Fatalf("verify backup ciphertext: %v", err)
	}

	quorumLeafNames := make([]string, len(backupLeafQuorum))
	quorumShares := make([][]byte, len(backupLeafQuorum))
	for i, leafIdx := range backupLeafQuorum {
		quorumLeafNames[i] = backupLeafNames[leafIdx]
		share, err := mpc.PVEACPartyDecryptRow(nil, mpc.CurveEd25519, access, ciphertext, backupRow, backupLeafNames[leafIdx], backupDKs[leafIdx], label)
		if err != nil {
			log.Fatalf("decrypt backup share for %s: %v", backupLeafNames[leafIdx], err)
		}
		quorumShares[i] = share
	}

	recoveredScalars, err := mpc.PVEACAggregateToRestoreRow(nil, mpc.CurveEd25519, access, quorumLeafNames, quorumShares, ciphertext, backupRow, label)
	if err != nil {
		log.Fatalf("aggregate backup shares: %v", err)
	}
	sensitiveRecoveredScalars := mpc.SensitiveSlices(recoveredScalars)
	defer func() { _ = sensitiveRecoveredScalars.Close() }()

	restoredParty0KeyBlobs := make([][]byte, keyCount)
	for keyIdx, key := range keys {
		restoredKeyBlob, err := mpc.EdDSAMPAttachPrivateScalar(key.publicKeyBlob, sensitiveRecoveredScalars.Bytes()[keyIdx], key.publicShare)
		if err != nil {
			log.Fatalf("reattach key %d: %v", keyIdx, err)
		}
		restoredParty0KeyBlobs[keyIdx] = restoredKeyBlob
	}

	for keyIdx, key := range keys {
		msg := []byte(fmt.Sprintf("cb-mpc-go eddsa mp backup demo key %d", keyIdx))
		sig, err := signWithRestoredKey(ctx, msg, key.partyKeyBlobs, restoredParty0KeyBlobs[keyIdx])
		if err != nil {
			log.Fatalf("sign with restored key %d: %v", keyIdx, err)
		}
		if !ed25519.Verify(ed25519.PublicKey(key.publicKey), msg, sig) {
			log.Fatalf("signature verification failed for key %d", keyIdx)
		}
		fmt.Printf("key %d restored signer share verified: public key bytes=%d signature bytes=%d\n", keyIdx, len(key.publicKey), len(sig))
	}

	fmt.Printf("Backed up %d detached Ed25519 signer shares into one %d-row PVE-AC ciphertext (%d bytes)\n", keyCount, count, len(ciphertext))
	fmt.Printf("Recovered with quorum %v and reattached the restored scalar shares back into usable signer key blobs\n", quorumLeafNames)
}

func backupAccessStructure() (*mpc.AccessStructure, []string, []int) {
	leafNames := []string{
		"urn:uuid:54bc82b4-4e56-55ae-b858-d6206a654613",
		"urn:uuid:0f4c3f34-28e2-5259-9800-0d3f3be8f9ef",
		"urn:uuid:813f1a5f-53c9-549e-8d16-c2c5a193f357",
	}
	return &mpc.AccessStructure{
		Nodes: []mpc.AccessStructureNode{
			{Type: mpc.AccessNodeLeaf, LeafName: leafNames[0]},
			{Type: mpc.AccessNodeLeaf, LeafName: leafNames[1]},
			{Type: mpc.AccessNodeLeaf, LeafName: leafNames[2]},
			{Type: mpc.AccessNodeThreshold, ThresholdK: 2, Children: []int{0, 1, 2}},
		},
		Root: 3,
	}, leafNames, []int{0, 2}
}

func generateBackupableKey(ctx context.Context, keyIdx int) (backedUpKey, []byte, error) {
	// This helper runs all logical parties in one process for zero-setup local
	// demonstration. Use demos/ for process-isolated, production-like flows.
	sessions := mpctest.NewMockNetwork(signerCount)
	jobs := make([]mpc.JobMP, signerCount)
	for i := range sessions {
		jobs[i] = mpc.JobMP{
			Self:       i,
			PartyNames: append([]string(nil), signerNames...),
			Transport:  sessions[i],
		}
	}
	defer closeSessions(sessions)

	keyBlobs := make([][]byte, signerCount)
	sidOuts := make([][]byte, signerCount)
	if err := runPerParty(sessions, func(idx int) error {
		keyBlob, sidOut, err := mpc.EdDSAMPDKGAdditive(ctx, jobs[idx], mpc.CurveEd25519)
		if err != nil {
			return fmt.Errorf("party %d dkg additive: %w", idx, err)
		}
		keyBlobs[idx] = keyBlob
		sidOuts[idx] = sidOut
		return nil
	}); err != nil {
		return backedUpKey{}, nil, err
	}
	if _, err := allEqualNonEmptyBytes(sidOuts); err != nil {
		return backedUpKey{}, nil, fmt.Errorf("key %d sid mismatch: %w", keyIdx, err)
	}

	publicKeys := make([][]byte, signerCount)
	for i, keyBlob := range keyBlobs {
		publicKey, err := mpc.EdDSAMPGetPublicKeyCompressed(keyBlob)
		if err != nil {
			return backedUpKey{}, nil, fmt.Errorf("key %d get public key for party %d: %w", keyIdx, i, err)
		}
		publicKeys[i] = publicKey
	}
	publicKey, err := allEqualNonEmptyBytes(publicKeys)
	if err != nil {
		return backedUpKey{}, nil, fmt.Errorf("key %d public key mismatch: %w", keyIdx, err)
	}

	publicShare, err := mpc.EdDSAMPGetPublicShareCompressed(keyBlobs[0])
	if err != nil {
		return backedUpKey{}, nil, fmt.Errorf("key %d get public share: %w", keyIdx, err)
	}
	publicKeyBlob, privateScalar, err := mpc.EdDSAMPDetachPrivateScalar(keyBlobs[0])
	if err != nil {
		return backedUpKey{}, nil, fmt.Errorf("key %d detach private scalar: %w", keyIdx, err)
	}

	fmt.Printf("key %d generated: public key bytes=%d detached scalar bytes=%d\n", keyIdx, len(publicKey), len(privateScalar))
	return backedUpKey{
		partyKeyBlobs: keyBlobs,
		publicKey:     publicKey,
		publicKeyBlob: publicKeyBlob,
		publicShare:   publicShare,
	}, privateScalar, nil
}

func signWithRestoredKey(ctx context.Context, msg []byte, originalPartyKeyBlobs [][]byte, restoredParty0KeyBlob []byte) ([]byte, error) {
	// This helper runs all logical parties in one process for zero-setup local
	// demonstration. Use demos/ for process-isolated, production-like flows.
	sessions := mpctest.NewMockNetwork(signerCount)
	jobs := make([]mpc.JobMP, signerCount)
	for i := range sessions {
		jobs[i] = mpc.JobMP{
			Self:       i,
			PartyNames: append([]string(nil), signerNames...),
			Transport:  sessions[i],
		}
	}
	defer closeSessions(sessions)

	signatures := make([][]byte, signerCount)
	if err := runPerParty(sessions, func(idx int) error {
		keyBlob := originalPartyKeyBlobs[idx]
		if idx == 0 {
			keyBlob = restoredParty0KeyBlob
		}
		sig, err := mpc.EdDSAMPSignAdditive(ctx, jobs[idx], keyBlob, msg, signReceiver)
		if err != nil {
			return fmt.Errorf("party %d sign additive: %w", idx, err)
		}
		signatures[idx] = sig
		return nil
	}); err != nil {
		return nil, err
	}
	return requireSignatureOnlyOnReceiver(signatures, signReceiver)
}

func runPerParty(sessions []*mpctest.MockTransport, fn func(idx int) error) error {
	errCh := make(chan error, len(sessions))
	var wg sync.WaitGroup
	for i := range sessions {
		wg.Add(1)
		go func(idx int) {
			defer wg.Done()
			if err := fn(idx); err != nil {
				errCh <- err
			}
		}(i)
	}
	wg.Wait()
	close(errCh)
	for err := range errCh {
		return err
	}
	return nil
}

func closeSessions(sessions []*mpctest.MockTransport) {
	for _, session := range sessions {
		_ = session.Close()
	}
}

func allEqualNonEmptyBytes(values [][]byte) ([]byte, error) {
	if len(values) == 0 {
		return nil, fmt.Errorf("no values provided")
	}
	ref := values[0]
	if len(ref) == 0 {
		return nil, fmt.Errorf("first value is empty")
	}
	for i := 1; i < len(values); i++ {
		if len(values[i]) == 0 {
			return nil, fmt.Errorf("value %d is empty", i)
		}
		if !bytes.Equal(ref, values[i]) {
			return nil, fmt.Errorf("value %d does not match value 0", i)
		}
	}
	return ref, nil
}

func requireSignatureOnlyOnReceiver(signatures [][]byte, receiver int) ([]byte, error) {
	if receiver < 0 || receiver >= len(signatures) {
		return nil, fmt.Errorf("receiver %d out of range [0, %d)", receiver, len(signatures))
	}
	if len(signatures[receiver]) == 0 {
		return nil, fmt.Errorf("receiver %d did not receive a signature", receiver)
	}
	for i, sig := range signatures {
		if i == receiver {
			continue
		}
		if len(sig) != 0 {
			return nil, fmt.Errorf("non-receiver %d unexpectedly received a signature", i)
		}
	}
	return signatures[receiver], nil
}
