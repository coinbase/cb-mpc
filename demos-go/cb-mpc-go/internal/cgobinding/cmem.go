package cgobinding

import (
	"runtime"
	"unsafe"
)

/*
#cgo                                CXXFLAGS:  -std=c++17 -Wno-switch -Wno-parentheses -Wno-attributes -Wno-deprecated-declarations -DNO_DEPRECATED_OPENSSL
#cgo                                CFLAGS:    -Wno-deprecated-declarations
#cgo arm64                          CXXFLAGS:  -march=armv8-a+crypto
#cgo !linux                         LDFLAGS:   -lcrypto
#cgo android                        LDFLAGS:   -lcrypto -static-libstdc++
#cgo                                LDFLAGS:   -ldl
// Local headers/libs are provided via CGO_* environment variables.
// See scripts/go_with_cpp.sh for how we set:
//   CGO_CFLAGS/CGO_CXXFLAGS to include <repo>/src
//   CGO_LDFLAGS to include <repo>/build/<type>/lib and <repo>/lib/<type>
#cgo linux,!android                 CFLAGS:    -I/usr/local/include
#cgo linux,!android                 CXXFLAGS:  -I/usr/local/include
#cgo linux,!android                 LDFLAGS:   /usr/local/lib64/libcrypto.a
#cgo darwin,!iossimulator,!ios  	CFLAGS:    -I/usr/local/opt/openssl@3.2.0/include
#cgo darwin,!iossimulator,!ios  	CXXFLAGS:  -I/usr/local/opt/openssl@3.2.0/include
#cgo darwin,!iossimulator,!ios  	LDFLAGS:   -L/usr/local/opt/openssl@3.2.0/lib

#cgo CFLAGS:    -I${SRCDIR}
#cgo CXXFLAGS:  -I${SRCDIR}
#cgo LDFLAGS:   -lcbmpc
#cgo linux,!android                 LDFLAGS:   /usr/local/lib64/libcrypto.a

#include <stdlib.h>
#include <string.h>
#include "cblib.h"
*/
import "C"

// Memory Management Utilities

type CMEM = C.cmem_t

func cmem(in []byte) CMEM {
	var mem CMEM
	mem.size = C.int(len(in))
	if len(in) > 0 {
		mem.data = (*C.uchar)(&in[0])
	} else {
		mem.data = nil
	}
	return mem
}

func CMEMGet(cmem CMEM) []byte {
	if cmem.data == nil {
		return nil
	}
	out := C.GoBytes(unsafe.Pointer(cmem.data), cmem.size)
	C.memset(unsafe.Pointer(cmem.data), 0, C.ulong(cmem.size))
	C.free(unsafe.Pointer(cmem.data))
	return out
}

type CMEMS = C.cmems_t

func cmems(in [][]byte) CMEMS {
	var mems CMEMS
	count := len(in)
	if count > 0 {
		lens := make([]int32, count)
		mems.sizes = (*C.int)(&lens[0])
		mems.count = C.int(count)
		var n, k int
		for i := 0; i < count; i++ {
			l := len(in[i])
			lens[i] = int32(l)
			n += int(lens[i])
		}
		if n > 0 {
			data := make([]byte, n)
			for i := 0; i < count; i++ {
				l := len(in[i])
				if l > 0 {
					copy(data[k:k+l], in[i])
				}
				k += l
			}
			mems.data = (*C.uchar)(&data[0])
		} else {
			mems.data = nil
		}
	} else {
		mems.sizes = nil
		mems.data = nil
		mems.count = 0
	}
	return mems
}

func CMEMSGet(cmems CMEMS) [][]byte {
	if cmems.data == nil {
		return nil
	}
	count := int(cmems.count)
	out := make([][]byte, count)
	n := 0
	p := uintptr(unsafe.Pointer(cmems.data))
	for i := 0; i < count; i++ {
		// Inline array access to avoid dependency on network.go
		sizePtr := (*C.int)(unsafe.Pointer(uintptr(unsafe.Pointer(cmems.sizes)) + uintptr(i*int(unsafe.Sizeof(C.int(0))))))
		l := int(*sizePtr)
		out[i] = C.GoBytes(unsafe.Pointer(p), C.int(l))
		p += uintptr(l)
		n += l
	}
	C.memset(unsafe.Pointer(cmems.data), 0, C.ulong(n))
	C.free(unsafe.Pointer(cmems.data))
	C.free(unsafe.Pointer(cmems.sizes))
	return out
}

// cmemsPin holds Go-owned backing storage for a CMEMS so the Go GC cannot
// reclaim it while a C function is executing. Always call runtime.KeepAlive
// on the returned value after the C call returns.
type cmemsPin struct {
	c    CMEMS
	lens []int32
	data []byte
}

// makeCmems builds a CMEMS value backed by Go slices that stay reachable via
// the returned cmemsPin. Call runtime.KeepAlive(pin) after the C call.
func makeCmems(in [][]byte) cmemsPin {
	var mems CMEMS
	count := len(in)
	if count > 0 {
		lens := make([]int32, count)
		mems.sizes = (*C.int)(&lens[0])
		mems.count = C.int(count)
		var n, k int
		for i := 0; i < count; i++ {
			l := len(in[i])
			lens[i] = int32(l)
			n += int(lens[i])
		}
		var data []byte
		if n > 0 {
			data = make([]byte, n)
			for i := 0; i < count; i++ {
				l := len(in[i])
				if l > 0 {
					copy(data[k:k+l], in[i])
				}
				k += l
			}
			mems.data = (*C.uchar)(&data[0])
		} else {
			mems.data = nil
		}
		// Ensure the slices are considered live until function return
		// (and later via runtime.KeepAlive in callers).
		_ = lens
		_ = data
		return cmemsPin{c: mems, lens: lens, data: data}
	}
	mems.sizes = nil
	mems.data = nil
	mems.count = 0
	// KeepAlive on zero-value pin is harmless.
	pin := cmemsPin{c: mems}
	runtime.KeepAlive(pin)
	return pin
}
