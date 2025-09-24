package mpc

import (
	"fmt"

	"crypto/sha256"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/curvemap"
)

// ECDSA2PCKey is an opaque handle to a 2-party ECDSA key share.
//
// It intentionally does **not** expose the underlying cgobinding type so that
// callers of the API do not need to import the low-level binding package. The
// only supported operation right now is an internal conversion back to the
// cgobinding representation so that the implementation can keep using the
// existing MPC primitives. Additional helper functions (e.g. serialization,
// freeing resources) can be added later.
//
// NOTE: the zero value of ECDSA2PCKey is considered invalid and can be used in
// tests to assert a key share was returned.
type ECDSA2PCKey cgobinding.Mpc_ecdsa2pc_key_ref

// cgobindingRef converts the wrapper back to the underlying cgobinding type.
// It is unexported because callers outside this package should never rely on
// the cgobinding representation.
func (k ECDSA2PCKey) cgobindingRef() cgobinding.Mpc_ecdsa2pc_key_ref {
	return cgobinding.Mpc_ecdsa2pc_key_ref(k)
}

// RoleIndex returns which party (e.g., 0 or 1) owns this key share.
// It delegates to the underlying cgobinding implementation.
func (k ECDSA2PCKey) RoleIndex() (int, error) {
	return cgobinding.KeyRoleIndex(k.cgobindingRef())
}

// Q returns the public key point associated with the distributed key. The
// returned Point must be freed by the caller once no longer needed.
func (k ECDSA2PCKey) Q() (*curve.Point, error) {
	cPointRef, err := cgobinding.KeyQ(k.cgobindingRef())
	if err != nil {
		return nil, err
	}
	bytes := cgobinding.ECCPointToBytes(cPointRef)
	return curve.NewPointFromBytes(bytes)
}

// Curve returns the elliptic curve associated with this key.
// The caller is responsible for freeing the returned Curve when done.
func (k ECDSA2PCKey) Curve() (curve.Curve, error) {
	code, err := cgobinding.KeyCurveCode(k.cgobindingRef())
	if err != nil {
		return nil, err
	}

	return curvemap.CurveForCode(code)
}

// XShare returns the scalar share x_i held by this party.
func (k ECDSA2PCKey) XShare() (*curve.Scalar, error) {
	bytes, err := cgobinding.KeyXShare(k.cgobindingRef())
	if err != nil {
		return nil, err
	}
	return &curve.Scalar{Bytes: bytes}, nil
}

// ECDSA2PCKeyGenRequest represents the input parameters for ECDSA 2PC key generation
type ECDSA2PCKeyGenRequest struct {
	Curve curve.Curve // Curve to use for key generation
}

// ECDSA2PCKeyGenResponse represents the output of ECDSA 2PC key generation
type ECDSA2PCKeyGenResponse struct {
	KeyShare ECDSA2PCKey // The party's share of the key
}

// ECDSA2PCKeyGen executes the distributed key generation protocol between two parties.
// Both parties will generate complementary key shares that can be used together for signing.
func ECDSA2PCKeyGen(job2p *Job2P, req *ECDSA2PCKeyGenRequest) (*ECDSA2PCKeyGenResponse, error) {
	if req == nil || req.Curve == nil {
		return nil, fmt.Errorf("curve must be provided")
	}

	// Execute the distributed key generation using the provided Job2P
	keyShareRef, err := cgobinding.DistributedKeyGen(job2p.cgo(), curve.Code(req.Curve))
	if err != nil {
		return nil, fmt.Errorf("ECDSA 2PC key generation failed: %v", err)
	}

	return &ECDSA2PCKeyGenResponse{KeyShare: ECDSA2PCKey(keyShareRef)}, nil
}

// ECDSA2PCSignRequest represents the input parameters for ECDSA 2PC signing
type ECDSA2PCSignRequest struct {
	SessionID []byte      // Session identifier for the signing operation
	KeyShare  ECDSA2PCKey // The party's share of the key
	Message   []byte      // The message to sign
}

// ECDSA2PCSignResponse represents the output of ECDSA 2PC signing
type ECDSA2PCSignResponse struct {
	Signature []byte // The ECDSA signature
}

