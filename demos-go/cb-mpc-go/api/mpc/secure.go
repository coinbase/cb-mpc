package mpc

import (
	"crypto/hmac"
	"crypto/sha256"
	"crypto/subtle"
)

// ExpandFromSeed deterministically expands a 32-byte seed into l bytes.
// If l <= 32, it returns the first l bytes. If l > 32, it uses HMAC-SHA256
// with a counter to derive additional pseudorandom blocks securely.
// This is suitable for seeding or deriving per-encapsulation randomness.
func ExpandFromSeed(seed [32]byte, l int) []byte {
	if l <= 0 {
		return nil
	}
	out := make([]byte, 0, l)
	// Generate blocks: HMAC_SHA256(seed, "rho" || counter)
	// counter starts at 1
	var counter byte = 1
	for len(out) < l {
		h := hmac.New(sha256.New, seed[:])
		// domain separation string
		h.Write([]byte("rho"))
		h.Write([]byte{counter})
		block := h.Sum(nil)
		need := l - len(out)
		if need > len(block) {
			need = len(block)
		}
		out = append(out, block[:need]...)
		counter++
	}
	return out
}

// SecureWipe overwrites the given byte slice with zeros using a constant-time
// copy to minimise the risk of compiler optimisations removing the call.  The
// slice length is left unchanged but its contents become all-zero.
//
// This is a best-effort helper – Go's garbage collector may still keep old
// copies alive until the next collection cycle.  Use it immediately after you
// no longer need a secret key reference that contains raw key material.
func SecureWipe(buf []byte) {
	if buf == nil {
		return
	}
	zero := make([]byte, len(buf))
	subtle.ConstantTimeCopy(1, buf, zero)
}
