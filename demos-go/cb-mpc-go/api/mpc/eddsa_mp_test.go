package mpc

import (
	"bytes"
	"fmt"
	"testing"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// EDDSAMPCWithMockNet executes the full EdDSA N-party workflow using the in-memory
// mock network. It is intentionally lightweight compared to the exhaustive
// ECDSA test-suite – its goal is to ensure the basic API surface compiles and
// the protocol can run end-to-end.
func EDDSAMPCWithMockNet(nParties int, cv curve.Curve, message []byte) ([]*EDDSAMPCKeyGenResponse, []*EDDSAMPCSignResponse, error) {
	if nParties < 3 {
		return nil, nil, fmt.Errorf("EdDSA N-party requires at least 3 parties")
	}
	if len(message) == 0 {
		return nil, nil, fmt.Errorf("message cannot be empty")
	}

	runner := mocknet.NewMPCRunner(mocknet.GeneratePartyNames(nParties)...)

	// ---------------- KeyGen ----------------
	keyGenInputs := make([]*mocknet.MPCIO, nParties)
	for i := 0; i < nParties; i++ {
		keyGenInputs[i] = &mocknet.MPCIO{Opaque: cv}
	}
	keyGenOutputs, err := runner.MPCRunMP(func(job cgobinding.JobMP, input *mocknet.MPCIO) (*mocknet.MPCIO, error) {
		curveObj := input.Opaque.(curve.Curve)
		apiJob := &JobMP{inner: job}
		resp, err := EDDSAMPCKeyGen(apiJob, &EDDSAMPCKeyGenRequest{Curve: curveObj})
		if err != nil {
			return nil, err
		}
		return &mocknet.MPCIO{Opaque: resp.KeyShare}, nil
	}, keyGenInputs)
	if err != nil {
		return nil, nil, err
	}

	keyShares := make([]EDDSAMPCKey, nParties)
	keyGenResponses := make([]*EDDSAMPCKeyGenResponse, nParties)
	for i := 0; i < nParties; i++ {
		keyShares[i] = keyGenOutputs[i].Opaque.(EDDSAMPCKey)
		keyGenResponses[i] = &EDDSAMPCKeyGenResponse{KeyShare: keyShares[i]}
	}

	// ---------------- Sign ----------------
	signInputs := make([]*mocknet.MPCIO, nParties)
	for i := 0; i < nParties; i++ {
		signInputs[i] = &mocknet.MPCIO{Opaque: struct {
			Key EDDSAMPCKey
			Msg []byte
		}{Key: keyShares[i], Msg: message}}
	}

	const sigReceiver = 0

	signOutputs, err := runner.MPCRunMP(func(job cgobinding.JobMP, input *mocknet.MPCIO) (*mocknet.MPCIO, error) {
		data := input.Opaque.(struct {
			Key EDDSAMPCKey
			Msg []byte
		})
		apiJob := &JobMP{inner: job}
		resp, err := EDDSAMPCSign(apiJob, &EDDSAMPCSignRequest{
			KeyShare:          data.Key,
			Message:           data.Msg,
			SignatureReceiver: sigReceiver,
		})
		if err != nil {
			return nil, err
		}
		return &mocknet.MPCIO{Opaque: resp.Signature}, nil
	}, signInputs)
	if err != nil {
		return nil, nil, err
	}

	signResponses := make([]*EDDSAMPCSignResponse, nParties)
	for i := 0; i < nParties; i++ {
		var sigBytes []byte
		if i == sigReceiver {
			sigBytes = signOutputs[i].Opaque.([]byte)
		}
		signResponses[i] = &EDDSAMPCSignResponse{Signature: sigBytes}
	}

	return keyGenResponses, signResponses, nil
}

func TestEDDSAMPC_EndToEnd(t *testing.T) {
	ed, err := curve.NewEd25519()
	if err != nil {
		t.Fatalf("failed to init curve: %v", err)
	}

	const nParties = 3
	message := []byte("hello eddsa")

	keyRes, signRes, err := EDDSAMPCWithMockNet(nParties, ed, message)
	if err != nil {
		t.Fatalf("protocol failed: %v", err)
	}

	if len(keyRes) != nParties || len(signRes) != nParties {
		t.Fatalf("unexpected response sizes")
	}

	if len(signRes[0].Signature) == 0 {
		t.Fatalf("signature receiver did not obtain signature")
	}
	// Non-receiver parties should have empty signatures
	for i := 1; i < nParties; i++ {
		if len(signRes[i].Signature) != 0 {
			t.Fatalf("party %d unexpectedly received signature bytes", i)
		}
	}

	// Validate EDDSAMPCKey.Curve() and EDDSAMPCKey.Q() accessors on resulting key shares
	expectedCode := curve.Code(ed)
	var q0 []byte
	for i := 0; i < nParties; i++ {
		c, err := keyRes[i].KeyShare.Curve()
		if err != nil {
			t.Fatalf("Curve() failed for party %d: %v", i, err)
		}
		if got := curve.Code(c); got != expectedCode {
			t.Fatalf("Curve() returned unexpected code for party %d: got %d want %d", i, got, expectedCode)
		}

		q, err := keyRes[i].KeyShare.Q()
		if err != nil {
			t.Fatalf("Q() failed for party %d: %v", i, err)
		}
		qBytes := q.Bytes()
		if len(qBytes) == 0 {
			t.Fatalf("Q() returned empty point for party %d", i)
		}
		if q.IsZero() {
			t.Fatalf("Q() returned zero point for party %d", i)
		}
		if i == 0 {
			q0 = qBytes
		} else if !bytes.Equal(qBytes, q0) {
			t.Fatalf("Q() mismatch across parties: party %d differs", i)
		}
		q.Free()
	}

	// Negative checks: zero-value key should surface errors from Curve() and Q()
	var zeroKey EDDSAMPCKey
	if _, err := zeroKey.Curve(); err == nil {
		t.Fatalf("expected Curve() to fail on zero-value key")
	}
	if _, err := zeroKey.Q(); err == nil {
		t.Fatalf("expected Q() to fail on zero-value key")
	}
}
