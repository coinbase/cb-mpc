package main

import (
	"bytes"
	b64 "encoding/base64"
	"fmt"
	"log"

	"github.com/coinbase/cb-mpc/cb-mpc-go/cblib"
	"github.com/coinbase/cb-mpc/cb-mpc-go/network"
	"github.com/coinbase/cb-mpc/demos/mocknet"
)

type SignMPInput struct {
	Key cblib.MPC_ECDSAMPC_KEY_PTR
	Msg []byte
}

func EcdsaMPKeygenWrapper(job network.JobSessionMP, input *mocknet.MPCIO) (*mocknet.MPCIO, error) {
	curveCode := input.Opaque.(int)
	keyshare, err := cblib.MPC_ecdsampc_dkg(job, curveCode)
	if err != nil {
		return nil, fmt.Errorf("calling ecdsa mp keygen: %v", err)
	}
	return &mocknet.MPCIO{Opaque: keyshare}, nil
}

func EcdsaMPSignWrapper(job network.JobSessionMP, input *mocknet.MPCIO) (*mocknet.MPCIO, error) {
	signInput := input.Opaque.(SignMPInput)
	msg := signInput.Msg
	defaultSigReceiver := 0
	sig, err := cblib.MPC_ecdsampc_sign(job, signInput.Key, msg, defaultSigReceiver)
	if err != nil {
		return nil, fmt.Errorf("calling ecdsa mp sign: %v", err)
	}
	return &mocknet.MPCIO{Opaque: sig}, nil
}

func main() {
	NID_secp256k1 := 714 // Hardcoded OpenSSl NID for secp256k1
	runner := mocknet.NewMPCRunner(4)
	// run ecdsa mpc dkg and get the additive key shares
	fmt.Println("\n## Running 4-party ecdsa mp keygen")
	outputs, err := runner.MPCRunMP(EcdsaMPKeygenWrapper, []*mocknet.MPCIO{{Opaque: NID_secp256k1}, {Opaque: NID_secp256k1}, {Opaque: NID_secp256k1}, {Opaque: NID_secp256k1}})
	if err != nil {
		log.Fatal(err)
	}
	x, y, err := cblib.MPC_ecdsa_mpc_public_key_to_string(outputs[0].Opaque.(cblib.MPC_ECDSAMPC_KEY_PTR))
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Public key:\n - x: %x\n - y: %x\n", x, y)
	// run PVE many to many to backup each of the key shares
	// Step 1: create the access structure for backing up the keys
	root := cblib.NewNode(cblib.NodeType_AND, "root", 0)
	root_child_1 := cblib.NewNode(cblib.NodeType_LEAF, "leaf1", 0)
	root_child_2 := cblib.NewNode(cblib.NodeType_THRESHOLD, "th", 2)
	root_child_2_child_1 := cblib.NewNode(cblib.NodeType_LEAF, "leaf2", 0)
	root_child_2_child_2 := cblib.NewNode(cblib.NodeType_LEAF, "leaf3", 0)
	root_child_2_child_3 := cblib.NewNode(cblib.NodeType_LEAF, "leaf4", 0)
	cblib.AddChild(root_child_2, root_child_2_child_1)
	cblib.AddChild(root_child_2, root_child_2_child_2)
	cblib.AddChild(root_child_2, root_child_2_child_3)
	cblib.AddChild(root, root_child_1)
	cblib.AddChild(root, root_child_2)

	// Step 2: create encryption keys that will be used to encrypt each of the subshares at the leaves.
	//         this demo uses ECIES keys but both ECIES and RSA are supported by the library
	leafCount := 4
	privKeys, pubKeys, err := cblib.NewEncKeyPairs(leafCount)
	if err != nil {
		log.Fatal(err)
	}

	// Step 3: choose a human readable label. This will be cryptographically bound to the backup data
	inputLabel := "demo-data"

	// Step 4: type conversion: the expected type of the "demo" apis is a serialized vec<bn_t> values
	dataCount := 4
	keyshares := make([]cblib.MPC_ECDSAMPC_KEY_PTR, 4)
	for i := 0; i < dataCount; i++ {
		keyshares[i] = outputs[i].Opaque.(cblib.MPC_ECDSAMPC_KEY_PTR)
	}
	xs, Xs, err := cblib.SerializeECDSAShares(keyshares)
	if err != nil {
		log.Fatal(err)
	}

	// Step 5: create a publicly verifiable backup the secret data using all the above
	pveBundle, err := cblib.PVE_quorum_encrypt(root, pubKeys, leafCount, xs, dataCount, inputLabel)
	if err != nil {
		log.Fatal(fmt.Errorf("PVE encrypt failed, %v", err))
	}

	// Step 6: IMPORTANT: the above PVE only backs up the private keyshare. But an ECDSA key struct has,
	//         a couple of other important "public" data that needs to be backed up separately including the
	//         public key, all the public shares, and party indices.
	//         Since this is just for demonstration purposes, we are only focusing on the private keyshare

	// run PVE many to many to restore each of the key shares
	decryptedShares, err := cblib.PVE_quorum_decrypt(root, privKeys, leafCount, pubKeys, leafCount, pveBundle, Xs, dataCount, inputLabel)
	if err != nil {
		log.Fatal(fmt.Errorf("PVE decrypt failed, %v", err))
	}
	// convert the data back to the proper type
	// As described in Step 6 above, this is not a complete backup solution. Therefore, we simply assert that the decrypted
	// values match the inputs.
	for i := 0; i < dataCount; i++ {
		if !bytes.Equal(decryptedShares[i], xs[i]) {
			log.Fatal(fmt.Errorf("decrypted value does not match the original value"))
		}
	}

	// run ecdsa mpc to sign a message
	msg := []byte("This is a message")
	inputs := []*mocknet.MPCIO{
		{Opaque: SignMPInput{
			Msg: msg,
			Key: outputs[0].Opaque.(cblib.MPC_ECDSAMPC_KEY_PTR),
		}},
		{Opaque: SignMPInput{
			Msg: msg,
			Key: outputs[1].Opaque.(cblib.MPC_ECDSAMPC_KEY_PTR),
		}},
		{Opaque: SignMPInput{
			Msg: msg,
			Key: outputs[2].Opaque.(cblib.MPC_ECDSAMPC_KEY_PTR),
		}},
		{Opaque: SignMPInput{
			Msg: msg,
			Key: outputs[3].Opaque.(cblib.MPC_ECDSAMPC_KEY_PTR),
		}},
	}
	outputs, err = runner.MPCRunMP(EcdsaMPSignWrapper, inputs)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("sig: %v\n", b64.StdEncoding.EncodeToString(outputs[0].Opaque.([]byte)))
}
