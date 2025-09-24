// Replace placeholder with test implementations
package mpc

import (
	"testing"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

// createThresholdAccessStructure builds an in-memory AccessStructure tree
// representing a simple "threshold-of-n" policy and returns a high-level
// Go wrapper that can be passed to the MPC APIs.
func createThresholdAccessStructure(pnames []string, threshold int, cv curve.Curve) *AccessStructure {
	// Build leaf nodes for each party.
	kids := make([]*AccessNode, len(pnames))
	for i, n := range pnames {
		kids[i] = Leaf(n)
	}

	// Root is a THRESHOLD node with K=threshold.
	root := Threshold("", threshold, kids...)

	return &AccessStructure{Root: root, Curve: cv}
}

// TestECDSAMPCThresholdDKGWithMockNet exercises the high-level
// ECDSAMPCThresholdDKG wrapper across multiple parties using the in-memory mock
// network. It validates that each participant receives a non-nil key share and
// that basic invariants (party name, curve code) hold.
func TestECDSAMPCThresholdDKGWithMockNet(t *testing.T) {
	const (
		nParties  = 3
		threshold = 2
	)

	// Prepare curve instance.
	cv, err := curve.NewSecp256k1()
	require.NoError(t, err)
	defer cv.Free()

	// Prepare mock network primitives.
	pnames := mocknet.GeneratePartyNames(nParties)
	messengers := mocknet.NewMockNetwork(nParties)

	// Channel to gather per-party results.
	type result struct {
		idx  int
		resp *ECDSAMPCThresholdDKGResponse
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

			req := &ECDSAMPCThresholdDKGRequest{
				Curve:           cv,
				SessionID:       nil, // let native generate SID
				AccessStructure: ac,
			}

			r, e := ECDSAMPCThresholdDKG(job, req)
			resCh <- result{idx: idx, resp: r, err: e}
		}(i)
	}

	// Collect results.
	resp := make([]*ECDSAMPCThresholdDKGResponse, nParties)
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
	}
}

// TestECDSAMPC_ToAdditiveShare verifies that a subset of parties satisfying the
// quorum threshold can convert their threshold-DKG key share into an additive
// secret share without error.
func TestECDSAMPC_ToAdditiveShare(t *testing.T) {
	const (
		nParties  = 4
		threshold = 2
	)

	cv, err := curve.NewSecp256k1()
	require.NoError(t, err)
	defer cv.Free()

	pnames := mocknet.GeneratePartyNames(nParties)
	messengers := mocknet.NewMockNetwork(nParties)

	// First run threshold DKG to obtain key shares.
	type dkgResult struct {
		idx   int
		share ECDSAMPCKey
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

			req := &ECDSAMPCThresholdDKGRequest{Curve: cv, AccessStructure: ac}
			resp, err := ECDSAMPCThresholdDKG(job, req)
			if err != nil {
				dkgCh <- dkgResult{idx: idx, err: err}
				return
			}
			dkgCh <- dkgResult{idx: idx, share: resp.KeyShare, err: nil}
		}(i)
	}

	shares := make([]ECDSAMPCKey, nParties)
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
