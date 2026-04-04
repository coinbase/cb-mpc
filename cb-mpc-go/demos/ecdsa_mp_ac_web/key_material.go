package main

import (
	"encoding/hex"
	"fmt"

	secp256k1 "github.com/decred/dcrd/dcrec/secp256k1/v4"

	"github.com/coinbase/cb-mpc-go/pkg/mpc"
)

type keyMaterial struct {
	JointPublicKey             []byte
	JointPublicKeyUncompressed []byte
	PublicShare                []byte
	PrivateScalar              []byte
}

func extractKeyMaterial(keyBlob []byte) (*keyMaterial, error) {
	if len(keyBlob) == 0 {
		return nil, nil
	}
	jointPublicKey, err := mpc.ECDSAMPGetPublicKeyCompressed(keyBlob)
	if err != nil {
		return nil, fmt.Errorf("get joint public key: %w", err)
	}
	jointPublicKeyUncompressed, err := expandSecp256k1PublicKey(jointPublicKey)
	if err != nil {
		return nil, fmt.Errorf("expand joint public key: %w", err)
	}
	publicShare, err := mpc.ECDSAMPGetPublicShareCompressed(keyBlob)
	if err != nil {
		return nil, fmt.Errorf("get public share: %w", err)
	}
	_, privateScalar, err := mpc.ECDSAMPDetachPrivateScalar(keyBlob)
	if err != nil {
		return nil, fmt.Errorf("detach private scalar: %w", err)
	}
	return &keyMaterial{
		JointPublicKey:             append([]byte(nil), jointPublicKey...),
		JointPublicKeyUncompressed: append([]byte(nil), jointPublicKeyUncompressed...),
		PublicShare:                append([]byte(nil), publicShare...),
		PrivateScalar:              append([]byte(nil), privateScalar...),
	}, nil
}

func expandSecp256k1PublicKey(publicKeyCompressed []byte) ([]byte, error) {
	if len(publicKeyCompressed) == 0 {
		return nil, nil
	}
	publicKey, err := secp256k1.ParsePubKey(publicKeyCompressed)
	if err != nil {
		return nil, err
	}
	return publicKey.SerializeUncompressed(), nil
}

func fullHex(data []byte) string {
	if len(data) == 0 {
		return ""
	}
	return hex.EncodeToString(data)
}
