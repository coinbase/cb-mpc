package main

import (
	"context"
	"crypto/tls"
	"fmt"

	secp256k1 "github.com/decred/dcrd/dcrec/secp256k1/v4"
	secp256k1ecdsa "github.com/decred/dcrd/dcrec/secp256k1/v4/ecdsa"

	"github.com/coinbase/cb-mpc-go/pkg/mpc"
)

const (
	ECDSAMPACDKGStagePortOffset     = 0
	ECDSAMPACSignStagePortOffset    = 100
	ECDSAMPACRefreshStagePortOffset = 200
)

var defaultECDSAMPACPartyNames = []string{
	"urn:uuid:2f4b567e-7d58-52c5-9c74-0d0c8b6a3e10",
	"urn:uuid:7a1f5e4a-b8b4-5c67-a4cb-5777f284d64e",
	"urn:uuid:3f5d3b64-51d7-58d6-9d8d-3a7e6b1f4032",
}

// DefaultECDSAMPACPartyNames returns the stable UUID-form party identifiers
// used by the shipped ECDSA MP AC web demo.
func DefaultECDSAMPACPartyNames() []string {
	return append([]string(nil), defaultECDSAMPACPartyNames...)
}

func sequentialPartyIndices(count int) []int {
	indices := make([]int, count)
	for i := range indices {
		indices[i] = i
	}
	return indices
}

// ThresholdAccessStructure builds a threshold access structure over all parties.
func ThresholdAccessStructure(partyNames []string, threshold int) (*mpc.AccessStructure, error) {
	if len(partyNames) < 2 {
		return nil, fmt.Errorf("partyNames must contain at least 2 entries, got %d", len(partyNames))
	}
	if threshold <= 0 || threshold > len(partyNames) {
		return nil, fmt.Errorf("threshold %d out of range [1, %d]", threshold, len(partyNames))
	}

	nodes := make([]mpc.AccessStructureNode, 0, len(partyNames)+1)
	children := make([]int, len(partyNames))
	for i, partyName := range partyNames {
		nodes = append(nodes, mpc.AccessStructureNode{
			Type:     mpc.AccessNodeLeaf,
			LeafName: partyName,
		})
		children[i] = i
	}
	nodes = append(nodes, mpc.AccessStructureNode{
		Type:       mpc.AccessNodeThreshold,
		ThresholdK: threshold,
		Children:   children,
	})
	return &mpc.AccessStructure{
		Nodes: nodes,
		Root:  len(nodes) - 1,
	}, nil
}

// ECDSAMPACDKGResult captures the local outputs from AC DKG.
type ECDSAMPACDKGResult struct {
	ACKeyBlob           []byte
	SID                 []byte
	PublicKeyCompressed []byte
}

// ECDSAMPACRefreshResult captures the local outputs from AC refresh.
type ECDSAMPACRefreshResult struct {
	ACKeyBlob           []byte
	SID                 []byte
	PublicKeyCompressed []byte
}

// ECDSAMPACSignResult captures the local outputs from AC signing.
type ECDSAMPACSignResult struct {
	SignatureDER []byte
	IsReceiver   bool
}

func newReadyStageTransport(ctx context.Context, self int, partyAddrs []string, tlsConfig *tls.Config, peerIdentityIndices []int) (*MTLSTransport, error) {
	if self < 0 || self >= len(partyAddrs) {
		return nil, fmt.Errorf("self %d out of range [0, %d)", self, len(partyAddrs))
	}
	if peerIdentityIndices == nil {
		peerIdentityIndices = sequentialPartyIndices(len(partyAddrs))
	}
	if len(peerIdentityIndices) != len(partyAddrs) {
		return nil, fmt.Errorf("peer identity count %d must match partyAddrs count %d", len(peerIdentityIndices), len(partyAddrs))
	}
	peerNames, err := demoPeerNamesForIndices(peerIdentityIndices)
	if err != nil {
		return nil, err
	}
	transport, err := NewNamedMTLSTransport(self, partyAddrs[self], partyAddrs, peerNames, tlsConfig)
	if err != nil {
		return nil, err
	}
	if err := transport.WaitForPeers(ctx, OtherPartyIndices(self, len(partyAddrs))); err != nil {
		_ = transport.Close()
		return nil, err
	}
	return transport, nil
}

