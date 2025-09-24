package mpc

import (
	"bytes"
	"encoding"
	"encoding/gob"
	"fmt"
	"runtime"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// Compile-time assertions to ensure EDDSAMPCKey implements the binary marshaling
// interfaces.
var _ encoding.BinaryMarshaler = (*EDDSAMPCKey)(nil)
var _ encoding.BinaryUnmarshaler = (*EDDSAMPCKey)(nil)

// ============================================================================
// Type definitions
// ============================================================================

// EDDSAMPCKey represents an opaque N-party EdDSA key share owned by the current
// party. Internally it wraps cgobinding.Mpc_eckey_mp_ref – the underlying representation
// is shared with other N-party key types.
//
// NOTE: The zero value is invalid.
//
// All methods are intentionally kept analogous to the existing multi-party key APIs to provide a
// consistent developer experience.
//
// ----------------------------------------------------------------------------

type EDDSAMPCKey cgobinding.Mpc_eckey_mp_ref

func newEDDSAMPCKey(ref cgobinding.Mpc_eckey_mp_ref) EDDSAMPCKey {
	return EDDSAMPCKey(ref)
}

// Free releases the underlying native resources.
func (k *EDDSAMPCKey) Free() {
	if k == nil {
		return
	}
	ref := cgobinding.Mpc_eckey_mp_ref(*k)
	(&ref).Free()
	*k = EDDSAMPCKey(cgobinding.Mpc_eckey_mp_ref{})
	runtime.SetFinalizer(k, nil)
}

func (k EDDSAMPCKey) cgobindingRef() cgobinding.Mpc_eckey_mp_ref {
	return cgobinding.Mpc_eckey_mp_ref(k)
}

// MarshalBinary serialises the key share into a portable wire format.
func (k EDDSAMPCKey) MarshalBinary() ([]byte, error) {
	parts, err := cgobinding.SerializeKeyShare(k.cgobindingRef())
	if err != nil {
		return nil, err
	}
	var buf bytes.Buffer
	if err := gob.NewEncoder(&buf).Encode(parts); err != nil {
		return nil, err
	}
	return buf.Bytes(), nil
}

// UnmarshalBinary restores a key share previously produced by MarshalBinary.
func (k *EDDSAMPCKey) UnmarshalBinary(data []byte) error {
	var parts [][]byte
	if err := gob.NewDecoder(bytes.NewReader(data)).Decode(&parts); err != nil {
		return err
	}
	ref, err := cgobinding.DeserializeKeyShare(parts)
	if err != nil {
		return err
	}
	*k = newEDDSAMPCKey(ref)
	return nil
}

// Accessors ---------------------------------------------------------------------------------

func (k EDDSAMPCKey) PartyName() (string, error) {
	return cgobinding.MPC_mpc_eckey_mp_get_party_name(k.cgobindingRef())
}

func (k EDDSAMPCKey) XShare() (*curve.Scalar, error) {
	bytes, err := cgobinding.MPC_mpc_eckey_mp_get_x_share(k.cgobindingRef())
	if err != nil {
		return nil, err
	}
	return &curve.Scalar{Bytes: bytes}, nil
}

func (k EDDSAMPCKey) Q() (*curve.Point, error) {
	bytes, err := cgobinding.KeyShareQBytes(k.cgobindingRef())
	if err != nil {
		return nil, err
	}
	return curve.NewPointFromBytes(bytes)
}

func (k EDDSAMPCKey) Curve() (curve.Curve, error) {
	code, err := cgobinding.KeyShareCurveCode(k.cgobindingRef())
	if err != nil {
		return nil, err
	}
	return curve.NewFromCode(code)
}

func (k EDDSAMPCKey) Qis() (map[string]*curve.Point, error) {
	names, points, err := cgobinding.MPC_mpc_eckey_mp_Qis(k.cgobindingRef())
	if err != nil {
		return nil, err
	}
	if len(names) != len(points) {
		return nil, fmt.Errorf("inconsistent Qis arrays: %d names vs %d points", len(names), len(points))
	}
	out := make(map[string]*curve.Point, len(names))
	for i, nameBytes := range names {
		pt, err := curve.NewPointFromBytes(points[i])
		if err != nil {
			return nil, fmt.Errorf("failed to decode Qi for party %s: %v", string(nameBytes), err)
		}
		out[string(nameBytes)] = pt
	}
	return out, nil
}

// ============================================================================
// Request / Response structs
// ============================================================================

type EDDSAMPCKeyGenRequest struct {
	Curve curve.Curve
}

type EDDSAMPCKeyGenResponse struct {
	KeyShare EDDSAMPCKey
}

type EDDSAMPCSignRequest struct {
	KeyShare          EDDSAMPCKey
	Message           []byte
	SignatureReceiver int
}

type EDDSAMPCSignResponse struct {
	Signature []byte
}

type EDDSAMPCRefreshRequest struct {
	KeyShare  EDDSAMPCKey
	SessionID []byte
}

type EDDSAMPCRefreshResponse struct {
	NewKeyShare EDDSAMPCKey
}

// ============================================================================
// Core API functions
// ============================================================================

// EDDSAMPCKeyGen performs algorithm-agnostic distributed key generation.
func EDDSAMPCKeyGen(jobmp *JobMP, req *EDDSAMPCKeyGenRequest) (*EDDSAMPCKeyGenResponse, error) {
	if jobmp == nil {
		return nil, fmt.Errorf("job must be provided")
	}
	if req == nil {
		return nil, fmt.Errorf("request cannot be nil")
	}
	if req.Curve == nil {
		return nil, fmt.Errorf("curve must be provided")
	}
	if jobmp.NParties() < 3 {
		return nil, fmt.Errorf("n-party EdDSA requires at least 3 parties")
	}

	key, err := cgobinding.KeyShareDKGCode(jobmp.cgo(), curve.Code(req.Curve))
	if err != nil {
		return nil, fmt.Errorf("EdDSA N-party key generation failed: %v", err)
	}
	return &EDDSAMPCKeyGenResponse{KeyShare: newEDDSAMPCKey(key)}, nil
}

// EDDSAMPCSign performs N-party EdDSA signing.
func EDDSAMPCSign(jobmp *JobMP, req *EDDSAMPCSignRequest) (*EDDSAMPCSignResponse, error) {
	if jobmp == nil {
		return nil, fmt.Errorf("job must be provided")
	}
	if req == nil {
		return nil, fmt.Errorf("request cannot be nil")
	}
	if jobmp.NParties() < 3 {
		return nil, fmt.Errorf("n-party signing requires at least 3 parties")
	}
	if len(req.Message) == 0 {
		return nil, fmt.Errorf("message cannot be empty")
	}

	sig, err := cgobinding.MPC_eddsampc_sign(jobmp.cgo(), req.KeyShare.cgobindingRef(), req.Message, req.SignatureReceiver)
	if err != nil {
		return nil, fmt.Errorf("EdDSA N-party signing failed: %v", err)
	}

	roleIdx := jobmp.GetPartyIndex()
	var sigBytes []byte
	if roleIdx == req.SignatureReceiver {
		sigBytes = sig
	}
	return &EDDSAMPCSignResponse{Signature: sigBytes}, nil
}

// EDDSAMPCRefresh re-shares secret without changing public key.
func EDDSAMPCRefresh(jobmp *JobMP, req *EDDSAMPCRefreshRequest) (*EDDSAMPCRefreshResponse, error) {
	if jobmp == nil {
		return nil, fmt.Errorf("job must be provided")
	}
	if req == nil {
		return nil, fmt.Errorf("request cannot be nil")
	}
	if jobmp.NParties() < 3 {
		return nil, fmt.Errorf("n-party refresh requires at least 3 parties")
	}
	sid := req.SessionID
	newKey, err := cgobinding.KeyShareRefresh(jobmp.cgo(), sid, req.KeyShare.cgobindingRef())
	if err != nil {
		return nil, fmt.Errorf("EdDSA N-party refresh failed: %v", err)
	}
	return &EDDSAMPCRefreshResponse{NewKeyShare: newEDDSAMPCKey(newKey)}, nil
}

// The threshold-DKG and ToAdditiveShare helpers reuse the same low-level bindings
// to avoid code duplication.

// EDDSAMPCThresholdDKGRequest holds the parameters for running the threshold-DKG
// protocol when creating an N-party EdDSA key.
type EDDSAMPCThresholdDKGRequest struct {
	Curve           curve.Curve      // Elliptic curve to use
	SessionID       []byte           // Optional caller-supplied session identifier
	AccessStructure *AccessStructure // Quorum access-structure description
	QuorumRIDs      []int            // (Optional) indices of parties that will form the quorum; defaults to all parties if nil/empty
}

// EDDSAMPCThresholdDKGResponse contains the key share produced for the calling
// party by the threshold-DKG protocol.
type EDDSAMPCThresholdDKGResponse struct {
	KeyShare EDDSAMPCKey
}

// EDDSAMPCThresholdDKG executes the threshold DKG protocol for EdDSA and
// returns the caller's key share.
func EDDSAMPCThresholdDKG(jobmp *JobMP, req *EDDSAMPCThresholdDKGRequest) (*EDDSAMPCThresholdDKGResponse, error) {
	if jobmp == nil {
		return nil, fmt.Errorf("job must be provided")
	}
	if req == nil {
		return nil, fmt.Errorf("request cannot be nil")
	}
	if req.Curve == nil {
		return nil, fmt.Errorf("curve must be provided")
	}

	sid := req.SessionID

	if req.AccessStructure == nil {
		return nil, fmt.Errorf("access structure must be provided")
	}

	acPtr := req.AccessStructure.toCryptoAC()

	roleIndices := req.QuorumRIDs
	if len(roleIndices) == 0 {
		roleIndices = make([]int, jobmp.NParties())
		for i := 0; i < jobmp.NParties(); i++ {
			roleIndices[i] = i
		}
	}

	keyShareRef, err := cgobinding.ThresholdDKGCode(jobmp.cgo(), curve.Code(req.Curve), sid, acPtr, roleIndices)
	if err != nil {
		return nil, fmt.Errorf("EdDSA threshold DKG failed: %v", err)
	}

	return &EDDSAMPCThresholdDKGResponse{KeyShare: newEDDSAMPCKey(keyShareRef)}, nil
}

func (k EDDSAMPCKey) ToAdditiveShare(ac *AccessStructure, quorumPartyNames []string) (EDDSAMPCKey, error) {
	if ac == nil {
		return EDDSAMPCKey{}, fmt.Errorf("access structure must be provided")
	}
	if len(quorumPartyNames) == 0 {
		return EDDSAMPCKey{}, fmt.Errorf("quorumPartyNames cannot be empty")
	}

	acPtr := ac.toCryptoAC()
	keyRef := cgobinding.Mpc_eckey_mp_ref(k)
	addRef, err := (&keyRef).ToAdditiveShare(acPtr, quorumPartyNames)
	if err != nil {
		return EDDSAMPCKey{}, err
	}
	return newEDDSAMPCKey(addRef), nil
}
