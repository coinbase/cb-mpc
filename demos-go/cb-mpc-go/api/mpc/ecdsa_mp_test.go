package mpc

import (
	"fmt"
	"testing"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// SignMPInput represents the input for multi-party signing operations
type SignMPInput struct {
	Key ECDSAMPCKey
	Msg []byte
}

// ECDSAMPCWithMockNet performs complete N-party ECDSA workflow using MockNet.
// It is used in unit tests to exercise the full protocol stack without external
// networking.
func ECDSAMPCWithMockNet(nParties int, c curve.Curve, message []byte) ([]*ECDSAMPCKeyGenResponse, []*ECDSAMPCSignResponse, error) {
	if nParties < 3 {
		return nil, nil, fmt.Errorf("n-party ECDSA requires at least 3 parties")
	}
	if len(message) == 0 {
		return nil, nil, fmt.Errorf("message cannot be empty")
	}

	// Create MockNet runner
	runner := mocknet.NewMPCRunner(mocknet.GeneratePartyNames(nParties)...)

	// ---------------------------------------------------------------------
	// Step 1: Distributed Key Generation
	// ---------------------------------------------------------------------
	keyGenInputs := make([]*mocknet.MPCIO, nParties)
	for i := 0; i < nParties; i++ {
		keyGenInputs[i] = &mocknet.MPCIO{Opaque: c}
	}

	keyGenOutputs, err := runner.MPCRunMP(func(job cgobinding.JobMP, input *mocknet.MPCIO) (*mocknet.MPCIO, error) {
		cv := input.Opaque.(curve.Curve)
		keyShare, err := cgobinding.KeyShareDKGCode(job, curve.Code(cv))
		if err != nil {
			return nil, fmt.Errorf("n-party key generation failed: %v", err)
		}
		return &mocknet.MPCIO{Opaque: keyShare}, nil
	}, keyGenInputs)
	if err != nil {
		return nil, nil, fmt.Errorf("key generation failed: %v", err)
	}

	keyGenResponses := make([]*ECDSAMPCKeyGenResponse, nParties)
	for i := 0; i < nParties; i++ {
		keyGenResponses[i] = &ECDSAMPCKeyGenResponse{
			KeyShare: ECDSAMPCKey(keyGenOutputs[i].Opaque.(cgobinding.Mpc_eckey_mp_ref)),
		}
	}

	// ---------------------------------------------------------------------
	// Step 2: Distributed Signing (no explicit public key extraction needed)
	// ---------------------------------------------------------------------
	signatureReceiver := 0 // Party 0 receives the signature
	signInputs := make([]*mocknet.MPCIO, nParties)
	for i := 0; i < nParties; i++ {
		signInputs[i] = &mocknet.MPCIO{Opaque: SignMPInput{Key: keyGenResponses[i].KeyShare, Msg: message}}
	}

	signOutputs, err := runner.MPCRunMP(func(job cgobinding.JobMP, input *mocknet.MPCIO) (*mocknet.MPCIO, error) {
		signInput := input.Opaque.(SignMPInput)
		sig, err := cgobinding.MPC_ecdsampc_sign(job, signInput.Key.cgobindingRef(), signInput.Msg, signatureReceiver)
		if err != nil {
			return nil, fmt.Errorf("n-party signing failed: %v", err)
		}
		return &mocknet.MPCIO{Opaque: sig}, nil
	}, signInputs)
	if err != nil {
		return nil, nil, fmt.Errorf("signing failed: %v", err)
	}

	signResponses := make([]*ECDSAMPCSignResponse, nParties)
	for i := 0; i < nParties; i++ {
		var sigBytes []byte
		if i == signatureReceiver {
			sigBytes = signOutputs[i].Opaque.([]byte)
		}
		signResponses[i] = &ECDSAMPCSignResponse{Signature: sigBytes}
	}

	return keyGenResponses, signResponses, nil
}

func TestECDSAMPCWithMockNet(t *testing.T) {
	tests := []struct {
		name     string
		nParties int
		message  []byte
		wantErr  bool
	}{
		{
			name:     "valid_3_party_ecdsa",
			nParties: 3,
			message:  []byte("test message for 3-party ECDSA"),
			wantErr:  false,
		},
		{
			name:     "valid_4_party_ecdsa",
			nParties: 4,
			message:  []byte("test message for 4-party ECDSA"),
			wantErr:  false,
		},
		{
			name:     "valid_5_party_ecdsa",
			nParties: 5,
			message:  []byte("test message for 5-party ECDSA"),
			wantErr:  false,
		},
		{
			name:     "invalid_too_few_parties",
			nParties: 2,
			message:  []byte("test message"),
			wantErr:  true,
		},
		{
			name:     "invalid_empty_message",
			nParties: 3,
			message:  []byte{},
			wantErr:  true,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			// Use secp256k1 for all tests (matches previous NID 714)
			secp, errCurve := curve.NewSecp256k1()
			if errCurve != nil {
				t.Fatalf("failed to create curve: %v", errCurve)
			}

			keyGenResponses, signResponses, err := ECDSAMPCWithMockNet(tt.nParties, secp, tt.message)

			if tt.wantErr {
				if err == nil {
					t.Errorf("ECDSAMPCWithMockNet() expected error but got none")
				}
				return
			}

			if err != nil {
				t.Errorf("ECDSAMPCWithMockNet() error = %v, wantErr %v", err, tt.wantErr)
				return
			}

			// Verify key generation responses
			if len(keyGenResponses) != tt.nParties {
				t.Errorf("Expected %d key generation responses, got %d", tt.nParties, len(keyGenResponses))
			}

			// Verify signing responses
			if len(signResponses) != tt.nParties {
				t.Errorf("Expected %d signing responses, got %d", tt.nParties, len(signResponses))
			}

			// Verify that only party 0 receives the signature
			signatureReceiver := 0
			for i, resp := range signResponses {
				if i == signatureReceiver {
					if len(resp.Signature) == 0 {
						t.Errorf("Party %d should receive signature but got empty", i)
					}
				} else {
					if len(resp.Signature) != 0 {
						t.Errorf("Party %d should not receive signature but got %d bytes", i, len(resp.Signature))
					}
				}
			}

			// Verify public key is populated using get_Q
			pt, err := keyGenResponses[0].KeyShare.Q()
			if err != nil {
				t.Errorf("Failed to retrieve public key: %v", err)
			} else {
				if len(pt.GetX()) == 0 {
					t.Error("Public key X coordinate is empty")
				}
				if len(pt.GetY()) == 0 {
					t.Error("Public key Y coordinate is empty")
				}
				pt.Free()
			}
		})
	}
}

