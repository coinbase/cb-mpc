package mpc

import (
	"fmt"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"
)

// ------------------------------------------------------------
// Helper types & functions
// ------------------------------------------------------------

type partyResult[T any] struct {
	idx int
	val T
	err error
}

// keyGenWithMockNet spins up `n` in-memory parties and runs ECDSAMPCKeyGen
// via the public API, returning the per-party responses.
func keyGenWithMockNet(n int, cv curve.Curve) ([]*ECDSAMPCKeyGenResponse, error) {
	pnames := mocknet.GeneratePartyNames(n)
	messengers := mocknet.NewMockNetwork(n)

	respCh := make(chan partyResult[*ECDSAMPCKeyGenResponse], n)

	for i := 0; i < n; i++ {
		go func(idx int) {
			j, err := NewJobMP(messengers[idx], n, idx, pnames)
			if err != nil {
				respCh <- partyResult[*ECDSAMPCKeyGenResponse]{idx: idx, val: nil, err: err}
				return
			}
			defer j.Free()

			r, e := ECDSAMPCKeyGen(j, &ECDSAMPCKeyGenRequest{Curve: cv})
			respCh <- partyResult[*ECDSAMPCKeyGenResponse]{idx: idx, val: r, err: e}
		}(i)
	}

	res := make([]*ECDSAMPCKeyGenResponse, n)
	for i := 0; i < n; i++ {
		out := <-respCh
		if out.err != nil {
			return nil, fmt.Errorf("party %d keygen failed: %v", out.idx, out.err)
		}
		res[out.idx] = out.val
	}
	return res, nil
}

// refreshWithMockNet performs the refresh protocol on the provided key shares.
func refreshWithMockNet(orig []*ECDSAMPCKeyGenResponse, sessionID []byte) ([]*ECDSAMPCRefreshResponse, error) {
	n := len(orig)
	pnames := mocknet.GeneratePartyNames(n)
	messengers := mocknet.NewMockNetwork(n)

	respCh := make(chan partyResult[*ECDSAMPCRefreshResponse], n)

	for i := 0; i < n; i++ {
		go func(idx int) {
			j, err := NewJobMP(messengers[idx], n, idx, pnames)
			if err != nil {
				respCh <- partyResult[*ECDSAMPCRefreshResponse]{idx: idx, val: nil, err: err}
				return
			}
			defer j.Free()

			req := &ECDSAMPCRefreshRequest{KeyShare: orig[idx].KeyShare, SessionID: sessionID}
			r, e := ECDSAMPCRefresh(j, req)
			respCh <- partyResult[*ECDSAMPCRefreshResponse]{idx: idx, val: r, err: e}
		}(i)
	}

	res := make([]*ECDSAMPCRefreshResponse, n)
	for i := 0; i < n; i++ {
		out := <-respCh
		if out.err != nil {
			return nil, fmt.Errorf("party %d refresh failed: %v", out.idx, out.err)
		}
		res[out.idx] = out.val
	}
	return res, nil
}

// signWithMockNet executes a signing round over the provided key shares.
// Only party `receiver` will obtain the resulting signature.
func signWithMockNet(keyShares []ECDSAMPCKey, msg []byte, receiver int) ([]*ECDSAMPCSignResponse, error) {
	n := len(keyShares)
	pnames := mocknet.GeneratePartyNames(n)
	messengers := mocknet.NewMockNetwork(n)

	respCh := make(chan partyResult[*ECDSAMPCSignResponse], n)

	for i := 0; i < n; i++ {
		go func(idx int) {
			j, err := NewJobMP(messengers[idx], n, idx, pnames)
			if err != nil {
				respCh <- partyResult[*ECDSAMPCSignResponse]{idx: idx, val: nil, err: err}
				return
			}
			defer j.Free()

			req := &ECDSAMPCSignRequest{
				KeyShare:          keyShares[idx],
				Message:           msg,
				SignatureReceiver: receiver,
			}
			r, e := ECDSAMPCSign(j, req)
			respCh <- partyResult[*ECDSAMPCSignResponse]{idx: idx, val: r, err: e}
		}(i)
	}

	res := make([]*ECDSAMPCSignResponse, n)
	for i := 0; i < n; i++ {
		out := <-respCh
		if out.err != nil {
			return nil, fmt.Errorf("party %d sign failed: %v", out.idx, out.err)
		}
		res[out.idx] = out.val
	}
	return res, nil
}

