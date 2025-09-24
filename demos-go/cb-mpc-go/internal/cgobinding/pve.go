package cgobinding

/*
#cgo CXXFLAGS: -std=c++17
#include <stdlib.h>
#include <string.h>
#include "pve.h"
// Forward declarations of the Go callbacks – defined further below.

int go_pve_kem_encapsulate_bridge_ctx(
    void*,         // ctx
    cmem_t,        // ek
    cmem_t,        // rho
    cmem_t*,       // kem_ct out
    cmem_t*);      // kem_ss out
int go_pve_kem_decapsulate_bridge_ctx(
    void*,         // ctx
    void*,         // dk_handle
    cmem_t,        // kem_ct
    cmem_t*);      // kem_ss out

int go_pve_derive_pub_bridge_ctx(
    void*,         // ctx
    void*,         // dk_handle
    cmem_t*);      // out ek

// === single-party PVE helpers exposed by pve.h ===
int pve_encrypt(cmem_t, cmem_t, const char*, int, cmem_t*);
int pve_decrypt(cmem_t, cmem_t, const char*, int, cmem_t*);
int pve_verify(cmem_t, cmem_t, cmem_t, const char*);

// Activate context
void pve_activate_ctx(void*);
*/
import "C"

import (
	"errors"
	"fmt"
	"runtime"
	"sync"
	"sync/atomic"
	"unsafe"
)

// KEM describes the pluggable KEM backend used by the C++ PVE core.
// All byte slices are opaque references – the Go side decides their meaning.
// Implementations MUST be safe for concurrent use by multiple goroutines.
type KEM interface {
	Generate() (skRef, ek []byte, err error)
	Encapsulate(ek []byte, rho [32]byte) (ct, ss []byte, err error)
	Decapsulate(skHandle unsafe.Pointer, ct []byte) (ss []byte, err error)
	DerivePub(skRef []byte) ([]byte, error)
}

var (
	// Multi-instance registry keyed by opaque context pointers coming from C.
	instanceReg sync.Map // map[unsafe.Pointer]KEM
	nextCtxID   uint64
	// Ensures we register the C-side PKI callbacks only once per process.
	registerPKIFuncOnce sync.Once
	// Ensures we register the C-side KEM callbacks only once per process.
	registerKEMFuncOnce sync.Once
)

// ----------------------------------------------------------------------------
// PVE - single receiver, single value
// ----------------------------------------------------------------------------

func PVE_encrypt(pubKey []byte, x []byte, label string, curveCode int) ([]byte, error) {
	var out CMEM
	cLabel := C.CString(label)
	defer C.free(unsafe.Pointer(cLabel))
	rv := C.pve_encrypt(cmem(pubKey), cmem(x), cLabel, C.int(curveCode), (*C.cmem_t)(&out))
	if rv != 0 {
		return nil, fmt.Errorf("pve encrypt failed: %v", rv)
	}
	return CMEMGet(out), nil
}

func PVE_decrypt(prvKey []byte, ciphertext []byte, label string, curveCode int) ([]byte, error) {
	var out CMEM
	cLabel := C.CString(label)
	defer C.free(unsafe.Pointer(cLabel))
	rv := C.pve_decrypt(cmem(prvKey), cmem(ciphertext), cLabel, C.int(curveCode), (*C.cmem_t)(&out))
	if rv != 0 {
		return nil, fmt.Errorf("pve decrypt failed: %v", rv)
	}
	return CMEMGet(out), nil
}

func PVE_verify(pubKey []byte, ciphertext []byte, Q []byte, label string) error {
	cLabel := C.CString(label)
	defer C.free(unsafe.Pointer(cLabel))

	rv := C.pve_verify(cmem(pubKey), cmem(ciphertext), cmem(Q), cLabel)
	if rv != 0 {
		return fmt.Errorf("pve verify failed: %v", rv)
	}
	return nil
}

