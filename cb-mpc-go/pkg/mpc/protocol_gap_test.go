package mpc

import (
	"bytes"
	"crypto/ed25519"
	"crypto/sha256"
	"fmt"
	"testing"

	"github.com/coinbase/cb-mpc-go/pkg/mpctest"
)

func TestProtocolGapCoverage(t *testing.T) {
	t.Run("eddsa 2p refresh", func(t *testing.T) {
		ctx := testContext(t)
		sessions := mpctest.NewMockNetwork(2)
		jobs := test2PJobs("protocol-gap/eddsa-2p-refresh", sessions)
		t.Cleanup(func() { closeMockSessions(t, sessions) })

		keyBlobs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			keyBlob, err := EdDSA2PDKG(ctx, jobs[idx], CurveEd25519)
			if err != nil {
				return fmt.Errorf("dkg: %w", err)
			}
			keyBlobs[idx] = keyBlob
			return nil
		})

		originalPubKeys := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			pubKey, err := EdDSA2PGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public key for party %d: %v", i, err)
			}
			originalPubKeys[i] = pubKey
		}
		originalPubKey := requireAllEqualNonEmptyBytes(t, "eddsa 2p public key", originalPubKeys)

		refreshed := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			keyBlob, err := EdDSA2PRefresh(ctx, jobs[idx], keyBlobs[idx])
			if err != nil {
				return fmt.Errorf("refresh: %w", err)
			}
			refreshed[idx] = keyBlob
			return nil
		})

		refreshedPubKeys := make([][]byte, len(refreshed))
		for i, keyBlob := range refreshed {
			pubKey, err := EdDSA2PGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get refreshed public key for party %d: %v", i, err)
			}
			refreshedPubKeys[i] = pubKey
		}
		refreshedPubKey := requireAllEqualNonEmptyBytes(t, "refreshed eddsa 2p public key", refreshedPubKeys)
		if !bytes.Equal(refreshedPubKey, originalPubKey) {
			t.Fatalf("EdDSA 2P public key changed across refresh")
		}

		msg := []byte("eddsa-2p-refresh")
		sigs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			sig, err := EdDSA2PSign(ctx, jobs[idx], refreshed[idx], msg)
			if err != nil {
				return fmt.Errorf("sign: %w", err)
			}
			sigs[idx] = sig
			return nil
		})
		sig := requireAtLeastOneNonEmptyEqualBytes(t, "refreshed eddsa 2p signature", sigs)
		if !ed25519.Verify(ed25519.PublicKey(refreshedPubKey), msg, sig) {
			t.Fatalf("refreshed EdDSA 2P signature failed verification")
		}
	})

	t.Run("eddsa mp additive refresh", func(t *testing.T) {
		ctx := testContext(t)
		sessions := mpctest.NewMockNetwork(3)
		partyNames := testPartyNames("protocol-gap/eddsa-mp-refresh", len(sessions))
		jobs := jobsMP(sessions, partyNames)
		t.Cleanup(func() { closeMockSessions(t, sessions) })

		keyBlobs := make([][]byte, len(sessions))
		sidOuts := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			keyBlob, sidOut, err := EdDSAMPDKGAdditive(ctx, jobs[idx], CurveEd25519)
			if err != nil {
				return fmt.Errorf("dkg additive: %w", err)
			}
			keyBlobs[idx] = keyBlob
			sidOuts[idx] = sidOut
			return nil
		})
		requireAllEqualNonEmptyBytes(t, "eddsa mp sid", sidOuts)

		originalPubKeys := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			pubKey, err := EdDSAMPGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public key for party %d: %v", i, err)
			}
			originalPubKeys[i] = pubKey
		}
		originalPubKey := requireAllEqualNonEmptyBytes(t, "eddsa mp public key", originalPubKeys)

		refreshed := make([][]byte, len(sessions))
		refreshSIDOuts := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			sidOut, keyBlob, err := EdDSAMPRefreshAdditive(ctx, jobs[idx], sidOuts[idx], keyBlobs[idx])
			if err != nil {
				return fmt.Errorf("refresh additive: %w", err)
			}
			refreshSIDOuts[idx] = sidOut
			refreshed[idx] = keyBlob
			return nil
		})
		requireAllEqualNonEmptyBytes(t, "eddsa mp refresh sid", refreshSIDOuts)

		refreshedPubKeys := make([][]byte, len(refreshed))
		for i, keyBlob := range refreshed {
			pubKey, err := EdDSAMPGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get refreshed public key for party %d: %v", i, err)
			}
			refreshedPubKeys[i] = pubKey
		}
		refreshedPubKey := requireAllEqualNonEmptyBytes(t, "refreshed eddsa mp public key", refreshedPubKeys)
		if !bytes.Equal(refreshedPubKey, originalPubKey) {
			t.Fatalf("EdDSA MP public key changed across refresh")
		}

		msg := []byte("eddsa-mp-refresh")
		sigs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			sig, err := EdDSAMPSignAdditive(ctx, jobs[idx], refreshed[idx], msg, 0)
			if err != nil {
				return fmt.Errorf("sign additive: %w", err)
			}
			sigs[idx] = sig
			return nil
		})
		sig := requireSignatureOnlyOnReceiver(t, "refreshed eddsa mp signature", sigs, 0)
		if !ed25519.Verify(ed25519.PublicKey(refreshedPubKey), msg, sig) {
			t.Fatalf("refreshed EdDSA MP signature failed verification")
		}
	})

	t.Run("schnorr 2p refresh and compressed key", func(t *testing.T) {
		ctx := testContext(t)
		sessions := mpctest.NewMockNetwork(2)
		jobs := test2PJobs("protocol-gap/schnorr-2p-refresh", sessions)
		t.Cleanup(func() { closeMockSessions(t, sessions) })

		keyBlobs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			keyBlob, err := Schnorr2PDKG(ctx, jobs[idx], CurveSecp256k1)
			if err != nil {
				return fmt.Errorf("dkg: %w", err)
			}
			keyBlobs[idx] = keyBlob
			return nil
		})

		originalPubKeys := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			pubKey, err := Schnorr2PGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public key for party %d: %v", i, err)
			}
			originalPubKeys[i] = pubKey
		}
		originalPubKey := requireAllEqualNonEmptyBytes(t, "schnorr 2p compressed public key", originalPubKeys)
		if len(originalPubKey) != 33 {
			t.Fatalf("unexpected Schnorr 2P compressed public key size: got %d want 33", len(originalPubKey))
		}

		refreshed := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			keyBlob, err := Schnorr2PRefresh(ctx, jobs[idx], keyBlobs[idx])
			if err != nil {
				return fmt.Errorf("refresh: %w", err)
			}
			refreshed[idx] = keyBlob
			return nil
		})

		refreshedPubKeys := make([][]byte, len(refreshed))
		for i, keyBlob := range refreshed {
			pubKey, err := Schnorr2PGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get refreshed public key for party %d: %v", i, err)
			}
			refreshedPubKeys[i] = pubKey
		}
		refreshedPubKey := requireAllEqualNonEmptyBytes(t, "refreshed schnorr 2p compressed public key", refreshedPubKeys)
		if !bytes.Equal(refreshedPubKey, originalPubKey) {
			t.Fatalf("Schnorr 2P public key changed across refresh")
		}

		msgHash := sha256.Sum256([]byte("schnorr-2p-refresh"))
		sigs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			sig, err := Schnorr2PSign(ctx, jobs[idx], refreshed[idx], msgHash[:])
			if err != nil {
				return fmt.Errorf("sign: %w", err)
			}
			sigs[idx] = sig
			return nil
		})
		if sig := requireAtLeastOneNonEmptyEqualBytes(t, "refreshed schnorr 2p signature", sigs); len(sig) != 64 {
			t.Fatalf("unexpected Schnorr 2P signature size: got %d want 64", len(sig))
		}
	})

	t.Run("schnorr mp additive refresh and compressed key", func(t *testing.T) {
		ctx := testContext(t)
		sessions := mpctest.NewMockNetwork(3)
		partyNames := testPartyNames("protocol-gap/schnorr-mp-refresh", len(sessions))
		jobs := jobsMP(sessions, partyNames)
		t.Cleanup(func() { closeMockSessions(t, sessions) })

		keyBlobs := make([][]byte, len(sessions))
		sidOuts := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			keyBlob, sidOut, err := SchnorrMPDKGAdditive(ctx, jobs[idx], CurveSecp256k1)
			if err != nil {
				return fmt.Errorf("dkg additive: %w", err)
			}
			keyBlobs[idx] = keyBlob
			sidOuts[idx] = sidOut
			return nil
		})
		requireAllEqualNonEmptyBytes(t, "schnorr mp sid", sidOuts)

		originalPubKeys := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			pubKey, err := SchnorrMPGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public key for party %d: %v", i, err)
			}
			originalPubKeys[i] = pubKey
		}
		originalPubKey := requireAllEqualNonEmptyBytes(t, "schnorr mp compressed public key", originalPubKeys)
		if len(originalPubKey) != 33 {
			t.Fatalf("unexpected Schnorr MP compressed public key size: got %d want 33", len(originalPubKey))
		}

		refreshed := make([][]byte, len(sessions))
		refreshSIDOuts := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			sidOut, keyBlob, err := SchnorrMPRefreshAdditive(ctx, jobs[idx], sidOuts[idx], keyBlobs[idx])
			if err != nil {
				return fmt.Errorf("refresh additive: %w", err)
			}
			refreshSIDOuts[idx] = sidOut
			refreshed[idx] = keyBlob
			return nil
		})
		requireAllEqualNonEmptyBytes(t, "schnorr mp refresh sid", refreshSIDOuts)

		refreshedPubKeys := make([][]byte, len(refreshed))
		for i, keyBlob := range refreshed {
			pubKey, err := SchnorrMPGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get refreshed public key for party %d: %v", i, err)
			}
			refreshedPubKeys[i] = pubKey
		}
		refreshedPubKey := requireAllEqualNonEmptyBytes(t, "refreshed schnorr mp compressed public key", refreshedPubKeys)
		if !bytes.Equal(refreshedPubKey, originalPubKey) {
			t.Fatalf("Schnorr MP public key changed across refresh")
		}

		msgHash := sha256.Sum256([]byte("schnorr-mp-refresh"))
		sigs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			sig, err := SchnorrMPSignAdditive(ctx, jobs[idx], refreshed[idx], msgHash[:], 0)
			if err != nil {
				return fmt.Errorf("sign additive: %w", err)
			}
			sigs[idx] = sig
			return nil
		})
		if sig := requireSignatureOnlyOnReceiver(t, "refreshed schnorr mp signature", sigs, 0); len(sig) != 64 {
			t.Fatalf("unexpected Schnorr MP signature size: got %d want 64", len(sig))
		}
	})
}
