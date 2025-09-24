package mpc

import (
	"bytes"
	"fmt"
	"sync"
	"testing"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

func TestECDSA2PCKeyGenWithMockNet(t *testing.T) {
	secp, _ := curve.NewSecp256k1()

	// Valid case
	responses, err := ECDSA2PCKeyGenWithMockNet(secp)
	require.NoError(t, err)
	require.Len(t, responses, 2)

	for i, resp := range responses {
		assert.NotNil(t, resp, "response %d should not be nil", i)
		assert.NotEqual(t, resp.KeyShare, 0, "key share %d should not be zero", i)
	}

	// Invalid case: nil curve
	responsesNil, errNil := ECDSA2PCKeyGenWithMockNet(nil)
	assert.Error(t, errNil)
	assert.Nil(t, responsesNil)
}

func TestECDSA2PCFullProtocolWithMockNet(t *testing.T) {
	tests := []struct {
		name      string
		sessionID []byte
		message   []byte
		wantErr   bool
	}{
		{
			name:      "valid full protocol",
			sessionID: []byte("test-session"),
			message:   []byte("Hello, world!"),
			wantErr:   false,
		},
		{
			name:      "valid with empty session ID",
			sessionID: []byte{},
			message:   []byte("Test message"),
			wantErr:   false,
		},
		{
			name:      "invalid empty message",
			sessionID: []byte("test-session"),
			message:   []byte{},
			wantErr:   true,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			secp, _ := curve.NewSecp256k1()
			result, err := ECDSA2PCFullProtocolWithMockNet(secp, tt.sessionID, tt.message)

			if tt.wantErr {
				assert.Error(t, err)
				assert.Nil(t, result)
				return
			}

			require.NoError(t, err)
			require.NotNil(t, result)

			// Verify key generation results
			require.Len(t, result.KeyGenResponses, 2)
			for i, resp := range result.KeyGenResponses {
				assert.NotNil(t, resp, "key gen response %d should not be nil", i)
				assert.NotEqual(t, resp.KeyShare, 0, "key share %d should not be zero", i)
			}

			// Verify signing results
			require.Len(t, result.SignResponses, 2)

			// In ECDSA 2PC, only Party 0 gets the final signature
			assert.NotNil(t, result.SignResponses[0], "sign response 0 should not be nil")
			assert.NotNil(t, result.SignResponses[0].Signature, "signature 0 should not be nil")
			assert.NotEmpty(t, result.SignResponses[0].Signature, "signature 0 should not be empty")

			// Party 1 contributes to signing but doesn't receive the final signature
			assert.NotNil(t, result.SignResponses[1], "sign response 1 should not be nil")
			assert.NotNil(t, result.SignResponses[1].Signature, "signature 1 should not be nil")
			assert.Empty(t, result.SignResponses[1].Signature, "signature 1 should be empty (expected behavior)")
		})
	}
}

func TestECDSA2PCKeyGenRequest_Validation(t *testing.T) {
	secp, _ := curve.NewSecp256k1()
	_, err := ECDSA2PCKeyGenWithMockNet(secp)
	assert.NoError(t, err)

	// Nil curve should error
	_, errNil := ECDSA2PCKeyGenWithMockNet(nil)
	assert.Error(t, errNil)
}

func TestECDSA2PCSignRequest_Validation(t *testing.T) {
	// First generate valid key shares for testing
	secp, _ := curve.NewSecp256k1()
	keyGenResponses, err := ECDSA2PCKeyGenWithMockNet(secp)
	require.NoError(t, err)
	require.Len(t, keyGenResponses, 2)

	tests := []struct {
		name    string
		message []byte
		wantErr bool
	}{
		{"valid message", []byte("Hello, world!"), false},
		{"invalid empty message", []byte{}, true},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			sessionID := []byte("test-session")
			_, err := ECDSA2PCFullProtocolWithMockNet(secp, sessionID, tt.message)

			if tt.wantErr {
				assert.Error(t, err)
			} else {
				assert.NoError(t, err)
			}
		})
	}
}

func TestECDSA2PC_DeterministicSignatures(t *testing.T) {
	// Test that Party 0 consistently gets signatures and Party 1 gets empty results
	secp, _ := curve.NewSecp256k1()
	sessionID := []byte("deterministic-test")
	message := []byte("Consistent message")

	// Run the protocol multiple times
	for i := 0; i < 3; i++ {
		result, err := ECDSA2PCFullProtocolWithMockNet(secp, sessionID, message)
		require.NoError(t, err, "iteration %d should succeed", i)
		require.NotNil(t, result)

		// Party 0 should always get a signature
		assert.NotEmpty(t, result.SignResponses[0].Signature, "iteration %d: Party 0 should get signature", i)

		// Party 1 should always get an empty signature (this is expected behavior)
		assert.Empty(t, result.SignResponses[1].Signature, "iteration %d: Party 1 should get empty signature", i)
	}
}

