package mpc

import (
	"fmt"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// AgreeRandomRequest represents the input parameters for agree random protocol
type AgreeRandomRequest struct {
	BitLen int // Number of bits for the random value
}

// AgreeRandomResponse represents the output of agree random protocol
type AgreeRandomResponse struct {
	RandomValue []byte // The agreed-upon random value
}

// AgreeRandom executes the agree random protocol between two parties.
// Both parties will agree on the same random value of the specified bit length.
func AgreeRandom(job2p *Job2P, req *AgreeRandomRequest) (*AgreeRandomResponse, error) {
	if req.BitLen <= 0 {
		return nil, fmt.Errorf("bit length must be positive, got %d", req.BitLen)
	}

	// Execute the agree random protocol using the provided Job2P
	randomValue, err := cgobinding.AgreeRandom(job2p.cgo(), req.BitLen)
	if err != nil {
		return nil, fmt.Errorf("agree random protocol failed: %v", err)
	}

	return &AgreeRandomResponse{
		RandomValue: randomValue,
	}, nil
}
