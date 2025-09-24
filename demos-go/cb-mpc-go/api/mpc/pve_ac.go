package mpc

import (
	"fmt"
	"runtime"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// BaseEncPrivateKey is an opaque byte slice holding a serialized base enc private key (KEM dk)
type BaseEncPrivateKey []byte

// BaseEncPublicKey is an opaque byte slice holding a serialized base enc public key (KEM ek)
type BaseEncPublicKey []byte

// PVECiphertext is an opaque byte slice holding a serialized PVE bundle returned by encryption.
type PVECiphertext []byte

// PVEAcEncryptRequest represents a request for PVE encryption (backup)
type PVEAcEncryptRequest struct {
	AccessStructure *AccessStructure            // Quorum policy & curve description
	PublicKeys      map[string]BaseEncPublicKey // Map of leaf name -> public encryption key
	Curve           curve.Curve                 // Optional override curve (nil => derive from AccessStructure or default P-256)
	PrivateValues   []*curve.Scalar             // Private data to backup (key shares)
	Label           string                      // Human-readable label bound to the backup
}

type PVEAcEncryptResponse struct{ EncryptedBundle PVECiphertext }

// AcEncrypt performs publicly verifiable encryption of private shares for backup using the active KEM backend.
func (p *PVE) AcEncrypt(req *PVEAcEncryptRequest) (*PVEAcEncryptResponse, error) {
	if req == nil {
		return nil, fmt.Errorf("request cannot be nil")
	}
	if len(req.PrivateValues) == 0 {
		return nil, fmt.Errorf("private shares cannot be empty")
	}
	if len(req.PublicKeys) == 0 {
		return nil, fmt.Errorf("public keys map cannot be empty")
	}
	if req.Label == "" {
		return nil, fmt.Errorf("label cannot be empty")
	}
	if req.AccessStructure == nil {
		return nil, fmt.Errorf("access structure cannot be nil")
	}
	// Determine curve
	if req.Curve == nil {
		if req.AccessStructure.Curve != nil {
			req.Curve = req.AccessStructure.Curve
		} else {
			p256, err := curve.NewP256()
			if err != nil {
				return nil, fmt.Errorf("failed to initialise default curve: %v", err)
			}
			req.Curve = p256
		}
	}
	// Build inputs
	names := make([][]byte, 0, len(req.PublicKeys))
	pubKeys := make([][]byte, 0, len(req.PublicKeys))
	for name, key := range req.PublicKeys {
		names = append(names, []byte(name))
		pubKeys = append(pubKeys, []byte(key))
	}
	xs := make([][]byte, len(req.PrivateValues))
	for i, s := range req.PrivateValues {
		if s == nil {
			return nil, fmt.Errorf("private share %d is nil", i)
		}
		xs[i] = s.Bytes
	}
	acPtr := req.AccessStructure.toCryptoAC()
	defer cgobinding.FreeAccessStructure(acPtr)
	// Ensure the correct KEM instance is active in the native layer and run on one OS thread.
	runtime.LockOSThread()
	defer runtime.UnlockOSThread()
	p.activateCtx()
	rawBundle, err := cgobinding.PVE_AC_encrypt(acPtr, names, pubKeys, len(pubKeys), xs, len(xs), req.Label, curve.Code(req.Curve))
	if err != nil {
		return nil, fmt.Errorf("PVE encryption failed: %v", err)
	}
	return &PVEAcEncryptResponse{EncryptedBundle: PVECiphertext(rawBundle)}, nil
}

type PVEAcPartyDecryptRowRequest struct {
	AccessStructure *AccessStructure
	Path            string
	PrivateKey      BaseEncPrivateKey
	EncryptedBundle PVECiphertext
	Label           string
	// RowIndex selects the commitment row to use during decryption.
	// Theoretically it can be any value in [0, kappa). In practice, try RowIndex = 0 first;
	// decryption should succeed. If RowIndex = 0 fails, it usually indicates a mismatch
	// (e.g. label, public keys, or public shares), so halting and inspecting is preferable
	// to iterating over other indices.
	RowIndex int
}

type PVEAcPartyDecryptRowResponse struct{ Share []byte }

func (p *PVE) AcPartyDecryptRow(req *PVEAcPartyDecryptRowRequest) (*PVEAcPartyDecryptRowResponse, error) {
	if req == nil {
		return nil, fmt.Errorf("request cannot be nil")
	}
	if req.AccessStructure == nil {
		return nil, fmt.Errorf("access structure cannot be nil")
	}
	if req.Path == "" {
		return nil, fmt.Errorf("path cannot be empty")
	}
	if len(req.PrivateKey) == 0 {
		return nil, fmt.Errorf("private key cannot be empty")
	}
	if req.Label == "" {
		return nil, fmt.Errorf("label cannot be empty")
	}
	acPtr := req.AccessStructure.toCryptoAC()
	defer cgobinding.FreeAccessStructure(acPtr)
	runtime.LockOSThread()
	defer runtime.UnlockOSThread()
	p.activateCtx()
	share, err := cgobinding.PVE_AC_party_decrypt_row(acPtr, []byte(req.PrivateKey), []byte(req.EncryptedBundle), req.Label, req.Path, req.RowIndex)
	if err != nil {
		return nil, err
	}
	return &PVEAcPartyDecryptRowResponse{Share: share}, nil
}

type PVEAcAggregateToRestoreRowRequest struct {
	AccessStructure *AccessStructure
	EncryptedBundle PVECiphertext
	Label           string
	// RowIndex must match the row used to collect party shares.
	// Any value in [0, kappa) is valid, but the typical (and expected) choice is 0.
	// If aggregation at 0 fails, prefer investigating input correctness over trying other indices.
	RowIndex int
	Shares   map[string][]byte // path -> share
}

type PVEAcAggregateToRestoreRowResponse struct{ PrivateValues []*curve.Scalar }

func (p *PVE) AcAggregateToRestoreRow(req *PVEAcAggregateToRestoreRowRequest) (*PVEAcAggregateToRestoreRowResponse, error) {
	if req == nil {
		return nil, fmt.Errorf("request cannot be nil")
	}
	if req.AccessStructure == nil {
		return nil, fmt.Errorf("access structure cannot be nil")
	}
	if req.Label == "" {
		return nil, fmt.Errorf("label cannot be empty")
	}
	if len(req.Shares) == 0 {
		return nil, fmt.Errorf("shares cannot be empty")
	}
	acPtr := req.AccessStructure.toCryptoAC()
	defer cgobinding.FreeAccessStructure(acPtr)
	paths := make([][]byte, 0, len(req.Shares))
	shares := make([][]byte, 0, len(req.Shares))
	for path, sh := range req.Shares {
		paths = append(paths, []byte(path))
		shares = append(shares, sh)
	}
	runtime.LockOSThread()
	defer runtime.UnlockOSThread()
	p.activateCtx()
	recovered, err := cgobinding.PVE_AC_aggregate_to_restore_row(acPtr, []byte(req.EncryptedBundle), req.Label, paths, shares, req.RowIndex)
	if err != nil {
		return nil, err
	}
	orderLen := len(req.AccessStructure.Curve.Order())
	scalars := make([]*curve.Scalar, len(recovered))
	for i, s := range recovered {
		if len(s) > orderLen {
			s = s[len(s)-orderLen:]
		}
		scalars[i] = &curve.Scalar{Bytes: s}
	}
	return &PVEAcAggregateToRestoreRowResponse{PrivateValues: scalars}, nil
}

type PVEAcVerifyRequest struct {
	AccessStructure *AccessStructure
	PublicKeys      map[string]BaseEncPublicKey
	EncryptedBundle PVECiphertext
	PublicShares    []*curve.Point
	Label           string
}

type PVEAcVerifyResponse struct{ Valid bool }

// AcVerify checks whether the provided PVE ciphertext is valid with respect to the given public information.
func (p *PVE) AcVerify(req *PVEAcVerifyRequest) (*PVEAcVerifyResponse, error) {
	if req == nil {
		return nil, fmt.Errorf("request cannot be nil")
	}
	if req.AccessStructure == nil {
		return nil, fmt.Errorf("access structure cannot be nil")
	}
	if len(req.PublicKeys) == 0 {
		return nil, fmt.Errorf("public keys cannot be empty")
	}
	if len(req.PublicShares) == 0 {
		return nil, fmt.Errorf("public shares cannot be empty")
	}
	if req.Label == "" {
		return nil, fmt.Errorf("label cannot be empty")
	}
	acPtr := req.AccessStructure.toCryptoAC()
	defer cgobinding.FreeAccessStructure(acPtr)
	leafNames := collectLeafNames(req.AccessStructure.Root)
	names := make([][]byte, len(leafNames))
	pubBytes := make([][]byte, len(leafNames))
	for i, name := range leafNames {
		pk, ok := req.PublicKeys[name]
		if !ok {
			return nil, fmt.Errorf("missing public key for leaf %s", name)
		}
		names[i] = []byte(name)
		pubBytes[i] = []byte(pk)
	}
	xsBytes := make([][]byte, len(req.PublicShares))
	for i, pt := range req.PublicShares {
		if pt == nil {
			return nil, fmt.Errorf("public share %d is nil", i)
		}
		xsBytes[i] = pt.Bytes()
	}
	runtime.LockOSThread()
	defer runtime.UnlockOSThread()
	p.activateCtx()
	if err := cgobinding.PVE_AC_verify(acPtr, names, pubBytes, len(pubBytes), []byte(req.EncryptedBundle), xsBytes, len(xsBytes), req.Label); err != nil {
		return &PVEAcVerifyResponse{Valid: false}, err
	}
	return &PVEAcVerifyResponse{Valid: true}, nil
}

// collectLeafNames performs a DFS traversal to return leaf names in deterministic order.
func collectLeafNames(root *AccessNode) []string {
	var res []string
	var walk func(n *AccessNode)
	walk = func(n *AccessNode) {
		if n == nil {
			return
		}
		if n.Kind == KindLeaf {
			res = append(res, n.Name)
			return
		}
		for _, c := range n.Children {
			walk(c)
		}
	}
	walk(root)
	return res
}
