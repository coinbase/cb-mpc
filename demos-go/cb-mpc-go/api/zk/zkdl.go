package zk

import (
	"fmt"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// ZKUCDLProveRequest represents the input parameters for generating a
// zero-knowledge discrete logarithm proof.
//
//   - PublicKey is the point Q = w·G. It MUST be non-nil.
//   - Witness   is the scalar w. It MUST be non-nil.
//   - SessionID is an application supplied domain-separator that
//     distinguishes distinct proofs created with the same key material. It
//     MAY be nil/empty.
//   - Auxiliary is user defined additional data that is bound to the proof
//     (e.g. a transcript hash).
//
// The request mirrors the native C++ CB-MPC interface but uses the Go
// curve package types so that call-sites cannot accidentally confuse point
// and scalar byte slices.
type ZKUCDLProveRequest struct {
	PublicKey *curve.Point
	Witness   *curve.Scalar
	SessionID []byte
	Auxiliary uint64
}

// ZKUCDLProveResponse is returned by ZKUCDLProve.
//
// Proof holds an opaque, serialised representation of the zero-knowledge
// proof. Until the cgobinding for the real protocol is available the proof is
// a simple, deterministic mock value.
type ZKUCDLProveResponse struct {
	Proof []byte
}

// ZKUCDLProve is the Go wrapper around the native CB-MPC ZK DL prover.
// It delegates the heavy lifting to the C++ implementation exposed through
// the cgobinding package.
func ZKUCDLProve(req *ZKUCDLProveRequest) (*ZKUCDLProveResponse, error) {
	if req == nil {
		return nil, fmt.Errorf("nil request")
	}
	if req.PublicKey == nil {
		return nil, fmt.Errorf("public key cannot be nil")
	}
	if req.Witness == nil {
		return nil, fmt.Errorf("witness cannot be nil")
	}
	if req.SessionID == nil {
		req.SessionID = []byte{}
	}

	// Convert point bytes to native point ref for the binding
	pRef, err := cgobinding.ECCPointFromBytes(req.PublicKey.Bytes())
	if err != nil {
		return nil, fmt.Errorf("invalid public key: %v", err)
	}
	defer pRef.Free()
	proof, err := cgobinding.ZK_DL_Prove(pRef, req.Witness.Bytes, req.SessionID, req.Auxiliary)
	if err != nil {
		return nil, err
	}

	return &ZKUCDLProveResponse{Proof: proof}, nil
}

// ZKUCDLVerifyRequest represents the verifier input.
// The PublicKey, Proof, SessionID and Auxiliary fields must match the values
// used at prove time.
type ZKUCDLVerifyRequest struct {
	PublicKey *curve.Point
	Proof     []byte
	SessionID []byte
	Auxiliary uint64
}

// ZKUCDLVerifyResponse indicates whether the proof could be validated.
type ZKUCDLVerifyResponse struct {
	Valid bool
}

// ZKUCDLVerify validates a proof produced by ZKUCDLProve using the native
// implementation.
func ZKUCDLVerify(req *ZKUCDLVerifyRequest) (*ZKUCDLVerifyResponse, error) {
	if req == nil {
		return nil, fmt.Errorf("nil request")
	}
	if req.PublicKey == nil {
		return nil, fmt.Errorf("public key cannot be nil")
	}
	if len(req.Proof) == 0 {
		return nil, fmt.Errorf("proof cannot be empty")
	}
	if req.SessionID == nil {
		req.SessionID = []byte{}
	}

	pRef, err := cgobinding.ECCPointFromBytes(req.PublicKey.Bytes())
	if err != nil {
		return nil, fmt.Errorf("invalid public key: %v", err)
	}
	defer pRef.Free()
	valid, err := cgobinding.ZK_DL_Verify(pRef, req.Proof, req.SessionID, req.Auxiliary)
	if err != nil {
		return nil, err
	}
	return &ZKUCDLVerifyResponse{Valid: valid}, nil
}
