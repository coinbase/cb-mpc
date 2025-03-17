package main

import (
	b64 "encoding/base64"
	"fmt"
	"log"

	"github.com/coinbase/cb-mpc/cb-mpc-go/cblib"
	"github.com/coinbase/cb-mpc/cb-mpc-go/network"
	"github.com/coinbase/cb-mpc/demos/mocknet"
)

type SignInput struct {
	SessionID []byte
	Key       cblib.MPC_ECDSA2PC_KEY_PTR
	Msg       []byte
}

func Ecdsa2PKeygenWrapper(job network.JobSession2P, input *mocknet.MPCIO) (*mocknet.MPCIO, error) {
	curveCode := input.Opaque.(int)
	keyshare, err := cblib.DistributedKeyGen(job, curveCode)
	if err != nil {
		return nil, fmt.Errorf("calling ecdsa 2p keygen: %v", err)
	}
	return &mocknet.MPCIO{Opaque: keyshare}, nil
}

func Ecdsa2PSignWrapper(job network.JobSession2P, input *mocknet.MPCIO) (*mocknet.MPCIO, error) {
	signInput := input.Opaque.(SignInput)
	msgs := make([][]byte, 1)
	msgs[0] = signInput.Msg
	sigs, err := cblib.Sign(job, signInput.SessionID, signInput.Key, msgs)
	if err != nil {
		return nil, fmt.Errorf("calling ecdsa 2p sign: %v", err)
	}
	return &mocknet.MPCIO{Opaque: sigs[0]}, nil
}

func main() {
	NID_secp256k1 := 714 // Hardcoded OpenSSl NID for secp256k1
	runner := mocknet.NewMPCRunner(2)
	fmt.Println("\n## Running 2-party ecdsa 2p keygen")
	outputs, err := runner.MPCRun2P(Ecdsa2PKeygenWrapper, []*mocknet.MPCIO{{Opaque: NID_secp256k1}, {Opaque: NID_secp256k1}})
	if err != nil {
		log.Fatal(err)
	}

	sid := []byte("sid")
	msg := []byte("This is a message")
	inputs := []*mocknet.MPCIO{
		{Opaque: SignInput{
			SessionID: sid,
			Msg:       msg,
			Key:       outputs[0].Opaque.(cblib.MPC_ECDSA2PC_KEY_PTR),
		}},
		{Opaque: SignInput{
			SessionID: sid,
			Msg:       msg,
			Key:       outputs[1].Opaque.(cblib.MPC_ECDSA2PC_KEY_PTR),
		}},
	}
	outputs, err = runner.MPCRun2P(Ecdsa2PSignWrapper, inputs)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("sig 0: %v\n", b64.StdEncoding.EncodeToString(outputs[0].Opaque.([]byte)))
	fmt.Printf("sig 1: %v\n", b64.StdEncoding.EncodeToString(outputs[1].Opaque.([]byte)))
}
