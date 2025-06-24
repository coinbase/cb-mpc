package main

import (
	"encoding/hex"
	"fmt"
	"log"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/mpc"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"
)

// runAgreeRandomDemo runs the AgreeRandom protocol for the given bit length and
// prints the agreed-upon value for both parties.
func runAgreeRandomDemo(bitLen int) error {
	const nParties = 2

	// Create an in-memory mock network.
	messengers := mocknet.NewMockNetwork(nParties)

	partyNames := []string{"party_0", "party_1"}

	responses := make([]*mpc.AgreeRandomResponse, nParties)
	errChan := make(chan error, nParties)
	respChan := make(chan struct {
		idx  int
		resp *mpc.AgreeRandomResponse
	}, nParties)

	for i := 0; i < nParties; i++ {
		go func(partyIdx int) {
			// Construct Job2P for this party.
			jp, err := mpc.NewJob2P(messengers[partyIdx], partyIdx, partyNames)
			if err != nil {
				errChan <- err
				return
			}
			defer jp.Free()

			req := &mpc.AgreeRandomRequest{BitLen: bitLen}
			resp, err := mpc.AgreeRandom(jp, req)
			if err != nil {
				errChan <- err
				return
			}

			respChan <- struct {
				idx  int
				resp *mpc.AgreeRandomResponse
			}{partyIdx, resp}
		}(i)
	}

	// Collect results.
	for i := 0; i < nParties; i++ {
		select {
		case err := <-errChan:
			return err
		case r := <-respChan:
			responses[r.idx] = r.resp
		}
	}

	// Verify both parties agreed.
	agreedHex := hex.EncodeToString(responses[0].RandomValue)
	fmt.Printf("Party 0: agreed on randomness %s\n", agreedHex)
	fmt.Printf("Party 1: agreed on randomness %s\n", hex.EncodeToString(responses[1].RandomValue))
	if agreedHex == hex.EncodeToString(responses[1].RandomValue) {
		fmt.Println("✅ Both parties agreed on the same random value!")
	} else {
		fmt.Println("❌ Parties got different random values!")
	}

	return nil
}

func main() {
	fmt.Println("\n=== CB-MPC Agree Random Example ===")

	fmt.Println("## Running 2-party AgreeRandom (128 bits)")
	if err := runAgreeRandomDemo(128); err != nil {
		log.Fatalf("AgreeRandom 128-bit failed: %v", err)
	}

	fmt.Println()

	fmt.Println("## Running 2-party AgreeRandom (10 bits)")
	if err := runAgreeRandomDemo(10); err != nil {
		log.Fatalf("AgreeRandom 10-bit failed: %v", err)
	}

	fmt.Println("\nAgreeRandom example completed successfully!")
}
