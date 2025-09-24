package mpc

import (
	"fmt"
	"runtime"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// ===================== Single-party PVE (ec_pve_t) ==========================

// PVEEncryptRequest represents a request to back-up a single secret scalar `x`.
//
//   - PublicKey: the ECIES public key used for encryption (BaseEncPublicKey).
//   - PrivateValue: the secret scalar to encrypt (curve.Scalar).
//   - Curve: the elliptic curve of the scalar. If nil, defaults to P-256.
//   - Label: a human-readable domain-separator bound into the ciphertext.
//
// All fields are mandatory. The response contains the opaque PVECiphertext blob
// that must be persisted together with the public share (x * G) so that future
// verification and decryption operations can be performed.
type PVEEncryptRequest struct {
	PublicKey    BaseEncPublicKey
	PrivateValue *curve.Scalar
	Curve        curve.Curve
	Label        string
}

type PVEEncryptResponse struct {
	Ciphertext PVECiphertext
}

// Encrypt backs up a single secret scalar using the configuration bound to the
// receiving PVE handle.  All semantic requirements remain identical to the
// former package-level helper that this method replaces.
func (p *PVE) Encrypt(req *PVEEncryptRequest) (*PVEEncryptResponse, error) {
	if req == nil {
		return nil, fmt.Errorf("request cannot be nil")
	}
	if req.PrivateValue == nil {
		return nil, fmt.Errorf("private value cannot be nil")
	}
	if len(req.PublicKey) == 0 {
		return nil, fmt.Errorf("public key cannot be empty")
	}
	if req.Label == "" {
		return nil, fmt.Errorf("label cannot be empty")
	}
	if req.Curve == nil {
		return nil, fmt.Errorf("curve cannot be nil")
	}

	// Ensure the correct KEM instance is active in the native layer on this OS thread.
	runtime.LockOSThread()
	defer runtime.UnlockOSThread()
	p.activateCtx()

	cipher, err := cgobinding.PVE_encrypt(
		[]byte(req.PublicKey),
		req.PrivateValue.Bytes,
		req.Label,
		curve.Code(req.Curve),
	)
	if err != nil {
		return nil, err
	}

	return &PVEEncryptResponse{Ciphertext: PVECiphertext(cipher)}, nil
}

// ====================== Decryption ==========================================

type PVEDecryptRequest struct {
	PrivateKey BaseEncPrivateKey
	Ciphertext PVECiphertext
	Curve      curve.Curve
	Label      string
}

type PVEDecryptResponse struct {
	PrivateValue *curve.Scalar
}

// Decrypt recovers the secret scalar from a previously produced ciphertext.
// It mirrors the behaviour of the former PVEDecrypt helper.
func (p *PVE) Decrypt(req *PVEDecryptRequest) (*PVEDecryptResponse, error) {
	if req == nil {
		return nil, fmt.Errorf("request cannot be nil")
	}
	if len(req.PrivateKey) == 0 {
		return nil, fmt.Errorf("private key cannot be empty")
	}
	if len(req.Ciphertext) == 0 {
		return nil, fmt.Errorf("ciphertext cannot be empty")
	}
	if req.Label == "" {
		return nil, fmt.Errorf("label cannot be empty")
	}
	if req.Curve == nil {
		return nil, fmt.Errorf("curve cannot be nil")
	}

	// Ensure the correct KEM instance is active in the native layer on this OS thread.
	runtime.LockOSThread()
	defer runtime.UnlockOSThread()
	p.activateCtx()

	xBytes, err := cgobinding.PVE_decrypt(
		[]byte(req.PrivateKey),
		[]byte(req.Ciphertext),
		req.Label,
		curve.Code(req.Curve),
	)
	if err != nil {
		return nil, err
	}

	// Ensure correct length relative to curve order
	orderLen := len(req.Curve.Order())
	if len(xBytes) > orderLen {
		xBytes = xBytes[len(xBytes)-orderLen:]
	}

	return &PVEDecryptResponse{PrivateValue: &curve.Scalar{Bytes: xBytes}}, nil
}

// ====================== Verification ========================================

type PVEVerifyRequest struct {
	PublicKey   BaseEncPublicKey
	Ciphertext  PVECiphertext
	PublicShare *curve.Point
	Label       string
}

type PVEVerifyResponse struct {
	Valid bool
}

// Verify checks whether the ciphertext is a valid encryption of the provided
// public share under the embedded PKE scheme.  The logic is unchanged from the
// old stand-alone function.
func (p *PVE) Verify(req *PVEVerifyRequest) (*PVEVerifyResponse, error) {
	if req == nil {
		return nil, fmt.Errorf("request cannot be nil")
	}
	if len(req.PublicKey) == 0 {
		return nil, fmt.Errorf("public key cannot be empty")
	}
	if len(req.Ciphertext) == 0 {
		return nil, fmt.Errorf("ciphertext cannot be empty")
	}
	if req.PublicShare == nil {
		return nil, fmt.Errorf("public share cannot be nil")
	}
	if req.Label == "" {
		return nil, fmt.Errorf("label cannot be empty")
	}

	// Ensure the correct KEM instance is active in the native layer on this OS thread.
	runtime.LockOSThread()
	defer runtime.UnlockOSThread()
	p.activateCtx()

	err := cgobinding.PVE_verify(
		[]byte(req.PublicKey),
		[]byte(req.Ciphertext),
		req.PublicShare.Bytes(),
		req.Label,
	)
	if err != nil {
		return &PVEVerifyResponse{Valid: false}, err
	}
	return &PVEVerifyResponse{Valid: true}, nil
}