// RunECDSAMPACDKG runs the AC DKG stage for one party.
func RunECDSAMPACDKG(ctx context.Context, self int, partyNames []string, partyAddrs []string, tlsConfig *tls.Config, curve mpc.Curve, access *mpc.AccessStructure, quorumPartyNames []string) (*ECDSAMPACDKGResult, error) {
	transport, err := newReadyStageTransport(ctx, self, partyAddrs, tlsConfig, nil)
	if err != nil {
		return nil, fmt.Errorf("create DKG transport: %w", err)
	}
	defer func() { _ = transport.Close() }()

	job := mpc.JobMP{
		Self:       self,
		PartyNames: append([]string(nil), partyNames...),
		Transport:  transport,
	}
	acKeyBlob, sidOut, err := mpc.ECDSAMPDKGAC(ctx, job, curve, nil, access, quorumPartyNames)
	if err != nil {
		return nil, err
	}
	publicKeyCompressed, err := mpc.ECDSAMPGetPublicKeyCompressed(acKeyBlob)
	if err != nil {
		return nil, err
	}
	return &ECDSAMPACDKGResult{
		ACKeyBlob:           acKeyBlob,
		SID:                 sidOut,
		PublicKeyCompressed: publicKeyCompressed,
	}, nil
}

// RunECDSAMPACRefresh runs the AC refresh stage for one party.
func RunECDSAMPACRefresh(ctx context.Context, self int, partyNames []string, partyAddrs []string, tlsConfig *tls.Config, access *mpc.AccessStructure, quorumPartyNames []string, sidIn []byte, acKeyBlob []byte) (*ECDSAMPACRefreshResult, error) {
	transport, err := newReadyStageTransport(ctx, self, partyAddrs, tlsConfig, nil)
	if err != nil {
		return nil, fmt.Errorf("create refresh transport: %w", err)
	}
	defer func() { _ = transport.Close() }()

	job := mpc.JobMP{
		Self:       self,
		PartyNames: append([]string(nil), partyNames...),
		Transport:  transport,
	}
	sidOut, refreshedKeyBlob, err := mpc.ECDSAMPRefreshAC(ctx, job, sidIn, acKeyBlob, access, quorumPartyNames)
	if err != nil {
		return nil, err
	}
	publicKeyCompressed, err := mpc.ECDSAMPGetPublicKeyCompressed(refreshedKeyBlob)
	if err != nil {
		return nil, err
	}
	return &ECDSAMPACRefreshResult{
		ACKeyBlob:           refreshedKeyBlob,
		SID:                 sidOut,
		PublicKeyCompressed: publicKeyCompressed,
	}, nil
}

// RunECDSAMPACSign runs the AC sign stage for one quorum member.
func RunECDSAMPACSign(ctx context.Context, self int, quorumPartyNames []string, quorumPartyIndices []int, partyAddrs []string, tlsConfig *tls.Config, access *mpc.AccessStructure, acKeyBlob []byte, msgHash []byte, expectedPublicKey []byte, sigReceiver int) (*ECDSAMPACSignResult, error) {
	transport, err := newReadyStageTransport(ctx, self, partyAddrs, tlsConfig, quorumPartyIndices)
	if err != nil {
		return nil, fmt.Errorf("create sign transport: %w", err)
	}
	defer func() { _ = transport.Close() }()

	job := mpc.JobMP{
		Self:       self,
		PartyNames: append([]string(nil), quorumPartyNames...),
		Transport:  transport,
	}
	sigDER, err := mpc.ECDSAMPSignAC(ctx, job, acKeyBlob, access, msgHash, sigReceiver)
	if err != nil {
		return nil, err
	}
	if self == sigReceiver && len(expectedPublicKey) != 0 {
		if err := VerifySecp256k1ECDSASignature(expectedPublicKey, msgHash, sigDER); err != nil {
			return nil, err
		}
	}
	return &ECDSAMPACSignResult{
		SignatureDER: sigDER,
		IsReceiver:   self == sigReceiver,
	}, nil
}

// VerifySecp256k1ECDSASignature verifies a DER-encoded secp256k1 ECDSA
// signature against a compressed public key and message hash.
func VerifySecp256k1ECDSASignature(publicKeyCompressed []byte, msgHash []byte, sigDER []byte) error {
	publicKey, err := secp256k1.ParsePubKey(publicKeyCompressed)
	if err != nil {
		return fmt.Errorf("failed to parse secp256k1 public key: %w", err)
	}
	signature, err := secp256k1ecdsa.ParseDERSignature(sigDER)
	if err != nil {
		return fmt.Errorf("failed to parse ECDSA DER signature: %w", err)
	}
	if !signature.Verify(msgHash, publicKey) {
		return fmt.Errorf("ECDSA signature verification failed")
	}
	return nil
}
