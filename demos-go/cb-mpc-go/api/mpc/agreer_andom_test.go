package mpc

import (
	"fmt"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"
)

// AgreeRandomWithMockNet is a test-only convenience wrapper that spins up two parties
// connected via the in-memory mock network and runs the AgreeRandom protocol.
// This helper lives in *_test.go files so it is not exposed to library users.
func AgreeRandomWithMockNet(nParties int, bitLen int) ([]*AgreeRandomResponse, error) {
	if nParties != 2 {
		return nil, fmt.Errorf("agree random currently only supports 2 parties, got %d", nParties)
	}

	if bitLen <= 0 {
		return nil, fmt.Errorf("bit length must be positive, got %d", bitLen)
	}

	// Create mock network messengers
	messengers := mocknet.NewMockNetwork(nParties)

	partyNames := []string{"party_0", "party_1"}

	responses := make([]*AgreeRandomResponse, nParties)
	errChan := make(chan error, nParties)
	respChan := make(chan struct {
		index int
		resp  *AgreeRandomResponse
	}, nParties)

	for i := 0; i < nParties; i++ {
		go func(partyIndex int) {
			j, err := NewJob2P(messengers[partyIndex], partyIndex, partyNames)
			if err != nil {
				errChan <- fmt.Errorf("party %d failed to create Job2P: %v", partyIndex, err)
				return
			}
			defer j.Free()

			req := &AgreeRandomRequest{BitLen: bitLen}
			resp, err := AgreeRandom(j, req)
			if err != nil {
				errChan <- fmt.Errorf("party %d failed: %v", partyIndex, err)
				return
			}

			respChan <- struct {
				index int
				resp  *AgreeRandomResponse
			}{partyIndex, resp}
		}(i)
	}

	for i := 0; i < nParties; i++ {
		select {
		case err := <-errChan:
			return nil, err
		case result := <-respChan:
			responses[result.index] = result.resp
		}
	}

	return responses, nil
}

func TestAgreeRandomWithMockNet(t *testing.T) {
	tests := []struct {
		name     string
		nParties int
		bitLen   int
		wantErr  bool
	}{
		{
			name:     "valid 2-party 128-bit",
			nParties: 2,
			bitLen:   128,
			wantErr:  false,
		},
		{
			name:     "valid 2-party 10-bit",
			nParties: 2,
			bitLen:   10,
			wantErr:  false,
		},
		{
			name:     "valid 2-party 256-bit",
			nParties: 2,
			bitLen:   256,
			wantErr:  false,
		},
		{
			name:     "invalid party count",
			nParties: 3,
			bitLen:   128,
			wantErr:  true,
		},
		{
			name:     "invalid bit length zero",
			nParties: 2,
			bitLen:   0,
			wantErr:  true,
		},
		{
			name:     "invalid bit length negative",
			nParties: 2,
			bitLen:   -10,
			wantErr:  true,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			responses, err := AgreeRandomWithMockNet(tt.nParties, tt.bitLen)

			if tt.wantErr {
				assert.Error(t, err)
				assert.Nil(t, responses)
				return
			}

			require.NoError(t, err)
			require.Len(t, responses, tt.nParties)

			// Verify all parties got the same random value
			firstValue := responses[0].RandomValue
			for i, resp := range responses {
				assert.NotNil(t, resp, "response %d should not be nil", i)
				assert.NotNil(t, resp.RandomValue, "random value %d should not be nil", i)
				assert.Equal(t, firstValue, resp.RandomValue,
					"party %d should have same random value as party 0", i)
			}

			// Verify the random value has the expected length
			expectedBytes := (tt.bitLen + 7) / 8 // Round up to nearest byte
			assert.Len(t, firstValue, expectedBytes,
				"random value should have %d bytes for %d bits", expectedBytes, tt.bitLen)
		})
	}
}

func TestAgreeRandomRequest_Validation(t *testing.T) {
	tests := []struct {
		name    string
		bitLen  int
		wantErr bool
	}{
		{"valid small", 1, false},
		{"valid medium", 128, false},
		{"valid large", 2048, false},
		{"invalid zero", 0, true},
		{"invalid negative", -1, true},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			req := &AgreeRandomRequest{BitLen: tt.bitLen}

			// We can't easily test AgreeRandom without a real messenger,
			// so we test through AgreeRandomWithMockNet which does validation
			_, err := AgreeRandomWithMockNet(2, req.BitLen)

			if tt.wantErr {
				assert.Error(t, err)
			} else {
				assert.NoError(t, err)
			}
		})
	}
}

func TestAgreeRandomResponse_Structure(t *testing.T) {
	// Test that the response structure is as expected
	responses, err := AgreeRandomWithMockNet(2, 64)
	require.NoError(t, err)
	require.Len(t, responses, 2)

	for i, resp := range responses {
		assert.NotNil(t, resp, "response %d should not be nil", i)
		assert.NotNil(t, resp.RandomValue, "random value %d should not be nil", i)
		assert.Len(t, resp.RandomValue, 8, "64-bit value should be 8 bytes")
	}
}

func TestAgreeRandom_DeterministicAgreement(t *testing.T) {
	// Run the same configuration multiple times to ensure consistency
	bitLen := 32

	for i := 0; i < 5; i++ {
		responses, err := AgreeRandomWithMockNet(2, bitLen)
		require.NoError(t, err, "iteration %d should succeed", i)
		require.Len(t, responses, 2)

		// Both parties should agree each time
		assert.Equal(t, responses[0].RandomValue, responses[1].RandomValue,
			"iteration %d: parties should agree", i)

		// Values should be the correct length
		expectedBytes := (bitLen + 7) / 8
		assert.Len(t, responses[0].RandomValue, expectedBytes,
			"iteration %d: wrong length", i)
	}
}
