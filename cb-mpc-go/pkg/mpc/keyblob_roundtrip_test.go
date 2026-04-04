package mpc

import (
	"bytes"
	"crypto/ed25519"
	"crypto/sha256"
	"fmt"
	"testing"

	"github.com/coinbase/cb-mpc-go/pkg/mpctest"
)

func TestDetachAttachRoundTrips(t *testing.T) {
	t.Run("ecdsa-2p", func(t *testing.T) {
		ctx := testContext(t)
		sessions := mpctest.NewMockNetwork(2)
		jobs := test2PJobs("roundtrip/ecdsa-2p", sessions)
		t.Cleanup(func() { closeMockSessions(t, sessions) })

		keyBlobs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			keyBlob, err := ECDSA2PDKG(ctx, jobs[idx], CurveSecp256k1)
			if err != nil {
				return fmt.Errorf("dkg: %w", err)
			}
			keyBlobs[idx] = keyBlob
			return nil
		})

		originalPubKeys := make([][]byte, len(keyBlobs))
		originalShares := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			pubKey, err := ECDSA2PGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public key for party %d: %v", i, err)
			}
			share, err := ECDSA2PGetPublicShareCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public share for party %d: %v", i, err)
			}
			originalPubKeys[i] = pubKey
			originalShares[i] = share
		}
		originalPubKey := requireAllEqualNonEmptyBytes(t, "ecdsa 2p public key", originalPubKeys)

		reattachedKeyBlobs := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			publicKeyBlob, privateScalar, err := ECDSA2PDetachPrivateScalar(keyBlob)
			if err != nil {
				t.Fatalf("detach private scalar for party %d: %v", i, err)
			}
			reattachedKeyBlob, err := ECDSA2PAttachPrivateScalar(publicKeyBlob, privateScalar, originalShares[i])
			if err != nil {
				t.Fatalf("attach private scalar for party %d: %v", i, err)
			}
			reattachedKeyBlobs[i] = reattachedKeyBlob
		}

		reattachedPubKeys := make([][]byte, len(reattachedKeyBlobs))
		reattachedShares := make([][]byte, len(reattachedKeyBlobs))
		for i, keyBlob := range reattachedKeyBlobs {
			pubKey, err := ECDSA2PGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get reattached public key for party %d: %v", i, err)
			}
			share, err := ECDSA2PGetPublicShareCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get reattached public share for party %d: %v", i, err)
			}
			reattachedPubKeys[i] = pubKey
			reattachedShares[i] = share
		}
		if !bytes.Equal(originalPubKey, requireAllEqualNonEmptyBytes(t, "reattached ecdsa 2p public key", reattachedPubKeys)) {
			t.Fatalf("reattached ecdsa 2p public key mismatch")
		}
		requirePerPartyEqualBytes(t, "reattached ecdsa 2p public shares", reattachedShares, originalShares)

		hash := sha256.Sum256([]byte("ecdsa-2p-detach-attach"))
		sigs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			_, sigDER, err := ECDSA2PSign(ctx, jobs[idx], reattachedKeyBlobs[idx], hash[:], nil)
			if err != nil {
				return fmt.Errorf("sign: %w", err)
			}
			sigs[idx] = sigDER
			return nil
		})
		requireValidECDSADER(t, requireAtLeastOneNonEmptyEqualBytes(t, "reattached ecdsa 2p signature", sigs))
	})

	t.Run("ecdsa-mp", func(t *testing.T) {
		ctx := testContext(t)
		sessions := mpctest.NewMockNetwork(3)
		partyNames := testPartyNames("roundtrip/ecdsa-mp", len(sessions))
		jobs := jobsMP(sessions, partyNames)
		t.Cleanup(func() { closeMockSessions(t, sessions) })

		keyBlobs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			keyBlob, _, err := ECDSAMPDKGAdditive(ctx, jobs[idx], CurveSecp256k1)
			if err != nil {
				return fmt.Errorf("dkg additive: %w", err)
			}
			keyBlobs[idx] = keyBlob
			return nil
		})

		originalPubKeys := make([][]byte, len(keyBlobs))
		originalShares := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			pubKey, err := ECDSAMPGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public key for party %d: %v", i, err)
			}
			share, err := ECDSAMPGetPublicShareCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public share for party %d: %v", i, err)
			}
			originalPubKeys[i] = pubKey
			originalShares[i] = share
		}
		originalPubKey := requireAllEqualNonEmptyBytes(t, "ecdsa mp public key", originalPubKeys)

		reattachedKeyBlobs := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			publicKeyBlob, privateScalar, err := ECDSAMPDetachPrivateScalar(keyBlob)
			if err != nil {
				t.Fatalf("detach private scalar for party %d: %v", i, err)
			}
			reattachedKeyBlob, err := ECDSAMPAttachPrivateScalar(publicKeyBlob, privateScalar, originalShares[i])
			if err != nil {
				t.Fatalf("attach private scalar for party %d: %v", i, err)
			}
			reattachedKeyBlobs[i] = reattachedKeyBlob
		}

		reattachedPubKeys := make([][]byte, len(reattachedKeyBlobs))
		reattachedShares := make([][]byte, len(reattachedKeyBlobs))
		for i, keyBlob := range reattachedKeyBlobs {
			pubKey, err := ECDSAMPGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get reattached public key for party %d: %v", i, err)
			}
			share, err := ECDSAMPGetPublicShareCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get reattached public share for party %d: %v", i, err)
			}
			reattachedPubKeys[i] = pubKey
			reattachedShares[i] = share
		}
		if !bytes.Equal(originalPubKey, requireAllEqualNonEmptyBytes(t, "reattached ecdsa mp public key", reattachedPubKeys)) {
			t.Fatalf("reattached ecdsa mp public key mismatch")
		}
		requirePerPartyEqualBytes(t, "reattached ecdsa mp public shares", reattachedShares, originalShares)

		hash := sha256.Sum256([]byte("ecdsa-mp-detach-attach"))
		sigs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			sigDER, err := ECDSAMPSignAdditive(ctx, jobs[idx], reattachedKeyBlobs[idx], hash[:], 0)
			if err != nil {
				return fmt.Errorf("sign additive: %w", err)
			}
			sigs[idx] = sigDER
			return nil
		})
		requireValidECDSADER(t, requireSignatureOnlyOnReceiver(t, "reattached ecdsa mp signature", sigs, 0))
	})

	t.Run("eddsa-2p", func(t *testing.T) {
		ctx := testContext(t)
		sessions := mpctest.NewMockNetwork(2)
		jobs := test2PJobs("roundtrip/eddsa-2p", sessions)
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
		originalShares := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			pubKey, err := EdDSA2PGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public key for party %d: %v", i, err)
			}
			share, err := EdDSA2PGetPublicShareCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public share for party %d: %v", i, err)
			}
			originalPubKeys[i] = pubKey
			originalShares[i] = share
		}
		originalPubKey := requireAllEqualNonEmptyBytes(t, "eddsa 2p public key", originalPubKeys)

		reattachedKeyBlobs := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			publicKeyBlob, privateScalar, err := EdDSA2PDetachPrivateScalar(keyBlob)
			if err != nil {
				t.Fatalf("detach private scalar for party %d: %v", i, err)
			}
			reattachedKeyBlob, err := EdDSA2PAttachPrivateScalar(publicKeyBlob, privateScalar, originalShares[i])
			if err != nil {
				t.Fatalf("attach private scalar for party %d: %v", i, err)
			}
			reattachedKeyBlobs[i] = reattachedKeyBlob
		}

		reattachedPubKeys := make([][]byte, len(reattachedKeyBlobs))
		reattachedShares := make([][]byte, len(reattachedKeyBlobs))
		for i, keyBlob := range reattachedKeyBlobs {
			pubKey, err := EdDSA2PGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get reattached public key for party %d: %v", i, err)
			}
			share, err := EdDSA2PGetPublicShareCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get reattached public share for party %d: %v", i, err)
			}
			reattachedPubKeys[i] = pubKey
			reattachedShares[i] = share
		}
		if !bytes.Equal(originalPubKey, requireAllEqualNonEmptyBytes(t, "reattached eddsa 2p public key", reattachedPubKeys)) {
			t.Fatalf("reattached eddsa 2p public key mismatch")
		}
		requirePerPartyEqualBytes(t, "reattached eddsa 2p public shares", reattachedShares, originalShares)

		msg := []byte("eddsa-2p-detach-attach")
		sigs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			sig, err := EdDSA2PSign(ctx, jobs[idx], reattachedKeyBlobs[idx], msg)
			if err != nil {
				return fmt.Errorf("sign: %w", err)
			}
			sigs[idx] = sig
			return nil
		})
		sig := requireAtLeastOneNonEmptyEqualBytes(t, "reattached eddsa 2p signature", sigs)
		if !ed25519.Verify(ed25519.PublicKey(originalPubKey), msg, sig) {
			t.Fatalf("reattached EdDSA 2P signature failed verification")
		}
	})

	t.Run("eddsa-mp", func(t *testing.T) {
		ctx := testContext(t)
		sessions := mpctest.NewMockNetwork(3)
		partyNames := testPartyNames("roundtrip/eddsa-mp", len(sessions))
		jobs := jobsMP(sessions, partyNames)
		t.Cleanup(func() { closeMockSessions(t, sessions) })

		keyBlobs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			keyBlob, _, err := EdDSAMPDKGAdditive(ctx, jobs[idx], CurveEd25519)
			if err != nil {
				return fmt.Errorf("dkg additive: %w", err)
			}
			keyBlobs[idx] = keyBlob
			return nil
		})

		originalPubKeys := make([][]byte, len(keyBlobs))
		originalShares := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			pubKey, err := EdDSAMPGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public key for party %d: %v", i, err)
			}
			share, err := EdDSAMPGetPublicShareCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public share for party %d: %v", i, err)
			}
			originalPubKeys[i] = pubKey
			originalShares[i] = share
		}
		originalPubKey := requireAllEqualNonEmptyBytes(t, "eddsa mp public key", originalPubKeys)

		reattachedKeyBlobs := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			publicKeyBlob, privateScalar, err := EdDSAMPDetachPrivateScalar(keyBlob)
			if err != nil {
				t.Fatalf("detach private scalar for party %d: %v", i, err)
			}
			reattachedKeyBlob, err := EdDSAMPAttachPrivateScalar(publicKeyBlob, privateScalar, originalShares[i])
			if err != nil {
				t.Fatalf("attach private scalar for party %d: %v", i, err)
			}
			reattachedKeyBlobs[i] = reattachedKeyBlob
		}

		reattachedPubKeys := make([][]byte, len(reattachedKeyBlobs))
		reattachedShares := make([][]byte, len(reattachedKeyBlobs))
		for i, keyBlob := range reattachedKeyBlobs {
			pubKey, err := EdDSAMPGetPublicKeyCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get reattached public key for party %d: %v", i, err)
			}
			share, err := EdDSAMPGetPublicShareCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get reattached public share for party %d: %v", i, err)
			}
			reattachedPubKeys[i] = pubKey
			reattachedShares[i] = share
		}
		if !bytes.Equal(originalPubKey, requireAllEqualNonEmptyBytes(t, "reattached eddsa mp public key", reattachedPubKeys)) {
			t.Fatalf("reattached eddsa mp public key mismatch")
		}
		requirePerPartyEqualBytes(t, "reattached eddsa mp public shares", reattachedShares, originalShares)

		msg := []byte("eddsa-mp-detach-attach")
		sigs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			sig, err := EdDSAMPSignAdditive(ctx, jobs[idx], reattachedKeyBlobs[idx], msg, 0)
			if err != nil {
				return fmt.Errorf("sign additive: %w", err)
			}
			sigs[idx] = sig
			return nil
		})
		sig := requireSignatureOnlyOnReceiver(t, "reattached eddsa mp signature", sigs, 0)
		if !ed25519.Verify(ed25519.PublicKey(originalPubKey), msg, sig) {
			t.Fatalf("reattached EdDSA MP signature failed verification")
		}
	})

	t.Run("schnorr-2p", func(t *testing.T) {
		ctx := testContext(t)
		sessions := mpctest.NewMockNetwork(2)
		jobs := test2PJobs("roundtrip/schnorr-2p", sessions)
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

		originalXOnlyKeys := make([][]byte, len(keyBlobs))
		originalShares := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			xOnly, err := Schnorr2PExtractPublicKeyXOnly(keyBlob)
			if err != nil {
				t.Fatalf("extract x-only public key for party %d: %v", i, err)
			}
			share, err := Schnorr2PGetPublicShareCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public share for party %d: %v", i, err)
			}
			originalXOnlyKeys[i] = xOnly
			originalShares[i] = share
		}
		originalXOnlyKey := requireAllEqualNonEmptyBytes(t, "schnorr 2p x-only public key", originalXOnlyKeys)

		reattachedKeyBlobs := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			publicKeyBlob, privateScalar, err := Schnorr2PDetachPrivateScalar(keyBlob)
			if err != nil {
				t.Fatalf("detach private scalar for party %d: %v", i, err)
			}
			reattachedKeyBlob, err := Schnorr2PAttachPrivateScalar(publicKeyBlob, privateScalar, originalShares[i])
			if err != nil {
				t.Fatalf("attach private scalar for party %d: %v", i, err)
			}
			reattachedKeyBlobs[i] = reattachedKeyBlob
		}

		reattachedXOnlyKeys := make([][]byte, len(reattachedKeyBlobs))
		reattachedShares := make([][]byte, len(reattachedKeyBlobs))
		for i, keyBlob := range reattachedKeyBlobs {
			xOnly, err := Schnorr2PExtractPublicKeyXOnly(keyBlob)
			if err != nil {
				t.Fatalf("extract reattached x-only public key for party %d: %v", i, err)
			}
			share, err := Schnorr2PGetPublicShareCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get reattached public share for party %d: %v", i, err)
			}
			reattachedXOnlyKeys[i] = xOnly
			reattachedShares[i] = share
		}
		if !bytes.Equal(originalXOnlyKey, requireAllEqualNonEmptyBytes(t, "reattached schnorr 2p x-only public key", reattachedXOnlyKeys)) {
			t.Fatalf("reattached schnorr 2p public key mismatch")
		}
		requirePerPartyEqualBytes(t, "reattached schnorr 2p public shares", reattachedShares, originalShares)

		msgHash := sha256.Sum256([]byte("schnorr-2p-detach-attach"))
		sigs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			sig, err := Schnorr2PSign(ctx, jobs[idx], reattachedKeyBlobs[idx], msgHash[:])
			if err != nil {
				return fmt.Errorf("sign: %w", err)
			}
			sigs[idx] = sig
			return nil
		})
		sig := requireAtLeastOneNonEmptyEqualBytes(t, "reattached schnorr 2p signature", sigs)
		if len(sig) != 64 {
			t.Fatalf("unexpected Schnorr 2P signature length: got %d want 64", len(sig))
		}
	})

	t.Run("schnorr-mp", func(t *testing.T) {
		ctx := testContext(t)
		sessions := mpctest.NewMockNetwork(3)
		partyNames := testPartyNames("roundtrip/schnorr-mp", len(sessions))
		jobs := jobsMP(sessions, partyNames)
		t.Cleanup(func() { closeMockSessions(t, sessions) })

		keyBlobs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			keyBlob, _, err := SchnorrMPDKGAdditive(ctx, jobs[idx], CurveSecp256k1)
			if err != nil {
				return fmt.Errorf("dkg additive: %w", err)
			}
			keyBlobs[idx] = keyBlob
			return nil
		})

		originalXOnlyKeys := make([][]byte, len(keyBlobs))
		originalShares := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			xOnly, err := SchnorrMPExtractPublicKeyXOnly(keyBlob)
			if err != nil {
				t.Fatalf("extract x-only public key for party %d: %v", i, err)
			}
			share, err := SchnorrMPGetPublicShareCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get public share for party %d: %v", i, err)
			}
			originalXOnlyKeys[i] = xOnly
			originalShares[i] = share
		}
		originalXOnlyKey := requireAllEqualNonEmptyBytes(t, "schnorr mp x-only public key", originalXOnlyKeys)

		reattachedKeyBlobs := make([][]byte, len(keyBlobs))
		for i, keyBlob := range keyBlobs {
			publicKeyBlob, privateScalar, err := SchnorrMPDetachPrivateScalar(keyBlob)
			if err != nil {
				t.Fatalf("detach private scalar for party %d: %v", i, err)
			}
			reattachedKeyBlob, err := SchnorrMPAttachPrivateScalar(publicKeyBlob, privateScalar, originalShares[i])
			if err != nil {
				t.Fatalf("attach private scalar for party %d: %v", i, err)
			}
			reattachedKeyBlobs[i] = reattachedKeyBlob
		}

		reattachedXOnlyKeys := make([][]byte, len(reattachedKeyBlobs))
		reattachedShares := make([][]byte, len(reattachedKeyBlobs))
		for i, keyBlob := range reattachedKeyBlobs {
			xOnly, err := SchnorrMPExtractPublicKeyXOnly(keyBlob)
			if err != nil {
				t.Fatalf("extract reattached x-only public key for party %d: %v", i, err)
			}
			share, err := SchnorrMPGetPublicShareCompressed(keyBlob)
			if err != nil {
				t.Fatalf("get reattached public share for party %d: %v", i, err)
			}
			reattachedXOnlyKeys[i] = xOnly
			reattachedShares[i] = share
		}
		if !bytes.Equal(originalXOnlyKey, requireAllEqualNonEmptyBytes(t, "reattached schnorr mp x-only public key", reattachedXOnlyKeys)) {
			t.Fatalf("reattached schnorr mp public key mismatch")
		}
		requirePerPartyEqualBytes(t, "reattached schnorr mp public shares", reattachedShares, originalShares)

		msgHash := sha256.Sum256([]byte("schnorr-mp-detach-attach"))
		sigs := make([][]byte, len(sessions))
		runPerParty(t, sessions, func(idx int) error {
			sig, err := SchnorrMPSignAdditive(ctx, jobs[idx], reattachedKeyBlobs[idx], msgHash[:], 0)
			if err != nil {
				return fmt.Errorf("sign additive: %w", err)
			}
			sigs[idx] = sig
			return nil
		})
		sig := requireSignatureOnlyOnReceiver(t, "reattached schnorr mp signature", sigs, 0)
		if len(sig) != 64 {
			t.Fatalf("unexpected Schnorr MP signature length: got %d want 64", len(sig))
		}
	})
}

func requirePerPartyEqualBytes(t *testing.T, label string, got [][]byte, want [][]byte) {
	t.Helper()
	if len(got) != len(want) {
		t.Fatalf("%s length mismatch: got %d want %d", label, len(got), len(want))
	}
	for i := range want {
		if len(got[i]) == 0 {
			t.Fatalf("%s[%d] is empty", label, i)
		}
		if !bytes.Equal(got[i], want[i]) {
			t.Fatalf("%s[%d] mismatch: got=%x want=%x", label, i, got[i], want[i])
		}
	}
}