func TestECDSA2PC_DifferentMessages(t *testing.T) {
	// Test that different messages produce different signatures (from Party 0)
	secp, _ := curve.NewSecp256k1()
	sessionID := []byte("different-messages-test")
	message1 := []byte("First message")
	message2 := []byte("Second message")

	// Sign first message
	result1, err := ECDSA2PCFullProtocolWithMockNet(secp, sessionID, message1)
	require.NoError(t, err)
	require.NotNil(t, result1)
	require.NotEmpty(t, result1.SignResponses[0].Signature, "first signature should not be empty")

	// Sign second message
	result2, err := ECDSA2PCFullProtocolWithMockNet(secp, sessionID, message2)
	require.NoError(t, err)
	require.NotNil(t, result2)
	require.NotEmpty(t, result2.SignResponses[0].Signature, "second signature should not be empty")

	// Different messages should produce different signatures (from Party 0)
	assert.False(t, bytes.Equal(result1.SignResponses[0].Signature, result2.SignResponses[0].Signature),
		"different messages should produce different signatures")
}

func TestECDSA2PC_StructureValidation(t *testing.T) {
	// Test that all response structures are properly populated
	secp, _ := curve.NewSecp256k1()
	result, err := ECDSA2PCFullProtocolWithMockNet(secp, []byte("structure-test"), []byte("Test message"))
	require.NoError(t, err)
	require.NotNil(t, result)

	// Verify ECDSA2PCResult structure
	assert.NotNil(t, result.KeyGenResponses, "KeyGenResponses should not be nil")
	assert.NotNil(t, result.SignResponses, "SignResponses should not be nil")
	assert.Len(t, result.KeyGenResponses, 2, "should have 2 key gen responses")
	assert.Len(t, result.SignResponses, 2, "should have 2 sign responses")

	// Verify KeyGenResponse structure
	for i, resp := range result.KeyGenResponses {
		assert.NotNil(t, resp, "key gen response %d should not be nil", i)
		// KeyShare is an opaque pointer, can't easily verify its contents
	}

	// Verify SignResponse structure
	// Party 0 should have a signature
	assert.NotNil(t, result.SignResponses[0], "sign response 0 should not be nil")
	assert.NotNil(t, result.SignResponses[0].Signature, "signature 0 should not be nil")
	assert.Greater(t, len(result.SignResponses[0].Signature), 0, "signature 0 should have positive length")

	// Party 1 should have empty signature (expected behavior)
	assert.NotNil(t, result.SignResponses[1], "sign response 1 should not be nil")
	assert.NotNil(t, result.SignResponses[1].Signature, "signature 1 should not be nil")
	assert.Equal(t, 0, len(result.SignResponses[1].Signature), "signature 1 should be empty (expected)")
}

func TestECDSA2PCKey_RoleIndex(t *testing.T) {
	// Generate key shares for two parties
	secp, _ := curve.NewSecp256k1()
	keyGenResponses, err := ECDSA2PCKeyGenWithMockNet(secp)
	require.NoError(t, err)
	require.Len(t, keyGenResponses, 2)

	// Party 0
	idx0, err := keyGenResponses[0].KeyShare.RoleIndex()
	require.NoError(t, err, "party 0 RoleIndex should not error")
	assert.Equal(t, 0, idx0, "party 0 should have role index 0")

	// Party 1
	idx1, err := keyGenResponses[1].KeyShare.RoleIndex()
	require.NoError(t, err, "party 1 RoleIndex should not error")
	assert.Equal(t, 1, idx1, "party 1 should have role index 1")
}

func TestECDSA2PCKey_QAndXShare(t *testing.T) {
	// Generate key shares
	secp, _ := curve.NewSecp256k1()
	keyGenResponses, err := ECDSA2PCKeyGenWithMockNet(secp)
	require.NoError(t, err)
	require.Len(t, keyGenResponses, 2)

	// Extract curve
	curveObj, err := curve.NewSecp256k1()
	require.NoError(t, err)
	defer curveObj.Free()

	// Q from both parties should be the same
	Q0, err := keyGenResponses[0].KeyShare.Q()
	require.NoError(t, err)
	defer Q0.Free()

	Q1, err := keyGenResponses[1].KeyShare.Q()
	require.NoError(t, err)
	defer Q1.Free()

	assert.True(t, Q0.Equals(Q1), "public key points should match across parties")

	// x shares
	x0, err := keyGenResponses[0].KeyShare.XShare()
	require.NoError(t, err)
	x1, err := keyGenResponses[1].KeyShare.XShare()
	require.NoError(t, err)

	// x_sum = x0 + x1 mod order
	xSum, err := curveObj.Add(x0, x1)
	require.NoError(t, err)

	// G * x_sum should equal Q
	GxSum, err := curveObj.MultiplyGenerator(xSum)
	require.NoError(t, err)
	defer GxSum.Free()

	assert.True(t, GxSum.Equals(Q0), "G * (x0 + x1) should equal Q")
}

