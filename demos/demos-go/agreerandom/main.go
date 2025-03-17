package main

import (
	"encoding/hex"
	"fmt"

	"github.com/coinbase/cb-mpc/cb-mpc-go/network"
	"github.com/coinbase/cb-mpc/demos/mocknet"
)

func AgreeRandomWrapper(job network.JobSession2P, input *mocknet.MPCIO) (*mocknet.MPCIO, error) {
	agreeRandomInput := input.Opaque.(AgreeRandomInput)
	out, err := network.AgreeRandom(job, agreeRandomInput.BitLen)
	if err != nil {
		return nil, fmt.Errorf("calling agree_random: %v", err)
	}
	fmt.Printf("%d: agreed on randomness %s\n", job.GetRoleIndex(), hex.EncodeToString(out))
	return &mocknet.MPCIO{Opaque: out}, nil
}

type AgreeRandomInput struct {
	BitLen int
}

func main() {
	runner := mocknet.NewMPCRunner(2)

	fmt.Println("\n## Running 2-party AgreeRandom 128 bits")
	runner.MPCRun2P(AgreeRandomWrapper, []*mocknet.MPCIO{
		{Opaque: AgreeRandomInput{BitLen: 128}},
		{Opaque: AgreeRandomInput{BitLen: 128}}})
	fmt.Println("\n## Running 2-party AgreeRandom 10 bits")
	runner.MPCRun2P(AgreeRandomWrapper, []*mocknet.MPCIO{
		{Opaque: AgreeRandomInput{BitLen: 10}},
		{Opaque: AgreeRandomInput{BitLen: 10}}})
}