// ----------------------------------------------------------------------------
// PVE-AC - many receivers, many values
// ----------------------------------------------------------------------------
func PVE_AC_encrypt(ac C_AcPtr, names [][]byte, pubKeys [][]byte, count int, xs [][]byte, xsCount int, label string, curveCode int) ([]byte, error) {
	var out CMEM
	cLabel := C.CString(label)
	defer C.free(unsafe.Pointer(cLabel))

	// Pin array memory during the C call
	namesPin := makeCmems(names)
	pubPin := makeCmems(pubKeys)
	xsPin := makeCmems(xs)

	rv := C.pve_ac_encrypt((*C.crypto_ss_ac_ref)(&ac), namesPin.c, pubPin.c, C.int(count), xsPin.c, C.int(xsCount), cLabel, C.int(curveCode), &out)
	// Ensure Go slices are kept alive until after the C call returns
	runtime.KeepAlive(namesPin)
	runtime.KeepAlive(pubPin)
	runtime.KeepAlive(xsPin)
	if rv != 0 {
		return nil, fmt.Errorf("pve quorum encrypt (map) failed: %v", rv)
	}
	return CMEMGet(out), nil
}

func PVE_AC_party_decrypt_row(ac C_AcPtr, prvKey []byte, pveBundle []byte, label string, path string, rowIndex int) ([]byte, error) {
	var out CMEM
	cLabel := C.CString(label)
	cPath := C.CString(path)
	defer C.free(unsafe.Pointer(cLabel))
	defer C.free(unsafe.Pointer(cPath))

	rv := C.pve_ac_party_decrypt_row((*C.crypto_ss_ac_ref)(&ac), cmem(prvKey), cmem(pveBundle), cLabel, cPath, C.int(rowIndex), (*C.cmem_t)(&out))
	if rv != 0 {
		return nil, fmt.Errorf("pve quorum party_decrypt_row failed: %v", rv)
	}
	return CMEMGet(out), nil
}

func PVE_AC_aggregate_to_restore_row(ac C_AcPtr, pveBundle []byte, label string, paths [][]byte, shares [][]byte, rowIndex int) ([][]byte, error) {
	if len(paths) != len(shares) {
		return nil, fmt.Errorf("paths and shares length mismatch")
	}
	var out CMEMS
	cLabel := C.CString(label)
	defer C.free(unsafe.Pointer(cLabel))
	pathsPin := makeCmems(paths)
	sharesPin := makeCmems(shares)
	rv := C.pve_ac_aggregate_to_restore_row((*C.crypto_ss_ac_ref)(&ac), cmem(pveBundle), cLabel, pathsPin.c, sharesPin.c, C.int(len(paths)), C.int(rowIndex), &out)
	runtime.KeepAlive(pathsPin)
	runtime.KeepAlive(sharesPin)
	if rv != 0 {
		return nil, fmt.Errorf("pve quorum aggregate_to_restore_row failed: %v", rv)
	}
	return CMEMSGet(out), nil
}

func PVE_AC_verify(ac C_AcPtr, names [][]byte, pubKeys [][]byte, count int, pveBundle []byte, Xs [][]byte, xsCount int, label string) error {
	cLabel := C.CString(label)
	defer C.free(unsafe.Pointer(cLabel))

	namesPin := makeCmems(names)
	pubPin := makeCmems(pubKeys)
	xsPin := makeCmems(Xs)

	rv := C.pve_ac_verify((*C.crypto_ss_ac_ref)(&ac), namesPin.c, pubPin.c, C.int(count), cmem(pveBundle), xsPin.c, C.int(xsCount), cLabel)
	runtime.KeepAlive(namesPin)
	runtime.KeepAlive(pubPin)
	runtime.KeepAlive(xsPin)
	if rv != 0 {
		return fmt.Errorf("pve quorum verify (map) failed: %v", rv)
	}
	return nil
}

// RegisterKEMInstance registers a distinct KEM implementation and configures the C bridge.
func RegisterKEMInstance(p KEM) (unsafe.Pointer, error) {
	if p == nil {
		return nil, errors.New("RegisterKEMInstance: nil implementation")
	}
	registerKEMFuncOnce.Do(func() {
		C.pve_register_kem_functions(
			(C.kem_encap_ctx_fn)(unsafe.Pointer(C.go_pve_kem_encapsulate_bridge_ctx)),
			(C.kem_decap_ctx_fn)(unsafe.Pointer(C.go_pve_kem_decapsulate_bridge_ctx)),
			unsafe.Pointer(nil),
			(C.kem_dk_to_ek_ctx_fn)(unsafe.Pointer(C.go_pve_derive_pub_bridge_ctx)),
		)
	})

	// Fast-path: if already registered, return previous ctx.
	var foundCtx unsafe.Pointer
	instanceReg.Range(func(key, value any) bool {
		if value == p {
			foundCtx = key.(unsafe.Pointer)
			return false
		}
		return true
	})
	if foundCtx != nil {
		return foundCtx, nil
	}

	// Allocate an opaque context pointer.
	_ = atomic.AddUint64(&nextCtxID, 1)
	ctx := C.malloc(1)

	instanceReg.Store(ctx, p)
	return ctx, nil
}