// ECDSA2PCResult represents the complete result of key generation and signing
type ECDSA2PCResult struct {
	KeyGenResponses []*ECDSA2PCKeyGenResponse
	SignResponses   []*ECDSA2PCSignResponse
}

// ECDSA2PCFullProtocolWithMockNet runs the complete ECDSA 2PC protocol (key generation + signing)
// using the mock network. This is a test-only helper that provides a convenient way to exercise
// the full protocol flow.
func ECDSA2PCFullProtocolWithMockNet(curveObj curve.Curve, sessionID []byte, message []byte) (*ECDSA2PCResult, error) {
	if curveObj == nil {
		return nil, fmt.Errorf("curve must be provided")
	}
	if len(message) == 0 {
		return nil, fmt.Errorf("message cannot be empty")
	}

	// Use MPCRunner for proper coordination between parties
	runner := mocknet.NewMPCRunner(mocknet.GeneratePartyNames(2)...)

	// Step 1: Distributed Key Generation
	keyGenOutputs, err := runner.MPCRun2P(func(job cgobinding.Job2P, input *mocknet.MPCIO) (*mocknet.MPCIO, error) {
		cv := input.Opaque.(curve.Curve)
		keyShareRef, err := cgobinding.DistributedKeyGen(job, curve.Code(cv))
		if err != nil {
			return nil, fmt.Errorf("key generation failed: %v", err)
		}
		return &mocknet.MPCIO{Opaque: keyShareRef}, nil
	}, []*mocknet.MPCIO{
		{Opaque: curveObj},
		{Opaque: curveObj},
	})
	if err != nil {
		return nil, fmt.Errorf("key generation failed: %v", err)
	}

	// Extract key shares from outputs
	keyShare0 := ECDSA2PCKey(keyGenOutputs[0].Opaque.(cgobinding.Mpc_ecdsa2pc_key_ref))
	keyShare1 := ECDSA2PCKey(keyGenOutputs[1].Opaque.(cgobinding.Mpc_ecdsa2pc_key_ref))

	// Step 2: Collaborative Signing
	type signInput struct {
		SessionID []byte
		KeyShare  ECDSA2PCKey
		Message   []byte
	}

	signOutputs, err := runner.MPCRun2P(func(job cgobinding.Job2P, input *mocknet.MPCIO) (*mocknet.MPCIO, error) {
		signInput := input.Opaque.(signInput)
		messages := [][]byte{signInput.Message}
		signatures, err := cgobinding.Sign(job, signInput.SessionID, signInput.KeyShare.cgobindingRef(), messages)
		if err != nil {
			return nil, fmt.Errorf("signing failed: %v", err)
		}
		if len(signatures) == 0 {
			return nil, fmt.Errorf("no signature returned")
		}
		return &mocknet.MPCIO{Opaque: signatures[0]}, nil
	}, []*mocknet.MPCIO{
		{Opaque: signInput{
			SessionID: sessionID,
			KeyShare:  keyShare0,
			Message:   message,
		}},
		{Opaque: signInput{
			SessionID: sessionID,
			KeyShare:  keyShare1,
			Message:   message,
		}},
	})
	if err != nil {
		return nil, fmt.Errorf("signing failed: %v", err)
	}

	// Build result
	result := &ECDSA2PCResult{
		KeyGenResponses: []*ECDSA2PCKeyGenResponse{
			{KeyShare: keyShare0},
			{KeyShare: keyShare1},
		},
		SignResponses: []*ECDSA2PCSignResponse{
			{Signature: signOutputs[0].Opaque.([]byte)},
			{Signature: signOutputs[1].Opaque.([]byte)},
		},
	}

	return result, nil
}

// ECDSA2PCKeyGenWithMockNet is a test-only helper that runs the distributed key
// generation protocol locally using the in-memory mock network. This mirrors
// the original implementation that lived in the production API but has been
// moved here to avoid exposing testing utilities to API consumers.
func ECDSA2PCKeyGenWithMockNet(curveObj curve.Curve) ([]*ECDSA2PCKeyGenResponse, error) {
	if curveObj == nil {
		return nil, fmt.Errorf("curve must be provided")
	}

	// Coordinate two virtual parties using the mock network helper.
	runner := mocknet.NewMPCRunner(mocknet.GeneratePartyNames(2)...)

	outputs, err := runner.MPCRun2P(func(job cgobinding.Job2P, input *mocknet.MPCIO) (*mocknet.MPCIO, error) {
		cv := input.Opaque.(curve.Curve)
		keyShareRef, err := cgobinding.DistributedKeyGen(job, curve.Code(cv))
		if err != nil {
			return nil, fmt.Errorf("key generation failed: %v", err)
		}
		return &mocknet.MPCIO{Opaque: keyShareRef}, nil
	}, []*mocknet.MPCIO{
		{Opaque: curveObj},
		{Opaque: curveObj},
	})
	if err != nil {
		return nil, fmt.Errorf("key generation failed: %v", err)
	}

	// Convert outputs into the public response structure expected by callers.
	responses := []*ECDSA2PCKeyGenResponse{
		{KeyShare: ECDSA2PCKey(outputs[0].Opaque.(cgobinding.Mpc_ecdsa2pc_key_ref))},
		{KeyShare: ECDSA2PCKey(outputs[1].Opaque.(cgobinding.Mpc_ecdsa2pc_key_ref))},
	}

	return responses, nil
}