func TestECDSAMPCConsistency(t *testing.T) {
	// Test that multiple runs produce different signatures but same public key structure
	nParties := 3
	secp, errCurve := curve.NewSecp256k1()
	if errCurve != nil {
		t.Fatalf("failed to create curve: %v", errCurve)
	}

	// Run the protocol twice with different messages
	message1 := []byte("first test message")
	message2 := []byte("second test message")

	keyGenResponses1, signResponses1, err := ECDSAMPCWithMockNet(nParties, secp, message1)
	if err != nil {
		t.Fatalf("First run failed: %v", err)
	}

	keyGenResponses2, signResponses2, err := ECDSAMPCWithMockNet(nParties, secp, message2)
	if err != nil {
		t.Fatalf("Second run failed: %v", err)
	}

	// Public keys should have the same structure (both should be non-empty) using get_Q
	pt1, err := keyGenResponses1[0].KeyShare.Q()
	if err != nil {
		t.Fatalf("Failed to retrieve public key from first run: %v", err)
	}
	pt2, err := keyGenResponses2[0].KeyShare.Q()
	if err != nil {
		t.Fatalf("Failed to retrieve public key from second run: %v", err)
	}
	if len(pt1.GetX()) == 0 || len(pt1.GetY()) == 0 {
		t.Error("First public key has empty coordinates")
	}
	if len(pt2.GetX()) == 0 || len(pt2.GetY()) == 0 {
		t.Error("Second public key has empty coordinates")
	}
	pt1.Free()
	pt2.Free()

	// Key generation responses should have the same structure
	if len(keyGenResponses1) != len(keyGenResponses2) {
		t.Error("Key generation response counts differ")
	}

	// Signature responses should have the same structure
	if len(signResponses1) != len(signResponses2) {
		t.Error("Signature response counts differ")
	}

	// Signatures should be different (different messages)
	sig1 := signResponses1[0].Signature
	sig2 := signResponses2[0].Signature

	if len(sig1) == 0 || len(sig2) == 0 {
		t.Error("Signatures should not be empty")
	}

	// Compare signatures byte by byte - they should be different
	if len(sig1) == len(sig2) {
		allSame := true
		for i := 0; i < len(sig1); i++ {
			if sig1[i] != sig2[i] {
				allSame = false
				break
			}
		}
		if allSame {
			t.Error("Signatures should be different for different messages")
		}
	}
}