//export go_pve_derive_pub_bridge_ctx
func go_pve_derive_pub_bridge_ctx(
	ctx unsafe.Pointer,
	dkHandle unsafe.Pointer,
	out *C.cmem_t,
) C.int {
	v, ok := instanceReg.Load(ctx)
	if !ok {
		return 1
	}
	impl := v.(KEM)
	var dkGo []byte
	if dkHandle != nil {
		cm := (*C.cmem_t)(dkHandle)
		if cm != nil && cm.data != nil && cm.size >= 0 {
			dkGo = unsafe.Slice((*byte)(unsafe.Pointer(cm.data)), int(cm.size))
		} else {
			p := uintptr(dkHandle)
			b := make([]byte, unsafe.Sizeof(p))
			for i := 0; i < len(b); i++ {
				b[i] = byte(p >> (8 * i))
			}
			dkGo = b
		}
	}
	ek, err := impl.DerivePub(dkGo)
	if err != nil {
		return 2
	}
	mem := C.malloc(C.size_t(len(ek)))
	if len(ek) > 0 {
		C.memcpy(mem, unsafe.Pointer(&ek[0]), C.size_t(len(ek)))
	}
	out.data = (*C.uint8_t)(mem)
	out.size = C.int(len(ek))
	return 0
}

//export go_pve_kem_encapsulate_bridge_ctx
func go_pve_kem_encapsulate_bridge_ctx(
	ctx unsafe.Pointer,
	ek C.cmem_t,
	rho C.cmem_t,
	ctOut *C.cmem_t,
	ssOut *C.cmem_t,
) C.int {
	v, ok := instanceReg.Load(ctx)
	if !ok {
		return 1
	}
	impl := v.(KEM)
	// Enforce exactly 32 bytes of seed entropy
	if int(rho.size) != 32 {
		return 3
	}
	ekGo := unsafe.Slice((*byte)(unsafe.Pointer(ek.data)), int(ek.size))
	rhoGo := unsafe.Slice((*byte)(unsafe.Pointer(rho.data)), int(rho.size))
	var rhoArr [32]byte
	copy(rhoArr[:], rhoGo)
	ct, ss, err := impl.Encapsulate(ekGo, rhoArr)
	if err != nil {
		return 2
	}
	ctMem := C.malloc(C.size_t(len(ct)))
	if len(ct) > 0 {
		C.memcpy(ctMem, unsafe.Pointer(&ct[0]), C.size_t(len(ct)))
	}
	ctOut.data = (*C.uint8_t)(ctMem)
	ctOut.size = C.int(len(ct))
	ssMem := C.malloc(C.size_t(len(ss)))
	if len(ss) > 0 {
		C.memcpy(ssMem, unsafe.Pointer(&ss[0]), C.size_t(len(ss)))
	}
	ssOut.data = (*C.uint8_t)(ssMem)
	ssOut.size = C.int(len(ss))
	return 0
}

//export go_pve_kem_decapsulate_bridge_ctx
func go_pve_kem_decapsulate_bridge_ctx(
	ctx unsafe.Pointer,
	dkHandle unsafe.Pointer,
	ct C.cmem_t,
	ssOut *C.cmem_t,
) C.int {
	v, ok := instanceReg.Load(ctx)
	if !ok {
		return 1
	}
	impl := v.(KEM)
	ctGo := unsafe.Slice((*byte)(unsafe.Pointer(ct.data)), int(ct.size))
	ss, err := impl.Decapsulate(dkHandle, ctGo)
	if err != nil {
		return 2
	}
	mem := C.malloc(C.size_t(len(ss)))
	if len(ss) > 0 {
		C.memcpy(mem, unsafe.Pointer(&ss[0]), C.size_t(len(ss)))
	}
	ssOut.data = (*C.uint8_t)(mem)
	ssOut.size = C.int(len(ss))
	return 0
}

// ActivateCtx tells the C shim which KEM instance is about to run.
func ActivateCtx(ctx unsafe.Pointer) { C.pve_activate_ctx(ctx) }
