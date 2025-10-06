package mpc

import (
	"crypto/ed25519"
	"testing"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

// TestEDDSAMPCThresholdDKGWithMockNet exercises the high-level
// EDDSAMPCThresholdDKG wrapper across multiple parties using the in-memory mock
// network. It validates that the threshold DKG protocol works and that the
// resulting key shares can be used to sign a message.
func TestEDDSAMPCThresholdDKGWithMockNet(t *testing.T) {
	const (
		nParties  = 5
		threshold = 3 // 3-of-5 threshold policy
	)

	// Prepare curve instance.
	cv, err := curve.NewEd25519()
	require.NoError(t, err)
	defer cv.Free()

	// Prepare mock network primitives.
	pnames := mocknet.GeneratePartyNames(nParties)
	messengers := mocknet.NewMockNetwork(nParties)

	// Channel to gather per-party results.
	type result struct {
		idx  int
		resp *EDDSAMPCThresholdDKGResponse
		err  error
	}
	resCh := make(chan result, nParties)

	// Launch one goroutine per party.
	for i := 0; i < nParties; i++ {
		go func(idx int) {
			// Build JobMP wrapper for this party.
			job, err := NewJobMP(messengers[idx], nParties, idx, pnames)
			if err != nil {
				resCh <- result{idx: idx, resp: nil, err: err}
				return
			}
			defer job.Free()

			// Each party creates its own access-structure object.
			ac := createThresholdAccessStructure(pnames, threshold, cv)

			req := &EDDSAMPCThresholdDKGRequest{
				Curve:           cv,
				SessionID:       nil, // let native generate SID
				AccessStructure: ac,
			}

			r, e := EDDSAMPCThresholdDKG(job, req)
			resCh <- result{idx: idx, resp: r, err: e}
		}(i)
	}

	// Collect results.
	resp := make([]*EDDSAMPCThresholdDKGResponse, nParties)
	for i := 0; i < nParties; i++ {
		out := <-resCh
		require.NoError(t, out.err, "party %d threshold DKG should succeed", out.idx)
		require.NotNil(t, out.resp, "party %d response must not be nil", out.idx)
		resp[out.idx] = out.resp
	}

	// Basic validations.
	expectedCurveCode := curve.Code(cv)

	for i, r := range resp {
		// Key share must be non-zero.
		assert.NotEqual(t, 0, r.KeyShare, "party %d key share should not be zero", i)

		// Party name matches.
		pname, err := r.KeyShare.PartyName()
		require.NoError(t, err)
		assert.Equal(t, pnames[i], pname, "party %d pname mismatch", i)

		// Curve matches.
		c, err := r.KeyShare.Curve()
		require.NoError(t, err)
		actual := curve.Code(c)
		assert.Equal(t, expectedCurveCode, actual)
		c.Free()

		// Note: For threshold-DKG keys, SUM(Qis) may not equal Q until converted to additive shares.
	}

	// Convert to additive shares for a quorum of size `threshold`
	root := Threshold("", threshold, func() []*AccessNode {
		kids := make([]*AccessNode, len(pnames))
		for i, n := range pnames {
			kids[i] = Leaf(n)
		}
		return kids
	}()...)
	acQ := &AccessStructure{Root: root, Curve: cv}
	quorumNames := pnames[:threshold]
	additive := make([]EDDSAMPCKey, threshold)
	for i := 0; i < threshold; i++ {
		as, err := resp[i].KeyShare.ToAdditiveShare(acQ, quorumNames)
		require.NoError(t, err, "party %d additive share conversion failed", i)
		additive[i] = as
	}

	// Run an EdDSA MPC signing round with only the quorum parties using additive shares
	message := []byte("eddsa threshold dkg signing")
	sigReceiver := 0

	// Fresh mock network for signing across quorum parties
	signMessengers := mocknet.NewMockNetwork(threshold)
	signPNames := quorumNames

	type signResult struct {
		idx int
		sig []byte
		err error
	}
	signCh := make(chan signResult, threshold)

	for i := 0; i < threshold; i++ {
		go func(idx int) {
			job, err := NewJobMP(signMessengers[idx], threshold, idx, signPNames)
			if err != nil {
				signCh <- signResult{idx: idx, err: err}
				return
			}
			defer job.Free()

			req := &EDDSAMPCSignRequest{
				KeyShare:          additive[idx],
				Message:           message,
				SignatureReceiver: sigReceiver,
			}
			r, e := EDDSAMPCSign(job, req)
			if e != nil {
				signCh <- signResult{idx: idx, err: e}
				return
			}
			signCh <- signResult{idx: idx, sig: r.Signature, err: nil}
		}(i)
	}

	sigs := make([][]byte, threshold)
	for i := 0; i < threshold; i++ {
		out := <-signCh
		require.NoError(t, out.err, "party %d signing should succeed", out.idx)
		sigs[out.idx] = out.sig
	}

	// Only the receiver should obtain the signature bytes.
	require.NotEmpty(t, sigs[sigReceiver], "receiver should have signature bytes")
	for i := 0; i < threshold; i++ {
		if i == sigReceiver {
			continue
		}
		assert.Empty(t, sigs[i], "non-receiver party %d should not have signature", i)
	}

	// Verify the signature against the aggregated public key Q using Ed25519.
	Q, err := resp[0].KeyShare.Q()
	require.NoError(t, err)
	pub, err := ed25519PublicKeyFromPoint(Q)
	Q.Free()
	require.NoError(t, err)
	require.Len(t, sigs[sigReceiver], ed25519.SignatureSize)
	valid := ed25519.Verify(ed25519.PublicKey(pub), message, sigs[sigReceiver])
	require.True(t, valid, "signature verification failed")
}

// TestEDDSAMPC_ToAdditiveShare verifies that a subset of parties satisfying the
// quorum threshold can convert their threshold-DKG key share into an additive
// secret share without error.
func TestEDDSAMPC_ToAdditiveShare(t *testing.T) {
	const (
		nParties  = 4
		threshold = 2
	)

	cv, err := curve.NewEd25519()
	require.NoError(t, err)
	defer cv.Free()

	pnames := mocknet.GeneratePartyNames(nParties)
	messengers := mocknet.NewMockNetwork(nParties)

	// First run threshold DKG to obtain key shares.
	type dkgResult struct {
		idx   int
		share EDDSAMPCKey
		err   error
	}
	dkgCh := make(chan dkgResult, nParties)

	for i := 0; i < nParties; i++ {
		go func(idx int) {
			job, err := NewJobMP(messengers[idx], nParties, idx, pnames)
			if err != nil {
				dkgCh <- dkgResult{idx: idx, err: err}
				return
			}
			defer job.Free()

			ac := createThresholdAccessStructure(pnames, threshold, cv)

			req := &EDDSAMPCThresholdDKGRequest{Curve: cv, AccessStructure: ac}
			resp, err := EDDSAMPCThresholdDKG(job, req)
			if err != nil {
				dkgCh <- dkgResult{idx: idx, err: err}
				return
			}
			dkgCh <- dkgResult{idx: idx, share: resp.KeyShare, err: nil}
		}(i)
	}

	shares := make([]EDDSAMPCKey, nParties)
	for i := 0; i < nParties; i++ {
		out := <-dkgCh
		require.NoError(t, out.err)
		shares[out.idx] = out.share
	}

	// Prepare quorum party names – pick the first `threshold` parties.
	quorumPNames := pnames[:threshold]

	// Build an AccessStructure representing the same threshold policy.
	root := Threshold("", threshold, func() []*AccessNode {
		kids := make([]*AccessNode, len(pnames))
		for i, n := range pnames {
			kids[i] = Leaf(n)
		}
		return kids
	}()...)

	asQ := &AccessStructure{Root: root, Curve: cv}

	// Convert shares for the quorum parties and ensure success.
	for i := 0; i < threshold; i++ {
		additive, err := shares[i].ToAdditiveShare(asQ, quorumPNames)
		require.NoError(t, err, "party %d additive share conversion failed", i)
		assert.NotEqual(t, 0, additive, "party %d additive share should not be zero", i)
		// Clean up native resources to avoid leaks.
		ref := additive.cgobindingRef()
		(&ref).Free()
	}

	// Non-quorum parties can also convert to additive shares; ensure no error
	for i := threshold; i < nParties; i++ {
		additive, err := shares[i].ToAdditiveShare(asQ, quorumPNames)
		require.NoError(t, err, "non-quorum party %d additive conversion failed", i)
		assert.NotEqual(t, 0, additive, "non-quorum party %d additive share should not be zero", i)
		ref := additive.cgobindingRef()
		(&ref).Free()
	}
}

// TestEDDSAMPCThresholdDKG_SigningFailsWithTooFewParties ensures that attempting
// to sign with fewer than 3 parties (below the protocol minimum and below the
// 3-of-5 threshold) fails as expected.
func TestEDDSAMPCThresholdDKG_SigningFailsWithTooFewParties(t *testing.T) {
	const (
		nParties  = 5
		threshold = 3
	)

	cv, err := curve.NewEd25519()
	require.NoError(t, err)
	defer cv.Free()

	pnames := mocknet.GeneratePartyNames(nParties)
	messengers := mocknet.NewMockNetwork(nParties)

	// Run threshold DKG across all parties
	type dkgRes struct {
		idx  int
		resp *EDDSAMPCThresholdDKGResponse
		err  error
	}
	dkgCh := make(chan dkgRes, nParties)
	for i := 0; i < nParties; i++ {
		go func(idx int) {
			job, err := NewJobMP(messengers[idx], nParties, idx, pnames)
			if err != nil {
				dkgCh <- dkgRes{idx: idx, err: err}
				return
			}
			defer job.Free()
			ac := createThresholdAccessStructure(pnames, threshold, cv)
			r, e := EDDSAMPCThresholdDKG(job, &EDDSAMPCThresholdDKGRequest{Curve: cv, AccessStructure: ac})
			dkgCh <- dkgRes{idx: idx, resp: r, err: e}
		}(i)
	}
	resp := make([]*EDDSAMPCThresholdDKGResponse, nParties)
	for i := 0; i < nParties; i++ {
		out := <-dkgCh
		require.NoError(t, out.err)
		resp[out.idx] = out.resp
	}

	// Convert to additive shares for a valid 3-of-5 quorum, but we will only
	// attempt to sign with TWO parties to trigger failure.
	root := Threshold("", threshold, func() []*AccessNode {
		kids := make([]*AccessNode, len(pnames))
		for i, n := range pnames {
			kids[i] = Leaf(n)
		}
		return kids
	}()...)
	asQ := &AccessStructure{Root: root, Curve: cv}
	quorumNames := pnames[:threshold]
	additive := make([]EDDSAMPCKey, threshold)
	for i := 0; i < threshold; i++ {
		as, err := resp[i].KeyShare.ToAdditiveShare(asQ, quorumNames)
		require.NoError(t, err)
		additive[i] = as
	}

	// Use ONLY two parties for signing – should fail with "n-party signing requires at least 3 parties"
	signMessengers := mocknet.NewMockNetwork(2)
	signPNames := quorumNames[:2]
	type signResult struct {
		idx int
		err error
	}
	signCh := make(chan signResult, 2)
	message := []byte("eddsa threshold negative test")
	sigReceiver := 0
	for i := 0; i < 2; i++ {
		go func(idx int) {
			job, err := NewJobMP(signMessengers[idx], 2, idx, signPNames)
			if err != nil {
				signCh <- signResult{idx: idx, err: err}
				return
			}
			defer job.Free()
			_, e := EDDSAMPCSign(job, &EDDSAMPCSignRequest{KeyShare: additive[idx], Message: message, SignatureReceiver: sigReceiver})
			signCh <- signResult{idx: idx, err: e}
		}(i)
	}
	for i := 0; i < 2; i++ {
		out := <-signCh
		require.Error(t, out.err, "party %d signing should fail with too few parties", out.idx)
	}
}

// ed25519PublicKeyFromPoint helper is defined in eddsa_mp_test.go within the
// same package and reused here.
