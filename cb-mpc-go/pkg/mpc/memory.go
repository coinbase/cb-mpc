package mpc

import "runtime"

// Zeroize overwrites b with zeros as a best-effort attempt to reduce the
// lifetime of sensitive material (e.g., key blobs, private shares) in memory.
//
// Note: This only zeros the provided slice. Copies may still exist elsewhere
// (e.g., due to previous copies or GC behavior).
func Zeroize(b []byte) {
	for i := range b {
		b[i] = 0
	}
	runtime.KeepAlive(b)
}

// ZeroizeSlices zeroizes each slice in bs.
func ZeroizeSlices(bs [][]byte) {
	for _, b := range bs {
		Zeroize(b)
	}
}
