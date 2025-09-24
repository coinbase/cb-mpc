package mpc

import (
	"testing"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/testutil"

	"github.com/stretchr/testify/require"
)

// TestPVEAcEncryptDecrypt performs a full encrypt → decrypt round-trip on a simple threshold access structure.
func TestPVEAcEncryptDecrypt(t *testing.T) {
	const (
		nParties  = 5
		threshold = 3
	)

	// Prepare curve instance (use P-256 for speed).
	cv, err := curve.NewP256()
	require.NoError(t, err)
	defer cv.Free()

	// Party names and access structure.
	pnames := mocknet.GeneratePartyNames(nParties)
	ac := createThresholdAccessStructure(pnames, threshold, cv)

	// PVE handle with XOR KEM test backend (defined in pve_test.go)
	pve, err := NewPVE(Config{KEM: newTestXorKEM()})
	require.NoError(t, err)

	// Generate base encryption key pairs for every leaf using the test KEM.
	pubMap := make(map[string]BaseEncPublicKey, nParties)
	prvMap := make(map[string]BaseEncPrivateKey, nParties)
	for _, name := range pnames {
		dk, ek, err := newTestXorKEM().Generate()
		require.NoError(t, err)
		pubMap[name] = BaseEncPublicKey(ek)
		prvMap[name] = BaseEncPrivateKey(dk)
	}

	// Generate random private values to back-up.
	privValues := make([]*curve.Scalar, nParties)
	for i := 0; i < nParties; i++ {
		s, err := cv.RandomScalar()
		require.NoError(t, err)
		privValues[i] = s
	}

	pubShares := make([]*curve.Point, nParties)
	for i, s := range privValues {
		pt, err := cv.MultiplyGenerator(s)
		require.NoError(t, err)
		pubShares[i] = pt
	}

	// Encrypt
	encResp, err := pve.AcEncrypt(&PVEAcEncryptRequest{
		AccessStructure: ac,
		PublicKeys:      pubMap,
		PrivateValues:   privValues,
		Label:           "unit-test-backup",
		Curve:           cv,
	})
	require.NoError(t, err)
	require.Greater(t, len(encResp.EncryptedBundle), 0, "ciphertext should not be empty")

	// Verify ciphertext prior to decryption
	verifyResp, err := pve.AcVerify(&PVEAcVerifyRequest{
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

	testutil.TSilence(t, func(t *testing.T) {
		verifyResp, err = pve.AcVerify(&PVEAcVerifyRequest{
			AccessStructure: ac,
			PublicKeys:      pubMap,
			EncryptedBundle: PVECiphertext(tampered),
			PublicShares:    pubShares,
			Label:           "unit-test-backup",
		})
	})
	require.Error(t, err)
	require.NotNil(t, verifyResp)
	require.False(t, verifyResp.Valid, "verification should fail on tampered ciphertext")

	// Decrypt
	shares := make(map[string][]byte)
	for _, name := range pnames {
		resp, err := pve.AcPartyDecryptRow(&PVEAcPartyDecryptRowRequest{
			AccessStructure: ac,
			Path:            name,
			PrivateKey:      prvMap[name],
			EncryptedBundle: encResp.EncryptedBundle,
			Label:           "unit-test-backup",
			RowIndex:        0,
		})
		require.NoError(t, err)
		shares[name] = resp.Share
	}
	aggResp, err := pve.AcAggregateToRestoreRow(&PVEAcAggregateToRestoreRowRequest{
		AccessStructure: ac,
		EncryptedBundle: encResp.EncryptedBundle,
		Label:           "unit-test-backup",
		RowIndex:        0,
		Shares:          shares,
	})
	require.NoError(t, err)
	require.Equal(t, len(privValues), len(aggResp.PrivateValues))

	// Compare recovered values with originals.
	for i := 0; i < nParties; i++ {
		require.Equal(t, privValues[i].Bytes, aggResp.PrivateValues[i].Bytes)
	}
}

// TestPVEAcWithRSAHSMKEM verifies quorum PVE with an HSM-like RSA KEM backend.
func TestPVEAcWithRSAHSMKEM(t *testing.T) {
	const (
		nParties  = 4
		threshold = 2
	)

	cv, err := curve.NewP256()
	require.NoError(t, err)
	defer cv.Free()

	pnames := mocknet.GeneratePartyNames(nParties)
	ac := createThresholdAccessStructure(pnames, threshold, cv)

	// Use a single HSM-like KEM instance so handles resolve correctly.
	hsm := newRSAHSMKEM()
	pve, err := NewPVE(Config{KEM: hsm})
	require.NoError(t, err)

	pubMap := make(map[string]BaseEncPublicKey, nParties)
	prvMap := make(map[string]BaseEncPrivateKey, nParties)
	for _, name := range pnames {
		dk, ek, err := hsm.Generate()
		require.NoError(t, err)
		pubMap[name] = BaseEncPublicKey(ek)
		prvMap[name] = BaseEncPrivateKey(dk)
	}

	privValues := make([]*curve.Scalar, nParties)
	for i := 0; i < nParties; i++ {
		s, err := cv.RandomScalar()
		require.NoError(t, err)
		privValues[i] = s
	}

	pubShares := make([]*curve.Point, nParties)
	for i, s := range privValues {
		pt, err := cv.MultiplyGenerator(s)
		require.NoError(t, err)
		pubShares[i] = pt
	}

	encResp, err := pve.AcEncrypt(&PVEAcEncryptRequest{
		AccessStructure: ac,
		PublicKeys:      pubMap,
		PrivateValues:   privValues,
		Label:           "rsa-hsm-quorum",
		Curve:           cv,
	})
	require.NoError(t, err)
	require.NotNil(t, encResp)
	require.True(t, len(encResp.EncryptedBundle) > 0)

	verResp, err := pve.AcVerify(&PVEAcVerifyRequest{
		AccessStructure: ac,
		PublicKeys:      pubMap,
		EncryptedBundle: encResp.EncryptedBundle,
		PublicShares:    pubShares,
		Label:           "rsa-hsm-quorum",
	})
	require.NoError(t, err)
	require.True(t, verResp.Valid)

	// Interactive decryption for RSA-HSM KEM
	shares2 := make(map[string][]byte)
	for _, name := range pnames {
		resp, err := pve.AcPartyDecryptRow(&PVEAcPartyDecryptRowRequest{
			AccessStructure: ac,
			Path:            name,
			PrivateKey:      prvMap[name],
			EncryptedBundle: encResp.EncryptedBundle,
			Label:           "rsa-hsm-quorum",
			RowIndex:        0,
		})
		require.NoError(t, err)
		shares2[name] = resp.Share
	}
	aggResp2, err := pve.AcAggregateToRestoreRow(&PVEAcAggregateToRestoreRowRequest{
		AccessStructure: ac,
		EncryptedBundle: encResp.EncryptedBundle,
		Label:           "rsa-hsm-quorum",
		RowIndex:        0,
		Shares:          shares2,
	})
	require.NoError(t, err)
	require.Equal(t, len(privValues), len(aggResp2.PrivateValues))
	for i := 0; i < nParties; i++ {
		require.Equal(t, privValues[i].Bytes, aggResp2.PrivateValues[i].Bytes)
	}
}
