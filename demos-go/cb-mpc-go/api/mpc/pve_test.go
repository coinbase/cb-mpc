package mpc

import (
	"testing"

	curvepkg "github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"

	"github.com/stretchr/testify/require"
)

// TestPVEEncryptDecrypt performs a full encrypt → decrypt round-trip on a simple threshold access structure.
func TestPVEEncryptDecrypt(t *testing.T) {
	// t.Skip("PVE decrypt currently failing verification – needs point serialization fix")

	const (
		nParties  = 5
		threshold = 3
	)

	// Prepare curve instance (use P-256 for speed).
	cv, err := curvepkg.NewP256()
	require.NoError(t, err)
	defer cv.Free()

	// Party names and access structure.
	pnames := mocknet.GeneratePartyNames(nParties)
	ac := createThresholdAccessStructure(pnames, threshold, cv)

	// Generate base encryption key pairs for every leaf.
	pubMap := make(map[string]BaseEncPublicKey, nParties)
	prvMap := make(map[string]BaseEncPrivateKey, nParties)

	for _, name := range pnames {
		pub, prv, err := GenerateBaseEncKeypair()
		require.NoError(t, err)
		pubMap[name] = pub
		prvMap[name] = prv
	}

	// Generate random private values to back-up.
	privValues := make([]*curvepkg.Scalar, nParties)
	for i := 0; i < nParties; i++ {
		s, err := cv.RandomScalar()
		require.NoError(t, err)
		privValues[i] = s
	}

	pubShares := make([]*curvepkg.Point, nParties)
	for i, s := range privValues {
		pt, err := cv.MultiplyGenerator(s)
		require.NoError(t, err)
		pubShares[i] = pt
	}

	// Encrypt
	encResp, err := PVEEncrypt(&PVEEncryptRequest{
		AccessStructure: ac,
		PublicKeys:      pubMap,
		PrivateValues:   privValues,
		Label:           "unit-test-backup",
		Curve:           cv,
	})
	require.NoError(t, err)
	require.Greater(t, len(encResp.EncryptedBundle), 0, "ciphertext should not be empty")

	// Verify ciphertext prior to decryption
	verifyResp, err := PVEVerify(&PVEVerifyRequest{
		AccessStructure: ac,
		PublicKeys:      pubMap,
		EncryptedBundle: encResp.EncryptedBundle,
		PublicShares:    pubShares,
		Label:           "unit-test-backup",
	})
	require.NoError(t, err)
	require.NotNil(t, verifyResp)
	require.True(t, verifyResp.Valid, "verification should succeed on authentic ciphertext")

	// Tamper with ciphertext
	tampered := make([]byte, len(encResp.EncryptedBundle))
	copy(tampered, encResp.EncryptedBundle)
	if len(tampered) > 0 {
		tampered[0] ^= 0xFF // flip first byte
	}

	verifyResp, err = PVEVerify(&PVEVerifyRequest{
		AccessStructure: ac,
		PublicKeys:      pubMap,
		EncryptedBundle: PVECiphertext(tampered),
		PublicShares:    pubShares,
		Label:           "unit-test-backup",
	})
	require.Error(t, err)
	require.NotNil(t, verifyResp)
	require.False(t, verifyResp.Valid, "verification should fail on tampered ciphertext")

	// Decrypt
	decResp, err := PVEDecrypt(&PVEDecryptRequest{
		AccessStructure: ac,
		PrivateKeys:     prvMap,
		PublicKeys:      pubMap,
		EncryptedBundle: encResp.EncryptedBundle,
		PublicShares:    pubShares,
		Label:           "unit-test-backup",
	})
	require.NoError(t, err)
	require.Equal(t, len(privValues), len(decResp.PrivateValues))

	// Compare recovered values with originals.
	for i := 0; i < nParties; i++ {
		require.Equal(t, privValues[i].Bytes, decResp.PrivateValues[i].Bytes)
	}
}
