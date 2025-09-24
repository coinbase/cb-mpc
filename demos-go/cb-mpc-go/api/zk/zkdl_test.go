package zk

import (
	"testing"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/testutil"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

// curvesUnderTest returns instances of all curves supported by the Go wrapper.
// The caller must invoke Free on every returned curve.
func curvesUnderTest(t *testing.T) []curve.Curve {
	t.Helper()

	secp, err := curve.NewSecp256k1()
	require.NoError(t, err)
	p256, err := curve.NewP256()
	require.NoError(t, err)
	ed, err := curve.NewEd25519()
	require.NoError(t, err)

	return []curve.Curve{secp, p256, ed}
}

// TestProveAndVerifySuccess ensures that a proof created with ZKUCDLProve can be
// verified with ZKUCDLVerify for every supported curve.
func TestProveAndVerifySuccess(t *testing.T) {
	for _, c := range curvesUnderTest(t) {
		c := c // capture for parallel sub-test safety
		t.Run(c.String(), func(t *testing.T) {
			defer c.Free()

			w, W, err := c.RandomKeyPair()
			require.NoError(t, err)

			sessionID := []byte("session-" + c.String())
			auxiliary := uint64(42)

			pr, err := ZKUCDLProve(&ZKUCDLProveRequest{
				PublicKey: W,
				Witness:   w,
				SessionID: sessionID,
				Auxiliary: auxiliary,
			})
			require.NoError(t, err)
			require.NotEmpty(t, pr.Proof)

			vr, err := ZKUCDLVerify(&ZKUCDLVerifyRequest{
				PublicKey: W,
				Proof:     pr.Proof,
				SessionID: sessionID,
				Auxiliary: auxiliary,
			})
			require.NoError(t, err)
			assert.True(t, vr.Valid)
		})
	}
}

// TestVerifyRejectsTamperedProof modifies a valid proof and expects the
// verification to fail.
func TestVerifyRejectsTamperedProof(t *testing.T) {
	c, err := curve.NewSecp256k1()
	require.NoError(t, err)
	defer c.Free()

	w, W, err := c.RandomKeyPair()
	require.NoError(t, err)

	sessionID := []byte("tamper")
	auxiliary := uint64(1)

	pr, err := ZKUCDLProve(&ZKUCDLProveRequest{PublicKey: W, Witness: w, SessionID: sessionID, Auxiliary: auxiliary})
	require.NoError(t, err)

	// Flip first byte to invalidate the proof.
	corrupted := append([]byte{}, pr.Proof...)
	if len(corrupted) > 0 {
		corrupted[0] ^= 0xFF
	}

	var vr *ZKUCDLVerifyResponse
	testutil.TSilence(t, func(t *testing.T) {
		var err2 error
		vr, err2 = ZKUCDLVerify(&ZKUCDLVerifyRequest{PublicKey: W, Proof: corrupted, SessionID: sessionID, Auxiliary: auxiliary})
		require.NoError(t, err2)
	})
	assert.False(t, vr.Valid)
}

// TestProveRejectsMalformedPublicKey ensures that ZKUCDLProve returns an error
// when provided with an invalid public key (nil).
func TestProveRejectsMalformedPublicKey(t *testing.T) {
	c, err := curve.NewSecp256k1()
	require.NoError(t, err)
	defer c.Free()

	// Valid non-nil witness
	w := curve.NewScalarFromInt64(1)

	_, err = ZKUCDLProve(&ZKUCDLProveRequest{
		PublicKey: nil,
		Witness:   w,
		SessionID: []byte("badpk"),
		Auxiliary: 0,
	})
	require.Error(t, err)
	assert.Contains(t, err.Error(), "public key cannot be nil")
}

// TestVerifyRejectsMalformedPublicKey ensures that ZKUCDLVerify returns an error
// when provided with an invalid public key (nil).
func TestVerifyRejectsMalformedPublicKey(t *testing.T) {
	c, err := curve.NewSecp256k1()
	require.NoError(t, err)
	defer c.Free()

	// Provide any non-empty proof bytes to bypass the proof length check and
	// exercise the public key validation branch first.
	bogusProof := []byte{0x01}

	_, err = ZKUCDLVerify(&ZKUCDLVerifyRequest{
		PublicKey: nil,
		Proof:     bogusProof,
		SessionID: []byte("badpk"),
		Auxiliary: 0,
	})
	require.Error(t, err)
	assert.Contains(t, err.Error(), "public key cannot be nil")
}