// ------------------------------------------------------------
// Tests
// ------------------------------------------------------------

func TestECDSAMPC_DKG_Validation(t *testing.T) {
	partyCounts := []int{3, 4, 5}

	for _, n := range partyCounts {
		t.Run(fmt.Sprintf("dkg_%d_parties", n), func(t *testing.T) {
			cv, err := curve.NewSecp256k1()
			require.NoError(t, err)
			defer cv.Free()

			keyGenRes, err := keyGenWithMockNet(n, cv)
			require.NoError(t, err)
			require.Len(t, keyGenRes, n)

			// --- collect shared data from first party ---
			firstKey := keyGenRes[0].KeyShare

			Qglobal, err := firstKey.Q()
			require.NoError(t, err)
			defer Qglobal.Free()

			QiMap, err := firstKey.Qis()
			require.NoError(t, err)

			expectedCode := curve.Code(cv)

			// --- per-party validations ---
			pnames := mocknet.GeneratePartyNames(n)
			for i, resp := range keyGenRes {
				ks := resp.KeyShare

				// party name correct
				pname, err := ks.PartyName()
				require.NoError(t, err)
				assert.Equal(t, pnames[i], pname, "party %d name mismatch", i)

				// curve matches
				c, err := ks.Curve()
				require.NoError(t, err)
				actualCode := curve.Code(c)
				assert.Equal(t, expectedCode, actualCode)
				c.Free()

				// Q identical
				Q, err := ks.Q()
				require.NoError(t, err)
				assert.True(t, Q.Equals(Qglobal), "party %d Q differs", i)
				Q.Free()

				// Qi maps identical
				QiMapOther, err := ks.Qis()
				require.NoError(t, err)
				require.Equal(t, len(QiMap), len(QiMapOther))
				for k, v := range QiMap {
					other := QiMapOther[k]
					assert.True(t, v.Equals(other), "party %d Qi for %s differs", i, k)
					other.Free()
				}

				// x_share consistency
				x, err := ks.XShare()
				require.NoError(t, err)
				QiParty := QiMap[pname]
				expectedQi, err := cv.MultiplyGenerator(x)
				require.NoError(t, err)
				assert.True(t, expectedQi.Equals(QiParty), "party %d Qi != x_i*G", i)
				expectedQi.Free()
			}

			// --- Sum(Qi) == Q ---
			var sumPt *curve.Point
			// We need a stable iteration order; use pnames slice
			for i, name := range pnames {
				pt := QiMap[name]
				if i == 0 {
					sumPt = pt // borrow reference; do not free here
					continue
				}
				tmp := sumPt.Add(pt)
				if i != 0 {
					// Free previous accumulator if it was not one of the Qi map entries
					if sumPt != pt { // avoid double-free
						sumPt.Free()
					}
				}
				sumPt = tmp
			}
			assert.True(t, sumPt.Equals(Qglobal), "sum(Qi) != Q")

			// Free accumulator if it is not one of original Qi values
			accIsQi := false
			for _, pt := range QiMap {
				if pt == sumPt {
					accIsQi = true
					break
				}
			}
			if !accIsQi {
				sumPt.Free()
			}
		})
	}
}