func TestECDSAMPCScalability(t *testing.T) {
	// Test with different party counts to ensure scalability
	partyCounts := []int{3, 4, 5, 6}
	secp, errCurve := curve.NewSecp256k1()
	if errCurve != nil {
		t.Fatalf("failed to create curve: %v", errCurve)
	}
	message := []byte("scalability test message")

	for _, nParties := range partyCounts {
		t.Run(fmt.Sprintf("parties_%d", nParties), func(t *testing.T) {
			keyGenResponses, signResponses, err := ECDSAMPCWithMockNet(nParties, secp, message)
			if err != nil {
				t.Errorf("Failed with %d parties: %v", nParties, err)
				return
			}

			if len(keyGenResponses) != nParties {
				t.Errorf("Expected %d key shares, got %d", nParties, len(keyGenResponses))
			}

			if len(signResponses) != nParties {
				t.Errorf("Expected %d sign responses, got %d", nParties, len(signResponses))
			}

			// Verify public key is retrievable using get_Q
			pt, err := keyGenResponses[0].KeyShare.Q()
			if err != nil {
				t.Errorf("Failed to retrieve public key: %v", err)
			} else {
				pt.Free()
			}

			// Verify signature was generated
			if len(signResponses[0].Signature) == 0 {
				t.Error("Signature was not generated")
			}
		})
	}
}

func TestECDSAMPCKeyAccessors_Q_and_Curve(t *testing.T) {
	// Use secp256k1 curve and 3-party setup
	secp, errCurve := curve.NewSecp256k1()
	if errCurve != nil {
		t.Fatalf("failed to create curve: %v", errCurve)
	}
	defer secp.Free()

	msg := []byte("accessors test message")
	keyGenResponses, _, err := ECDSAMPCWithMockNet(3, secp, msg)
	if err != nil {
		t.Fatalf("ECDSAMPCWithMockNet failed: %v", err)
	}
	if len(keyGenResponses) == 0 {
		t.Fatalf("no key generation responses returned")
	}

	// Verify Q() returns a valid, non-empty public key point
	pt, err := keyGenResponses[0].KeyShare.Q()
	if err != nil {
		t.Fatalf("KeyShare.Q failed: %v", err)
	}
	if len(pt.GetX()) == 0 || len(pt.GetY()) == 0 {
		t.Fatalf("public key coordinates should be non-empty")
	}
	pt.Free()

	// Verify Curve() returns the same curve that was used for key generation
	kc, err := keyGenResponses[0].KeyShare.Curve()
	if err != nil {
		t.Fatalf("KeyShare.Curve failed: %v", err)
	}
	defer kc.Free()

	if curve.Code(kc) != curve.Code(secp) {
		t.Fatalf("curve code mismatch: got %d want %d", curve.Code(kc), curve.Code(secp))
	}
	if kc.String() != secp.String() {
		t.Fatalf("curve string mismatch: got %q want %q", kc.String(), secp.String())
	}
}
