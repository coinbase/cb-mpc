package mpc

import (
	"crypto/subtle"
	"runtime"
)

// SecureWipe overwrites the given byte slice with zeros using a constant-time
// copy to minimise the risk of compiler optimisations removing the call.  The
// slice length is left unchanged but its contents become all-zero.
//
// This is a best-effort helper – Go's garbage collector may still keep old
// copies alive until the next collection cycle.  Use it immediately after you
// no longer need a secret key reference that contains raw key material.
func SecureWipe(buf []byte) {
	if len(buf) == 0 {
		return
	}
	zero := make([]byte, len(buf))
	subtle.ConstantTimeCopy(1, buf, zero)
	// Keep the backing array alive until after the zeroization.
	runtime.KeepAlive(&buf[0])
}