// Verify verifies the DER-encoded signature against Q and 32-byte digest using the native crypto backend.
func (r *ECDSA2PCSignResponse) Verify(Q *curve.Point, digest []byte, c curve.Curve) error {
	if len(r.Signature) == 0 {
		return fmt.Errorf("empty signature")
	}
	if len(digest) != 32 {
		return fmt.Errorf("digest must be 32 bytes, got %d", len(digest))
	}
	// Build SEC1 uncompressed encoding: 0x04 || X || Y, with 32-byte padded coordinates
	pad32 := func(b []byte) []byte {
		if len(b) >= 32 {
			if len(b) == 32 {
				return b
			}
			// Trim if somehow longer
			return b[len(b)-32:]
		}
		p := make([]byte, 32)
		copy(p[32-len(b):], b)
		return p
	}
	x := pad32(Q.GetX())
	y := pad32(Q.GetY())
	pubOct := make([]byte, 1+32+32)
	pubOct[0] = 0x04
	copy(pubOct[1:1+32], x)
	copy(pubOct[1+32:], y)
	return cgobinding.ECCVerifyDER(curve.Code(c), pubOct, digest, r.Signature)
}

// ECDSA2PCSign executes the collaborative signing protocol between two parties.
// Both parties use their key shares to jointly create a signature for the given message.
func ECDSA2PCSign(job2p *Job2P, req *ECDSA2PCSignRequest) (*ECDSA2PCSignResponse, error) {
	if len(req.Message) == 0 {
		return nil, fmt.Errorf("message cannot be empty")
	}

	// Prepare 32-byte digest
	msg := req.Message
	if len(msg) != 32 {
		d := sha256.Sum256(msg)
		m := make([]byte, 32)
		copy(m, d[:])
		msg = m
	}

	// Execute the collaborative signing using batch API with a single message
	sigs, err := cgobinding.Sign(job2p.cgo(), req.SessionID, req.KeyShare.cgobindingRef(), [][]byte{msg})
	if err != nil {
		return nil, fmt.Errorf("ECDSA 2PC signing failed: %v", err)
	}
	if len(sigs) != 1 {
		return nil, fmt.Errorf("unexpected batch sign result")
	}

	return &ECDSA2PCSignResponse{Signature: sigs[0]}, nil
}

// ECDSA2PCRefreshRequest represents the parameters required to refresh (re-share)
// an existing 2-party ECDSA key.
//
// The protocol produces a fresh set of secret shares (x₁′, x₂′) that satisfy
// x₁′ + x₂′ = x₁ + x₂ mod n, i.e. the joint secret – and therefore the public
// key Q – remains unchanged while the individual shares are replaced with new
// uniformly-random values. Refreshing is useful to proactively rid the system
// of potentially compromised partial secrets.
//
// Only the existing key share is required as input because the curve is
// implicitly encoded in the key itself.
type ECDSA2PCRefreshRequest struct {
	KeyShare ECDSA2PCKey // The party's current key share to be refreshed
}

// ECDSA2PCRefreshResponse encapsulates the newly generated key share that
// replaces the caller's previous share.
type ECDSA2PCRefreshResponse struct {
	NewKeyShare ECDSA2PCKey // The refreshed key share for this party
}

// ECDSA2PCRefresh executes the key-refresh (re-share) protocol for an existing
// 2-party ECDSA key. Both parties must invoke this function concurrently with
// their respective messengers and key shares. On completion each party obtains
// a new, independent share such that the public key and the combined secret
// remain unchanged.
func ECDSA2PCRefresh(job2p *Job2P, req *ECDSA2PCRefreshRequest) (*ECDSA2PCRefreshResponse, error) {
	if req == nil {
		return nil, fmt.Errorf("request must be provided")
	}

	newKeyRef, err := cgobinding.Refresh(job2p.cgo(), req.KeyShare.cgobindingRef())
	if err != nil {
		return nil, fmt.Errorf("ECDSA 2PC refresh failed: %v", err)
	}

	return &ECDSA2PCRefreshResponse{NewKeyShare: ECDSA2PCKey(newKeyRef)}, nil
}