func TestECDSAMPC_Refresh(t *testing.T) {
	const nParties = 3
	cv, err := curve.NewSecp256k1()
	require.NoError(t, err)
	defer cv.Free()

	// --- initial keygen ---
	keyGenRes, err := keyGenWithMockNet(nParties, cv)
	require.NoError(t, err)

	// capture original x_shares & Q
	origX := make([]*curve.Scalar, nParties)
	for i, ks := range keyGenRes {
		x, err := ks.KeyShare.XShare()
		require.NoError(t, err)
		origX[i] = x
	}
	Qorig, err := keyGenRes[0].KeyShare.Q()
	require.NoError(t, err)
	defer Qorig.Free()

	// --- refresh ---
	refreshRes, err := refreshWithMockNet(keyGenRes, nil)
	require.NoError(t, err)

	// validations
	for i := 0; i < nParties; i++ {
		newShare := refreshRes[i].NewKeyShare

		// x_share changed
		newX, err := newShare.XShare()
		require.NoError(t, err)
		assert.False(t, newX.Equal(origX[i]), "party %d x_share should change after refresh", i)

		// Q unchanged
		Qnew, err := newShare.Q()
		require.NoError(t, err)
		assert.True(t, Qnew.Equals(Qorig), "party %d Q changed after refresh", i)
		Qnew.Free()
	}
}

func TestECDSAMPC_Sign_Refresh_Sign(t *testing.T) {
	const nParties = 3
	cv, err := curve.NewSecp256k1()
	require.NoError(t, err)
	defer cv.Free()

	// Key generation
	keyGenRes, err := keyGenWithMockNet(nParties, cv)
	require.NoError(t, err)

	keyShares := make([]ECDSAMPCKey, nParties)
	for i, r := range keyGenRes {
		keyShares[i] = r.KeyShare
	}

	msg1 := []byte("first message")

	// Sign before refresh
	sigRes1, err := signWithMockNet(keyShares, msg1, 0)
	require.NoError(t, err)

	// Only receiver (0) gets signature
	assert.Greater(t, len(sigRes1[0].Signature), 0)
	for i := 1; i < nParties; i++ {
		assert.Equal(t, 0, len(sigRes1[i].Signature))
	}

	// Refresh
	refreshRes, err := refreshWithMockNet(keyGenRes, nil)
	require.NoError(t, err)

	newShares := make([]ECDSAMPCKey, nParties)
	for i, r := range refreshRes {
		newShares[i] = r.NewKeyShare
	}

	// Sign after refresh (same message for simplicity)
	sigRes2, err := signWithMockNet(newShares, msg1, 0)
	require.NoError(t, err)
	assert.Greater(t, len(sigRes2[0].Signature), 0)

	// Signatures should differ
	assert.NotEqual(t, sigRes1[0].Signature, sigRes2[0].Signature)
}

func TestECDSAMPC_SerializeDeserialize(t *testing.T) {
	const nParties = 3

	cv, err := curve.NewSecp256k1()
	require.NoError(t, err)
	defer cv.Free()

	keyGenRes, err := keyGenWithMockNet(nParties, cv)
	require.NoError(t, err)

	deserShares := make([]ECDSAMPCKey, nParties)
	for i, res := range keyGenRes {
		ser, err := res.KeyShare.MarshalBinary()
		require.NoError(t, err)
		assert.Greater(t, len(ser), 0, "serialized data should not be empty")

		var newKey ECDSAMPCKey
		err = newKey.UnmarshalBinary(ser)
		require.NoError(t, err)

		// Party name should stay identical
		origPName, err := res.KeyShare.PartyName()
		require.NoError(t, err)
		newPName, err := newKey.PartyName()
		require.NoError(t, err)
		assert.Equal(t, origPName, newPName, "party %d name mismatch after serde", i)

		// Public key Q must match
		origQ, err := res.KeyShare.Q()
		require.NoError(t, err)
		newQ, err := newKey.Q()
		require.NoError(t, err)
		assert.True(t, origQ.Equals(newQ), "party %d Q mismatch after serde", i)
		origQ.Free()
		newQ.Free()

		deserShares[i] = newKey
	}

	message := []byte("serde-test-message")
	sigRes, err := signWithMockNet(deserShares, message, 0)
	require.NoError(t, err)

	assert.Greater(t, len(sigRes[0].Signature), 0, "receiver should get a signature")
}
