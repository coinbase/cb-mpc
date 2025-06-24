package cgobinding

import (
	"unsafe"
)

/*
#cgo                                CXXFLAGS:  -std=c++17 -Wno-switch -Wno-parentheses -Wno-attributes -Wno-deprecated-declarations -DNO_DEPRECATED_OPENSSL
#cgo                                CFLAGS:    -Wno-deprecated-declarations
#cgo arm64                          CXXFLAGS:  -march=armv8-a+crypto
#cgo !linux                         LDFLAGS:   -lcrypto
#cgo android                        LDFLAGS:   -lcrypto -static-libstdc++
#cgo                                LDFLAGS:   -ldl
#cgo linux,!android                 CFLAGS:    -I/usr/local/include
#cgo linux,!android                 CXXFLAGS:  -I/usr/local/include
#cgo linux,!android                 LDFLAGS:   /usr/local/lib64/libcrypto.a
#cgo darwin,!iossimulator,!ios  	CFLAGS:    -I/usr/local/opt/openssl@3.2.0/include
#cgo darwin,!iossimulator,!ios  	CXXFLAGS:  -I/usr/local/opt/openssl@3.2.0/include
#cgo darwin,!iossimulator,!ios  	LDFLAGS:   -L/usr/local/opt/openssl@3.2.0/lib

#cgo CFLAGS:    -I${SRCDIR}
#cgo CXXFLAGS:  -I${SRCDIR}
#cgo CFLAGS:    -I/usr/local/opt/cbmpc/include
#cgo CXXFLAGS:  -I/usr/local/opt/cbmpc/include
#cgo LDFLAGS:   -L/usr/local/opt/cbmpc/lib
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