func TestECDSA2PCKeyGen_CurveIntegrity(t *testing.T) {
	// Ensure that the curve associated with each generated key share matches
	// the curve requested during key generation.
	secp, _ := curve.NewSecp256k1()

	keyGenResponses, err := ECDSA2PCKeyGenWithMockNet(secp)
	require.NoError(t, err)
	require.Len(t, keyGenResponses, 2)

	expectedCode := curve.Code(secp)

	for i, resp := range keyGenResponses {
		c, err := resp.KeyShare.Curve()
		require.NoError(t, err, "party %d Curve() should not error", i)
		assert.NotNil(t, c, "party %d curve should not be nil", i)
		assert.Equal(t, expectedCode, curve.Code(c), "party %d curve code should match", i)
		c.Free()
	}
}

func TestECDSA2PC_Refresh(t *testing.T) {
	// Step 0: initialise curve
	curveObj, _ := curve.NewSecp256k1()

	// Step 1: Generate initial key shares
	keyGenResponses, err := ECDSA2PCKeyGenWithMockNet(curveObj)
	require.NoError(t, err)
	require.Len(t, keyGenResponses, 2)

	// Capture original public key Q
	origQ, err := keyGenResponses[0].KeyShare.Q()
	require.NoError(t, err)
	defer origQ.Free()

	// Create a fresh mock network for the refresh round
	const nParties = 2
	messengers := mocknet.NewMockNetwork(nParties)
	partyNames := []string{"party_0", "party_1"}

	type refreshResult struct {
		resp *ECDSA2PCRefreshResponse
		err  error
	}

	results := make([]refreshResult, nParties)

	var wg sync.WaitGroup
	wg.Add(nParties)
	for i := 0; i < nParties; i++ {
		go func(i int) {
			defer wg.Done()
			jp, err := NewJob2P(messengers[i], i, partyNames)
			if err != nil {
				results[i] = refreshResult{resp: nil, err: err}
				return
			}
			defer jp.Free()

			r, e := ECDSA2PCRefresh(jp, &ECDSA2PCRefreshRequest{KeyShare: keyGenResponses[i].KeyShare})
			results[i] = refreshResult{resp: r, err: e}
		}(i)
	}
	wg.Wait()

	for i := 0; i < nParties; i++ {
		require.NoError(t, results[i].err, "party %d refresh should succeed", i)
		require.NotNil(t, results[i].resp, "party %d response should not be nil", i)
	}

	newShare0 := results[0].resp.NewKeyShare
	newShare1 := results[1].resp.NewKeyShare

	// ===== Curve unchanged =====
	expectedCode := curve.Code(curveObj)

	c0, err := newShare0.Curve()
	require.NoError(t, err)
	assert.Equal(t, expectedCode, curve.Code(c0), "party 0 curve code should remain unchanged")
	c0.Free()

	c1, err := newShare1.Curve()
	require.NoError(t, err)
	assert.Equal(t, expectedCode, curve.Code(c1), "party 1 curve code should remain unchanged")
	c1.Free()

	// ===== Public key Q unchanged =====
	newQ0, err := newShare0.Q()
	require.NoError(t, err)
	defer newQ0.Free()
	newQ1, err := newShare1.Q()
	require.NoError(t, err)
	defer newQ1.Free()

	assert.True(t, origQ.Equals(newQ0), "public key should remain unchanged after refresh (party 0)")
	assert.True(t, origQ.Equals(newQ1), "public key should remain unchanged after refresh (party 1)")

	// ===== Key share sum unchanged =====
	x0New, err := newShare0.XShare()
	require.NoError(t, err)
	x1New, err := newShare1.XShare()
	require.NoError(t, err)

	sumNew, err := curveObj.Add(x0New, x1New)
	require.NoError(t, err)

	GsumNew, err := curveObj.MultiplyGenerator(sumNew)
	require.NoError(t, err)
	defer GsumNew.Free()

	assert.True(t, GsumNew.Equals(origQ), "G * (x0' + x1') should equal original Q after refresh")
}
