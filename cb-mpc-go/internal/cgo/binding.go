// Package cgo contains all CGO bindings to cb-mpc's public C API.
package cgo

/*
#cgo CFLAGS: -Wno-deprecated-declarations
#cgo CXXFLAGS: -std=c++17 -Wno-deprecated-declarations -Wno-switch -Wno-parentheses -Wno-attributes
#cgo linux CFLAGS: -fstack-protector-strong -D_FORTIFY_SOURCE=2
#cgo linux CXXFLAGS: -fstack-protector-strong -D_FORTIFY_SOURCE=2
#cgo darwin CFLAGS: -fstack-protector-strong
#cgo darwin CXXFLAGS: -fstack-protector-strong
#cgo arm64 CXXFLAGS: -march=armv8-a+crypto
#cgo LDFLAGS: -lcbmpc -lcrypto -ldl
#cgo darwin LDFLAGS: -lc++
#cgo linux,!android LDFLAGS: -lstdc++
#cgo android LDFLAGS: -lstdc++

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <cbmpc/c_api/access_structure.h>
#include <cbmpc/c_api/common.h>
#include <cbmpc/c_api/ecdsa_2p.h>
#include <cbmpc/c_api/ecdsa_mp.h>
#include <cbmpc/c_api/eddsa_2p.h>
#include <cbmpc/c_api/eddsa_mp.h>
#include <cbmpc/c_api/job.h>
#include <cbmpc/c_api/pve_base_pke.h>
#include <cbmpc/c_api/pve_batch_ac.h>
#include <cbmpc/c_api/pve_batch_single_recipient.h>
#include <cbmpc/c_api/schnorr_2p.h>
#include <cbmpc/c_api/schnorr_mp.h>
#include <cbmpc/c_api/tdh2.h>

extern cbmpc_error_t goTransportSend(void* ctx, int32_t receiver, uint8_t* data, int size);
extern cbmpc_error_t goTransportReceive(void* ctx, int32_t sender, cmem_t* out_msg);
extern cbmpc_error_t goTransportReceiveAll(void* ctx, int32_t* senders, int senders_count, cmems_t* out_msgs);
extern void goTransportFree(void* ctx, void* ptr);
extern cbmpc_error_t goPVEBasePKEEncrypt(void* ctx, cmem_t ek, cmem_t label, cmem_t plain, cmem_t rho, cmem_t* out_ct);
extern cbmpc_error_t goPVEBasePKEDecrypt(void* ctx, cmem_t dk, cmem_t label, cmem_t ct, cmem_t* out_plain);
extern cbmpc_error_t goPVERSAOAEPHSMDecap(void* ctx, cmem_t dk_handle, cmem_t kem_ct, cmem_t* out_kem_ss);
extern cbmpc_error_t goPVEECIESP256HSMECDH(void* ctx, cmem_t dk_handle, cmem_t kem_ct, cmem_t* out_dh_x32);
extern cbmpc_error_t goPVEKEMEncap(void* ctx, cmem_t ek, cmem_t rho32, cmem_t* out_kem_ct, cmem_t* out_kem_ss);
extern cbmpc_error_t goPVEKEMDecap(void* ctx, cmem_t dk, cmem_t kem_ct, cmem_t* out_kem_ss);
*/
import "C"

import (
	"context"
	"errors"
	"fmt"
	"math"
	"net"
	"runtime"
	"strings"
	"sync"
	"unsafe"
)

// Transport is the network contract required by cb-mpc interactive protocols.
type Transport interface {
	Send(ctx context.Context, toParty int, msg []byte) error
	Receive(ctx context.Context, fromParty int) ([]byte, error)
	ReceiveAll(ctx context.Context, fromParties []int) ([][]byte, error)
}

// CurveID matches cbmpc_curve_id_t.
type CurveID int32

const (
	CurveP256      CurveID = CurveID(C.CBMPC_CURVE_P256)
	CurveSecp256k1 CurveID = CurveID(C.CBMPC_CURVE_SECP256K1)
	CurveEd25519   CurveID = CurveID(C.CBMPC_CURVE_ED25519)
)

// TwoPartySelf matches cbmpc_2pc_party_t.
type TwoPartySelf int32

const (
	TwoPartyP1 TwoPartySelf = TwoPartySelf(C.CBMPC_2PC_P1)
	TwoPartyP2 TwoPartySelf = TwoPartySelf(C.CBMPC_2PC_P2)
)

// Job2P is the explicit 2-party execution context passed to cb-mpc.
type Job2P struct {
	Self      TwoPartySelf
	P1Name    string
	P2Name    string
	Transport Transport
}

// JobMP is the explicit multi-party execution context passed to cb-mpc.
type JobMP struct {
	Self       int
	PartyNames []string
	Transport  Transport
}

// AccessStructureNodeType matches cbmpc_access_structure_node_type_t.
type AccessStructureNodeType int32

const (
	AccessNodeLeaf      AccessStructureNodeType = AccessStructureNodeType(C.CBMPC_ACCESS_STRUCTURE_NODE_LEAF)
	AccessNodeAnd       AccessStructureNodeType = AccessStructureNodeType(C.CBMPC_ACCESS_STRUCTURE_NODE_AND)
	AccessNodeOr        AccessStructureNodeType = AccessStructureNodeType(C.CBMPC_ACCESS_STRUCTURE_NODE_OR)
	AccessNodeThreshold AccessStructureNodeType = AccessStructureNodeType(C.CBMPC_ACCESS_STRUCTURE_NODE_THRESHOLD)
)

// AccessStructureNode is a Go representation of a C access-structure node.
type AccessStructureNode struct {
	Type       AccessStructureNodeType
	LeafName   string
	ThresholdK int32
	Children   []int32
}

// AccessStructure is a Go representation of cbmpc_access_structure_t.
type AccessStructure struct {
	Nodes []AccessStructureNode
	Root  int32
}

// PVEBasePKECallbacks maps to cbmpc_pve_base_pke_t callback hooks.
type PVEBasePKECallbacks struct {
	Encrypt func(ek, label, plain, rho []byte) ([]byte, error)
	Decrypt func(dk, label, ct []byte) ([]byte, error)
}

// PVERSAOAEPHSMDecapCallbacks maps to cbmpc_pve_rsa_oaep_hsm_decap_t.
type PVERSAOAEPHSMDecapCallbacks struct {
	Decap func(dkHandle, kemCT []byte) ([]byte, error)
}

// PVEECIESP256HSMECDHCallbacks maps to cbmpc_pve_ecies_p256_hsm_ecdh_t.
type PVEECIESP256HSMECDHCallbacks struct {
	ECDH func(dkHandle, kemCT []byte) ([]byte, error)
}

// PVEBaseKEMCallbacks maps to cbmpc_pve_base_kem_t callback hooks.
type PVEBaseKEMCallbacks struct {
	Encap func(ek, rho32 []byte) (kemCT []byte, kemSS []byte, err error)
	Decap func(dk, kemCT []byte) ([]byte, error)
}

type transportContext struct {
	ctx       context.Context
	transport Transport

	mu               sync.Mutex
	lastTransportErr error
}

var (
	transportMap sync.Map

	pveBasePKEMap          sync.Map
	pveRSAOAEPHSMDecapMap  sync.Map
	pveECIESP256HSMECDHMap sync.Map
	pveBaseKEMMap          sync.Map
)

func registerTransport(ctx context.Context, transport Transport) (unsafe.Pointer, *transportContext, error) {
	if transport == nil {
		return nil, nil, fmt.Errorf("transport cannot be nil")
	}
	if ctx == nil {
		ctx = context.Background()
	}
	ptr := C.malloc(1)
	if ptr == nil {
		return nil, nil, fmt.Errorf("failed to allocate transport context")
	}
	tc := &transportContext{
		ctx:       ctx,
		transport: transport,
	}
	transportMap.Store(ptr, tc)
	return ptr, tc, nil
}

func unregisterTransport(ptr unsafe.Pointer) {
	if ptr == nil {
		return
	}
	transportMap.Delete(ptr)
	C.free(ptr)
}

func getTransport(ptr unsafe.Pointer) (*transportContext, bool) {
	value, ok := transportMap.Load(ptr)
	if !ok {
		return nil, false
	}
	tc, ok := value.(*transportContext)
	return tc, ok
}

var ErrTransportClosed = errors.New("cgo: transport closed")

type TransportError struct {
	Status error
	Cause  error
}

func (e *TransportError) Error() string {
	if e.Status == nil {
		if e.Cause == nil {
			return ""
		}
		return e.Cause.Error()
	}
	if e.Cause == nil {
		return e.Status.Error()
	}
	return fmt.Sprintf("%v: %v", e.Status, e.Cause)
}

func (e *TransportError) Unwrap() []error {
	errs := make([]error, 0, 2)
	if e.Status != nil {
		errs = append(errs, e.Status)
	}
	if e.Cause != nil {
		errs = append(errs, e.Cause)
	}
	return errs
}

func normalizeTransportErr(err error) error {
	if err == nil {
		return nil
	}
	errText := strings.ToLower(err.Error())
	if errors.Is(err, net.ErrClosed) ||
		strings.Contains(errText, "session closed") ||
		strings.Contains(errText, "transport closed") {
		return ErrTransportClosed
	}
	return err
}

func (tc *transportContext) storeTransportErr(err error) {
	tc.mu.Lock()
	defer tc.mu.Unlock()
	tc.lastTransportErr = normalizeTransportErr(err)
}

func (tc *transportContext) takeTransportErr() error {
	tc.mu.Lock()
	defer tc.mu.Unlock()
	err := tc.lastTransportErr
	tc.lastTransportErr = nil
	return err
}

func registerPVEBasePKECallbacks(cb *PVEBasePKECallbacks) (unsafe.Pointer, error) {
	if cb == nil {
		return nil, nil
	}
	if cb.Encrypt == nil || cb.Decrypt == nil {
		return nil, fmt.Errorf("both PVE base PKE callbacks must be provided")
	}
	ptr := C.malloc(1)
	if ptr == nil {
		return nil, fmt.Errorf("failed to allocate PVE base PKE context")
	}
	pveBasePKEMap.Store(ptr, cb)
	return ptr, nil
}

func unregisterPVEBasePKECallbacks(ptr unsafe.Pointer) {
	if ptr == nil {
		return
	}
	pveBasePKEMap.Delete(ptr)
	C.free(ptr)
}

func getPVEBasePKECallbacks(ptr unsafe.Pointer) (*PVEBasePKECallbacks, bool) {
	value, ok := pveBasePKEMap.Load(ptr)
	if !ok {
		return nil, false
	}
	cb, ok := value.(*PVEBasePKECallbacks)
	return cb, ok
}

func registerPVERSAOAEPHSMDecapCallbacks(cb *PVERSAOAEPHSMDecapCallbacks) (unsafe.Pointer, error) {
	if cb == nil {
		return nil, nil
	}
	if cb.Decap == nil {
		return nil, fmt.Errorf("RSA-OAEP HSM decap callback must be provided")
	}
	ptr := C.malloc(1)
	if ptr == nil {
		return nil, fmt.Errorf("failed to allocate RSA-OAEP HSM callback context")
	}
	pveRSAOAEPHSMDecapMap.Store(ptr, cb)
	return ptr, nil
}

func unregisterPVERSAOAEPHSMDecapCallbacks(ptr unsafe.Pointer) {
	if ptr == nil {
		return
	}
	pveRSAOAEPHSMDecapMap.Delete(ptr)
	C.free(ptr)
}

func getPVERSAOAEPHSMDecapCallbacks(ptr unsafe.Pointer) (*PVERSAOAEPHSMDecapCallbacks, bool) {
	value, ok := pveRSAOAEPHSMDecapMap.Load(ptr)
	if !ok {
		return nil, false
	}
	cb, ok := value.(*PVERSAOAEPHSMDecapCallbacks)
	return cb, ok
}

func registerPVEECIESP256HSMECDHCallbacks(cb *PVEECIESP256HSMECDHCallbacks) (unsafe.Pointer, error) {
	if cb == nil {
		return nil, nil
	}
	if cb.ECDH == nil {
		return nil, fmt.Errorf("ECIES P256 HSM ECDH callback must be provided")
	}
	ptr := C.malloc(1)
	if ptr == nil {
		return nil, fmt.Errorf("failed to allocate ECIES P256 HSM callback context")
	}
	pveECIESP256HSMECDHMap.Store(ptr, cb)
	return ptr, nil
}

func unregisterPVEECIESP256HSMECDHCallbacks(ptr unsafe.Pointer) {
	if ptr == nil {
		return
	}
	pveECIESP256HSMECDHMap.Delete(ptr)
	C.free(ptr)
}

func getPVEECIESP256HSMECDHCallbacks(ptr unsafe.Pointer) (*PVEECIESP256HSMECDHCallbacks, bool) {
	value, ok := pveECIESP256HSMECDHMap.Load(ptr)
	if !ok {
		return nil, false
	}
	cb, ok := value.(*PVEECIESP256HSMECDHCallbacks)
	return cb, ok
}

func registerPVEBaseKEMCallbacks(cb *PVEBaseKEMCallbacks) (unsafe.Pointer, error) {
	if cb == nil {
		return nil, nil
	}
	if cb.Encap == nil || cb.Decap == nil {
		return nil, fmt.Errorf("both PVE base KEM callbacks must be provided")
	}
	ptr := C.malloc(1)
	if ptr == nil {
		return nil, fmt.Errorf("failed to allocate PVE base KEM context")
	}
	pveBaseKEMMap.Store(ptr, cb)
	return ptr, nil
}

func unregisterPVEBaseKEMCallbacks(ptr unsafe.Pointer) {
	if ptr == nil {
		return
	}
	pveBaseKEMMap.Delete(ptr)
	C.free(ptr)
}

func getPVEBaseKEMCallbacks(ptr unsafe.Pointer) (*PVEBaseKEMCallbacks, bool) {
	value, ok := pveBaseKEMMap.Load(ptr)
	if !ok {
		return nil, false
	}
	cb, ok := value.(*PVEBaseKEMCallbacks)
	return cb, ok
}

type ErrorCode int32

const (
	ErrorCodeSuccess        ErrorCode = ErrorCode(C.CBMPC_SUCCESS)
	ErrorCodeUninitialized  ErrorCode = ErrorCode(C.CBMPC_UNINITIALIZED_ERROR)
	ErrorCodeGeneral        ErrorCode = ErrorCode(C.CBMPC_E_GENERAL)
	ErrorCodeBadArgument    ErrorCode = ErrorCode(C.CBMPC_E_BADARG)
	ErrorCodeFormat         ErrorCode = ErrorCode(C.CBMPC_E_FORMAT)
	ErrorCodeNotSupported   ErrorCode = ErrorCode(C.CBMPC_E_NOT_SUPPORTED)
	ErrorCodeNotFound       ErrorCode = ErrorCode(C.CBMPC_E_NOT_FOUND)
	ErrorCodeInsufficient   ErrorCode = ErrorCode(C.CBMPC_E_INSUFFICIENT)
	ErrorCodeRange          ErrorCode = ErrorCode(C.CBMPC_E_RANGE)
	ErrorCodeNetworkGeneral ErrorCode = ErrorCode(C.CBMPC_E_NET_GENERAL)
	ErrorCodeCrypto         ErrorCode = ErrorCode(C.CBMPC_E_CRYPTO)
	ErrorCodeECDSA2PBitLeak ErrorCode = ErrorCode(C.CBMPC_E_ECDSA_2P_BIT_LEAK)
)

type StatusError struct {
	Op      string
	Code    ErrorCode
	Message string
}

func (e *StatusError) Error() string {
	return fmt.Sprintf("%s failed: %s (code=%d)", e.Op, e.Message, int32(e.Code))
}

func codeError(op string, code C.cbmpc_error_t) error {
	if code == C.CBMPC_SUCCESS {
		return nil
	}

	msg := "cbmpc error"
	switch code {
	case C.CBMPC_UNINITIALIZED_ERROR:
		msg = "uninitialized error"
	case C.CBMPC_E_GENERAL:
		msg = "general failure"
	case C.CBMPC_E_BADARG:
		msg = "bad argument"
	case C.CBMPC_E_FORMAT:
		msg = "invalid format"
	case C.CBMPC_E_NOT_SUPPORTED:
		msg = "not supported"
	case C.CBMPC_E_NOT_FOUND:
		msg = "not found"
	case C.CBMPC_E_INSUFFICIENT:
		msg = "insufficient memory/resources"
	case C.CBMPC_E_RANGE:
		msg = "range error"
	case C.CBMPC_E_NET_GENERAL:
		msg = "network error"
	case C.CBMPC_E_CRYPTO:
		msg = "crypto failure"
	case C.CBMPC_E_ECDSA_2P_BIT_LEAK:
		msg = "ecdsa 2p bit leak error"
	}

	return &StatusError{
		Op:      op,
		Code:    ErrorCode(code),
		Message: msg,
	}
}

func bytesToOwnedCmem(data []byte) (C.cmem_t, func(), error) {
	mem := C.cmem_t{data: nil, size: 0}
	if len(data) == 0 {
		return mem, func() {}, nil
	}
	if len(data) > math.MaxInt32 {
		return C.cmem_t{}, nil, fmt.Errorf("input too large: %d bytes", len(data))
	}
	buf := C.cbmpc_malloc(C.size_t(len(data)))
	if buf == nil {
		return C.cmem_t{}, nil, fmt.Errorf("failed to allocate %d bytes for cmem input", len(data))
	}
	dst := unsafe.Slice((*byte)(buf), len(data))
	copy(dst, data)
	mem.data = (*C.uint8_t)(buf)
	mem.size = C.int(len(data))
	cleanup := func() {
		C.cbmpc_cmem_free(mem)
	}
	return mem, cleanup, nil
}

func bytesToOwnedCmems(values ...[]byte) ([]C.cmem_t, func(), error) {
	if len(values) == 0 {
		return nil, func() {}, nil
	}
	mems := make([]C.cmem_t, len(values))
	cleanups := make([]func(), len(values))
	for i, value := range values {
		mem, cleanup, err := bytesToOwnedCmem(value)
		if err != nil {
			for j := i - 1; j >= 0; j-- {
				if cleanups[j] != nil {
					cleanups[j]()
				}
			}
			return nil, nil, err
		}
		mems[i] = mem
		cleanups[i] = cleanup
	}
	return mems, func() {
		for i := len(cleanups) - 1; i >= 0; i-- {
			if cleanups[i] != nil {
				cleanups[i]()
			}
		}
	}, nil
}

func cmemToBytes(mem C.cmem_t) []byte {
	if mem.data == nil || mem.size <= 0 {
		return nil
	}
	return C.GoBytes(unsafe.Pointer(mem.data), C.int(mem.size))
}

func freeCmem(mem C.cmem_t) {
	if mem.data != nil {
		C.cbmpc_cmem_free(mem)
	}
}

func cmemToBytesAndFree(mem C.cmem_t) []byte {
	out := cmemToBytes(mem)
	freeCmem(mem)
	return out
}

func cmemsToByteSlices(mems C.cmems_t) ([][]byte, error) {
	count := int(mems.count)
	if count == 0 {
		return [][]byte{}, nil
	}
	if count < 0 {
		return nil, fmt.Errorf("invalid cmems count: %d", count)
	}
	if mems.sizes == nil {
		return nil, fmt.Errorf("invalid cmems: sizes is nil with count=%d", count)
	}

	sizes := unsafe.Slice((*C.int)(unsafe.Pointer(mems.sizes)), count)
	out := make([][]byte, count)
	offset := 0

	for i := 0; i < count; i++ {
		size := int(sizes[i])
		if size < 0 {
			return nil, fmt.Errorf("invalid negative cmems size at index %d", i)
		}
		if size == 0 {
			out[i] = []byte{}
			continue
		}
		if mems.data == nil {
			return nil, fmt.Errorf("invalid cmems: data is nil with non-empty segment")
		}
		if offset > math.MaxInt-size {
			return nil, fmt.Errorf("cmems total size overflow at index %d", i)
		}

		ptr := unsafe.Pointer(uintptr(unsafe.Pointer(mems.data)) + uintptr(offset))
		out[i] = C.GoBytes(ptr, C.int(size))
		offset += size
	}

	return out, nil
}

func freeCmems(mems C.cmems_t) {
	if mems.data != nil || mems.sizes != nil {
		C.cbmpc_cmems_free(mems)
	}
}

func cmemsToByteSlicesAndFree(mems C.cmems_t) ([][]byte, error) {
	out, err := cmemsToByteSlices(mems)
	freeCmems(mems)
	return out, err
}

func byteSlicesToOwnedCmems(values [][]byte) (C.cmems_t, func(), error) {
	if len(values) == 0 {
		return C.cmems_t{count: 0, data: nil, sizes: nil}, func() {}, nil
	}

	if len(values) > math.MaxInt32 {
		return C.cmems_t{}, nil, fmt.Errorf("too many slices for cmems: %d", len(values))
	}
	total := 0
	for i, value := range values {
		if len(value) > math.MaxInt32 {
			return C.cmems_t{}, nil, fmt.Errorf("slice %d too large for cmems: %d bytes", i, len(value))
		}
		if total > math.MaxInt-len(value) {
			return C.cmems_t{}, nil, fmt.Errorf("cmems total size overflows int")
		}
		total += len(value)
	}

	sizesPtr := C.cbmpc_malloc(C.size_t(len(values)) * C.size_t(C.sizeof_int))
	if sizesPtr == nil {
		return C.cmems_t{}, nil, fmt.Errorf("failed to allocate sizes for cmems input")
	}

	sizesSlice := unsafe.Slice((*C.int)(unsafe.Pointer(sizesPtr)), len(values))
	for i, value := range values {
		sizesSlice[i] = C.int(len(value))
	}

	var dataPtr unsafe.Pointer
	if total > 0 {
		dataPtr = C.cbmpc_malloc(C.size_t(total))
		if dataPtr == nil {
			C.cbmpc_free(sizesPtr)
			return C.cmems_t{}, nil, fmt.Errorf("failed to allocate data for cmems input")
		}
	}

	offset := 0
	for _, value := range values {
		if len(value) == 0 {
			continue
		}
		dst := unsafe.Pointer(uintptr(dataPtr) + uintptr(offset))
		copy(unsafe.Slice((*byte)(dst), len(value)), value)
		offset += len(value)
	}

	mems := C.cmems_t{
		count: C.int(len(values)),
		data:  (*C.uint8_t)(dataPtr),
		sizes: (*C.int)(unsafe.Pointer(sizesPtr)),
	}

	cleanup := func() {
		freeCmems(mems)
	}
	return mems, cleanup, nil
}

func byteSlicesToOwnedCmemArray(values [][]byte) (unsafe.Pointer, int, func(), error) {
	count := len(values)
	if count == 0 {
		return nil, 0, func() {}, nil
	}

	if count > math.MaxInt32 {
		return nil, 0, nil, fmt.Errorf("too many slices for cmem array: %d", count)
	}
	for i, value := range values {
		if len(value) > math.MaxInt32 {
			return nil, 0, nil, fmt.Errorf("slice %d too large for cmem array: %d bytes", i, len(value))
		}
	}

	arrayPtr := C.malloc(C.size_t(count) * C.size_t(C.sizeof_cmem_t))
	if arrayPtr == nil {
		return nil, 0, nil, fmt.Errorf("failed to allocate cmem array")
	}
	array := unsafe.Slice((*C.cmem_t)(arrayPtr), count)

	for i, value := range values {
		mem := C.cmem_t{data: nil, size: 0}
		if len(value) > 0 {
			data := C.cbmpc_malloc(C.size_t(len(value)))
			if data == nil {
				for j := 0; j < i; j++ {
					freeCmem(array[j])
				}
				C.free(arrayPtr)
				return nil, 0, nil, fmt.Errorf("failed to allocate cmem array data")
			}
			copy(unsafe.Slice((*byte)(data), len(value)), value)
			mem.data = (*C.uint8_t)(data)
			mem.size = C.int(len(value))
		}
		array[i] = mem
	}

	cleanup := func() {
		array := unsafe.Slice((*C.cmem_t)(arrayPtr), count)
		for i := range array {
			freeCmem(array[i])
		}
		C.free(arrayPtr)
	}

	return arrayPtr, count, cleanup, nil
}

func createCStringArray(values []string) (unsafe.Pointer, []*C.char, error) {
	if len(values) == 0 {
		return nil, nil, nil
	}
	if len(values) > math.MaxInt32 {
		return nil, nil, fmt.Errorf("too many strings for C string array: %d", len(values))
	}

	arrayPtr := C.malloc(C.size_t(len(values)) * C.size_t(unsafe.Sizeof(uintptr(0))))
	if arrayPtr == nil {
		return nil, nil, fmt.Errorf("failed to allocate C string array")
	}

	slots := unsafe.Slice((*unsafe.Pointer)(arrayPtr), len(values))
	cStrings := make([]*C.char, len(values))
	for i, value := range values {
		if value == "" {
			for j := 0; j < i; j++ {
				C.free(unsafe.Pointer(cStrings[j]))
			}
			C.free(arrayPtr)
			return nil, nil, fmt.Errorf("party name at index %d cannot be empty", i)
		}
		if strings.IndexByte(value, 0) != -1 {
			for j := 0; j < i; j++ {
				C.free(unsafe.Pointer(cStrings[j]))
			}
			C.free(arrayPtr)
			return nil, nil, fmt.Errorf("party name at index %d contains NUL byte", i)
		}
		cString := C.CString(value)
		if cString == nil {
			for j := 0; j < i; j++ {
				C.free(unsafe.Pointer(cStrings[j]))
			}
			C.free(arrayPtr)
			return nil, nil, fmt.Errorf("failed to allocate C string for party name at index %d", i)
		}
		cStrings[i] = cString
		slots[i] = unsafe.Pointer(cString)
	}

	return arrayPtr, cStrings, nil
}

func freeCStringArray(arrayPtr unsafe.Pointer, cStrings []*C.char) {
	for _, cString := range cStrings {
		if cString != nil {
			C.free(unsafe.Pointer(cString))
		}
	}
	if arrayPtr != nil {
		C.free(arrayPtr)
	}
}

func with2PCJob(ctx context.Context, job Job2P, fn func(*C.cbmpc_2pc_job_t) error) error {
	if err := validateJob2P(job); err != nil {
		return err
	}

	ctxPtr, tc, err := registerTransport(ctx, job.Transport)
	if err != nil {
		return err
	}
	defer unregisterTransport(ctxPtr)

	cP1Name := C.CString(job.P1Name)
	if cP1Name == nil {
		return fmt.Errorf("failed to allocate C string for p1Name")
	}
	defer C.free(unsafe.Pointer(cP1Name))
	cP2Name := C.CString(job.P2Name)
	if cP2Name == nil {
		return fmt.Errorf("failed to allocate C string for p2Name")
	}
	defer C.free(unsafe.Pointer(cP2Name))

	transportPtr := (*C.cbmpc_transport_t)(C.malloc(C.size_t(unsafe.Sizeof(C.cbmpc_transport_t{}))))
	if transportPtr == nil {
		return fmt.Errorf("failed to allocate cbmpc transport")
	}
	defer C.free(unsafe.Pointer(transportPtr))

	*transportPtr = C.cbmpc_transport_t{
		ctx:         ctxPtr,
		send:        (C.cbmpc_transport_send_fn)(C.goTransportSend),
		receive:     (C.cbmpc_transport_receive_fn)(C.goTransportReceive),
		receive_all: (C.cbmpc_transport_receive_all_fn)(C.goTransportReceiveAll),
		free:        (C.cbmpc_transport_free_fn)(C.goTransportFree),
	}

	jobPtr := (*C.cbmpc_2pc_job_t)(C.malloc(C.size_t(unsafe.Sizeof(C.cbmpc_2pc_job_t{}))))
	if jobPtr == nil {
		return fmt.Errorf("failed to allocate cbmpc 2pc job")
	}
	defer C.free(unsafe.Pointer(jobPtr))

	*jobPtr = C.cbmpc_2pc_job_t{
		self:      C.cbmpc_2pc_party_t(job.Self),
		p1_name:   cP1Name,
		p2_name:   cP2Name,
		transport: transportPtr,
	}

	err = fn(jobPtr)
	if err != nil {
		if cause := tc.takeTransportErr(); cause != nil {
			return &TransportError{Status: err, Cause: cause}
		}
		return err
	}
	_ = tc.takeTransportErr()
	return nil
}

func withMPJob(ctx context.Context, job JobMP, fn func(*C.cbmpc_mp_job_t) error) error {
	if err := validateJobMP(job); err != nil {
		return err
	}

	ctxPtr, tc, err := registerTransport(ctx, job.Transport)
	if err != nil {
		return err
	}
	defer unregisterTransport(ctxPtr)

	namesArray, cNames, err := createCStringArray(job.PartyNames)
	if err != nil {
		return err
	}
	defer freeCStringArray(namesArray, cNames)

	transportPtr := (*C.cbmpc_transport_t)(C.malloc(C.size_t(unsafe.Sizeof(C.cbmpc_transport_t{}))))
	if transportPtr == nil {
		return fmt.Errorf("failed to allocate cbmpc transport")
	}
	defer C.free(unsafe.Pointer(transportPtr))

	*transportPtr = C.cbmpc_transport_t{
		ctx:         ctxPtr,
		send:        (C.cbmpc_transport_send_fn)(C.goTransportSend),
		receive:     (C.cbmpc_transport_receive_fn)(C.goTransportReceive),
		receive_all: (C.cbmpc_transport_receive_all_fn)(C.goTransportReceiveAll),
		free:        (C.cbmpc_transport_free_fn)(C.goTransportFree),
	}

	jobPtr := (*C.cbmpc_mp_job_t)(C.malloc(C.size_t(unsafe.Sizeof(C.cbmpc_mp_job_t{}))))
	if jobPtr == nil {
		return fmt.Errorf("failed to allocate cbmpc mp job")
	}
	defer C.free(unsafe.Pointer(jobPtr))

	*jobPtr = C.cbmpc_mp_job_t{
		self:              C.int32_t(job.Self),
		party_names:       (**C.char)(namesArray),
		party_names_count: C.int(len(job.PartyNames)),
		transport:         transportPtr,
	}

	err = fn(jobPtr)
	if err != nil {
		if cause := tc.takeTransportErr(); cause != nil {
			return &TransportError{Status: err, Cause: cause}
		}
		return err
	}
	_ = tc.takeTransportErr()
	return nil
}

type compiledAccessStructure struct {
	cStruct      C.cbmpc_access_structure_t
	nodesPtr     unsafe.Pointer
	childIdxPtr  unsafe.Pointer
	leafNamePtrs []*C.char
}

func validateAccessStructure(access *AccessStructure) error {
	if access == nil {
		return fmt.Errorf("access structure cannot be nil")
	}
	if len(access.Nodes) == 0 {
		return fmt.Errorf("access structure must contain at least one node")
	}
	if len(access.Nodes) > math.MaxInt32 {
		return fmt.Errorf("access structure has too many nodes: %d", len(access.Nodes))
	}
	if access.Root < 0 || access.Root >= int32(len(access.Nodes)) {
		return fmt.Errorf("access structure root index %d out of range [0, %d)", access.Root, len(access.Nodes))
	}

	for i, node := range access.Nodes {
		switch node.Type {
		case AccessNodeLeaf:
			if node.LeafName == "" {
				return fmt.Errorf("access structure leaf node %d must have a non-empty leaf name", i)
			}
			if strings.IndexByte(node.LeafName, 0) != -1 {
				return fmt.Errorf("access structure leaf node %d leaf name contains NUL byte", i)
			}
			if node.ThresholdK != 0 {
				return fmt.Errorf("access structure leaf node %d must have threshold_k=0", i)
			}
			if len(node.Children) != 0 {
				return fmt.Errorf("access structure leaf node %d must not have children", i)
			}
		case AccessNodeAnd, AccessNodeOr:
			if node.LeafName != "" {
				return fmt.Errorf("access structure AND/OR node %d must not have a leaf name", i)
			}
			if node.ThresholdK != 0 {
				return fmt.Errorf("access structure AND/OR node %d must have threshold_k=0", i)
			}
			if len(node.Children) == 0 {
				return fmt.Errorf("access structure AND/OR node %d must have at least one child", i)
			}
		case AccessNodeThreshold:
			if node.LeafName != "" {
				return fmt.Errorf("access structure THRESHOLD node %d must not have a leaf name", i)
			}
			if len(node.Children) == 0 {
				return fmt.Errorf("access structure THRESHOLD node %d must have at least one child", i)
			}
			if node.ThresholdK <= 0 {
				return fmt.Errorf("access structure THRESHOLD node %d must have threshold_k >= 1", i)
			}
			if int(node.ThresholdK) > len(node.Children) {
				return fmt.Errorf("access structure THRESHOLD node %d has threshold_k=%d > child_count=%d", i, node.ThresholdK, len(node.Children))
			}
		default:
			return fmt.Errorf("access structure node %d has invalid type %d", i, node.Type)
		}

		for j, child := range node.Children {
			if child < 0 || child >= int32(len(access.Nodes)) {
				return fmt.Errorf("access structure node %d child[%d]=%d out of range [0, %d)", i, j, child, len(access.Nodes))
			}
		}
	}

	if access.Nodes[access.Root].Type == AccessNodeLeaf {
		return fmt.Errorf("access structure root node must not be a leaf")
	}

	// Enforce the C API tree requirements:
	// - no cycles
	// - no node reuse (DAGs are rejected)
	// - all nodes must be reachable from the root
	visited := make([]uint8, len(access.Nodes)) // 0=unvisited, 1=visiting, 2=done
	var dfs func(int32) error
	dfs = func(idx int32) error {
		i := int(idx)
		switch visited[i] {
		case 1:
			return fmt.Errorf("access structure contains a cycle involving node %d", i)
		case 2:
			return fmt.Errorf("access structure is not a tree: node %d is reused", i)
		}
		visited[i] = 1
		for _, child := range access.Nodes[i].Children {
			if err := dfs(child); err != nil {
				return err
			}
		}
		visited[i] = 2
		return nil
	}
	if err := dfs(access.Root); err != nil {
		return err
	}
	for i, state := range visited {
		if state == 0 {
			return fmt.Errorf("access structure contains unreachable node %d", i)
		}
	}
	return nil
}

func compileAccessStructure(access *AccessStructure) (*compiledAccessStructure, error) {
	if err := validateAccessStructure(access); err != nil {
		return nil, err
	}

	flatChildrenCount := 0
	for i, node := range access.Nodes {
		if len(node.Children) > 0 && flatChildrenCount > math.MaxInt32-len(node.Children) {
			return nil, fmt.Errorf("access structure child index list too large at node %d", i)
		}
		flatChildrenCount += len(node.Children)
	}

	nodesPtr := C.malloc(C.size_t(len(access.Nodes)) * C.size_t(C.sizeof_cbmpc_access_structure_node_t))
	if nodesPtr == nil {
		return nil, fmt.Errorf("failed to allocate access-structure nodes")
	}

	compiled := &compiledAccessStructure{
		nodesPtr: nodesPtr,
	}
	if flatChildrenCount > 0 {
		compiled.childIdxPtr = C.malloc(C.size_t(flatChildrenCount) * C.size_t(C.sizeof_int32_t))
		if compiled.childIdxPtr == nil {
			compiled.free()
			return nil, fmt.Errorf("failed to allocate access-structure child indices")
		}
	}

	nodes := unsafe.Slice((*C.cbmpc_access_structure_node_t)(nodesPtr), len(access.Nodes))
	var childIdx []C.int32_t
	if flatChildrenCount > 0 {
		childIdx = unsafe.Slice((*C.int32_t)(compiled.childIdxPtr), flatChildrenCount)
	}

	leafNames := make([]*C.char, 0, len(access.Nodes))
	childOffset := 0

	for i, node := range access.Nodes {
		cNode := C.cbmpc_access_structure_node_t{}
		cNode._type = C.cbmpc_access_structure_node_type_t(node.Type)
		cNode.leaf_name = nil
		cNode.threshold_k = C.int32_t(node.ThresholdK)
		cNode.child_indices_offset = C.int32_t(childOffset)
		cNode.child_indices_count = C.int32_t(len(node.Children))

		if node.Type == AccessNodeLeaf {
			cLeafName := C.CString(node.LeafName)
			if cLeafName == nil {
				compiled.leafNamePtrs = leafNames
				compiled.free()
				return nil, fmt.Errorf("failed to allocate leaf name for node %d", i)
			}
			leafNames = append(leafNames, cLeafName)
			cNode.leaf_name = cLeafName
		}

		for j, childIndex := range node.Children {
			childIdx[childOffset+j] = C.int32_t(childIndex)
		}
		childOffset += len(node.Children)
		nodes[i] = cNode
	}

	compiled.cStruct = C.cbmpc_access_structure_t{
		nodes:               (*C.cbmpc_access_structure_node_t)(nodesPtr),
		nodes_count:         C.int32_t(len(access.Nodes)),
		child_indices:       (*C.int32_t)(compiled.childIdxPtr),
		child_indices_count: C.int32_t(flatChildrenCount),
		root_index:          C.int32_t(access.Root),
	}
	compiled.leafNamePtrs = leafNames
	return compiled, nil
}

func (compiled *compiledAccessStructure) free() {
	if compiled == nil {
		return
	}
	for _, leafNamePtr := range compiled.leafNamePtrs {
		if leafNamePtr != nil {
			C.free(unsafe.Pointer(leafNamePtr))
		}
	}
	if compiled.childIdxPtr != nil {
		C.free(compiled.childIdxPtr)
	}
	if compiled.nodesPtr != nil {
		C.free(compiled.nodesPtr)
	}
}

const (
	maxTransportMessageSize         = 64 * 1024 * 1024  // 64 MiB per message
	maxTransportReceiveAllTotalSize = 256 * 1024 * 1024 // 256 MiB across all messages
)

func zeroCmemOutput(out *C.cmem_t) {
	if out == nil {
		return
	}
	out.data = nil
	out.size = 0
}

func zeroCmemsOutput(out *C.cmems_t) {
	if out == nil {
		return
	}
	out.count = 0
	out.data = nil
	out.sizes = nil
}

func freeOwnedCallbackAlloc(ptr *unsafe.Pointer) {
	if ptr == nil || *ptr == nil {
		return
	}
	C.cbmpc_free(*ptr)
	*ptr = nil
}

func freeAndZeroCmemOutput(out *C.cmem_t) {
	if out == nil {
		return
	}
	if out.data != nil {
		freeCmem(*out)
	}
	zeroCmemOutput(out)
}

// Panics must never cross the CGO boundary. Convert them into stable C error
// codes and leave any callback outputs in a zeroed state.
func recoverCallbackPanic(code *C.cbmpc_error_t, panicCode C.cbmpc_error_t, cleanup func()) {
	if recover() == nil {
		return
	}
	if cleanup != nil {
		cleanup()
	}
	*code = panicCode
}

const (
	testCBMPCEGeneral    = int32(C.CBMPC_E_GENERAL)
	testCBMPCENetGeneral = int32(C.CBMPC_E_NET_GENERAL)
	testCBMPCERange      = int32(C.CBMPC_E_RANGE)
)

var (
	testHookAfterTransportReceiveAlloc    func()
	testHookAfterTransportReceiveAllAlloc func()
)

func setOwnedCmemOutput(out *C.cmem_t, data []byte) C.cbmpc_error_t {
	if out == nil {
		return C.CBMPC_E_BADARG
	}
	zeroCmemOutput(out)
	if len(data) == 0 {
		return C.CBMPC_SUCCESS
	}
	if len(data) > math.MaxInt32 {
		return C.CBMPC_E_RANGE
	}
	buf := C.cbmpc_malloc(C.size_t(len(data)))
	if buf == nil {
		return C.CBMPC_E_INSUFFICIENT
	}
	copy(unsafe.Slice((*byte)(buf), len(data)), data)
	out.data = (*C.uint8_t)(buf)
	out.size = C.int(len(data))
	return C.CBMPC_SUCCESS
}

func validatePartyCount(partyCount int) error {
	if partyCount < 0 || partyCount > math.MaxInt32 {
		return fmt.Errorf("party count %d out of supported range [0, %d]", partyCount, math.MaxInt32)
	}
	return nil
}

func validatePartyName(name string, value string) error {
	if value == "" {
		return fmt.Errorf("%s cannot be empty", name)
	}
	if strings.IndexByte(value, 0) != -1 {
		return fmt.Errorf("%s cannot contain NUL bytes", name)
	}
	if isSequentialPartyLabel(value) {
		return fmt.Errorf("%s=%q uses a placeholder label; use a stable globally unique identifier such as a public key fingerprint or UUID", name, value)
	}
	if net.ParseIP(value) != nil {
		return fmt.Errorf("%s=%q must not be a bare IP address; use a stable globally unique identifier such as a public key fingerprint or UUID", name, value)
	}
	if host, port, err := net.SplitHostPort(value); err == nil && host != "" && port != "" {
		return fmt.Errorf("%s=%q must not be a host:port endpoint; use a stable globally unique identifier such as a public key fingerprint or UUID", name, value)
	}
	return nil
}

func isSequentialPartyLabel(value string) bool {
	lower := strings.ToLower(value)
	if !strings.HasPrefix(lower, "party") || len(lower) == len("party") {
		return false
	}
	for _, ch := range lower[len("party"):] {
		if ch < '0' || ch > '9' {
			return false
		}
	}
	return true
}

func validateDistinctPartyNames(name string, partyNames []string) error {
	seen := make(map[string]struct{}, len(partyNames))
	for i, partyName := range partyNames {
		if err := validatePartyName(fmt.Sprintf("%s[%d]", name, i), partyName); err != nil {
			return err
		}
		if _, ok := seen[partyName]; ok {
			return fmt.Errorf("%s[%d]=%q duplicates another party name", name, i, partyName)
		}
		seen[partyName] = struct{}{}
	}
	return nil
}

func validatePartyNameSubset(name string, subset []string, roster []string) error {
	if err := validateDistinctPartyNames(name, subset); err != nil {
		return err
	}
	rosterSet := make(map[string]struct{}, len(roster))
	for _, partyName := range roster {
		rosterSet[partyName] = struct{}{}
	}
	for i, partyName := range subset {
		if _, ok := rosterSet[partyName]; !ok {
			return fmt.Errorf("%s[%d]=%q is not present in job.PartyNames", name, i, partyName)
		}
	}
	return nil
}

func validateJob2P(job Job2P) error {
	if job.Transport == nil {
		return fmt.Errorf("job transport cannot be nil")
	}
	if job.Self != TwoPartyP1 && job.Self != TwoPartyP2 {
		return fmt.Errorf("invalid two-party self index %d", job.Self)
	}
	if err := validatePartyName("job.P1Name", job.P1Name); err != nil {
		return err
	}
	if err := validatePartyName("job.P2Name", job.P2Name); err != nil {
		return err
	}
	if job.P1Name == job.P2Name {
		return fmt.Errorf("job.P1Name and job.P2Name must be distinct")
	}
	return nil
}

func validateJobMP(job JobMP) error {
	if job.Transport == nil {
		return fmt.Errorf("job transport cannot be nil")
	}
	if err := validatePartyCount(len(job.PartyNames)); err != nil {
		return err
	}
	if len(job.PartyNames) < 2 {
		return fmt.Errorf("job.PartyNames must contain at least 2 entries, got %d", len(job.PartyNames))
	}
	if len(job.PartyNames) > 64 {
		return fmt.Errorf("job.PartyNames exceeds maximum size 64: got %d", len(job.PartyNames))
	}
	if err := validatePartyIndex("job.Self", job.Self, len(job.PartyNames)); err != nil {
		return err
	}
	return validateDistinctPartyNames("job.PartyNames", job.PartyNames)
}

func validatePartyIndex(name string, idx int, partyCount int) error {
	if err := validatePartyCount(partyCount); err != nil {
		return err
	}
	if idx < 0 || idx >= partyCount {
		return fmt.Errorf("%s %d out of range [0, %d)", name, idx, partyCount)
	}
	return nil
}

func validatePartyIndices(name string, indices []int, partyCount int) error {
	if err := validatePartyCount(partyCount); err != nil {
		return err
	}
	for i, idx := range indices {
		if err := validatePartyIndex(fmt.Sprintf("%s[%d]", name, i), idx, partyCount); err != nil {
			return err
		}
	}
	return nil
}

func validateEqualLengths(leftName string, leftLen int, rightName string, rightLen int) error {
	if leftLen != rightLen {
		return fmt.Errorf("%s and %s must have the same length", leftName, rightName)
	}
	return nil
}

func testGoTransportSendCode(ctx unsafe.Pointer) int32 {
	return int32(goTransportSend(ctx, 1, nil, 0))
}

func testGoTransportReceiveResult(ctx unsafe.Pointer) (int32, bool) {
	var out C.cmem_t
	code := goTransportReceive(ctx, 1, &out)
	zeroed := out.data == nil && out.size == 0
	if out.data != nil {
		C.cbmpc_free(unsafe.Pointer(out.data))
	}
	return int32(code), zeroed
}

func testGoTransportReceiveAllResult(ctx unsafe.Pointer) (int32, bool) {
	var out C.cmems_t
	code := goTransportReceiveAll(ctx, nil, 0, &out)
	zeroed := out.count == 0 && out.data == nil && out.sizes == nil
	if out.data != nil {
		C.cbmpc_free(unsafe.Pointer(out.data))
	}
	if out.sizes != nil {
		C.cbmpc_free(unsafe.Pointer(out.sizes))
	}
	return int32(code), zeroed
}

func testGoTransportReceiveAllWithSendersResult(ctx unsafe.Pointer, senders []int32) (int32, bool) {
	var out C.cmems_t
	if len(senders) == 0 {
		return testGoTransportReceiveAllResult(ctx)
	}
	cSenders := make([]C.int32_t, len(senders))
	for i, sender := range senders {
		cSenders[i] = C.int32_t(sender)
	}
	code := goTransportReceiveAll(ctx, &cSenders[0], C.int(len(cSenders)), &out)
	zeroed := out.count == 0 && out.data == nil && out.sizes == nil
	if out.data != nil {
		C.cbmpc_free(unsafe.Pointer(out.data))
	}
	if out.sizes != nil {
		C.cbmpc_free(unsafe.Pointer(out.sizes))
	}
	return int32(code), zeroed
}

func testGoPVEBasePKEEncryptResult(ctx unsafe.Pointer) (int32, bool) {
	var out C.cmem_t
	code := goPVEBasePKEEncrypt(ctx, C.cmem_t{}, C.cmem_t{}, C.cmem_t{}, C.cmem_t{}, &out)
	zeroed := out.data == nil && out.size == 0
	freeCmem(out)
	return int32(code), zeroed
}

func testGoPVEBasePKEDecryptResult(ctx unsafe.Pointer) (int32, bool) {
	var out C.cmem_t
	code := goPVEBasePKEDecrypt(ctx, C.cmem_t{}, C.cmem_t{}, C.cmem_t{}, &out)
	zeroed := out.data == nil && out.size == 0
	freeCmem(out)
	return int32(code), zeroed
}

func testGoPVERSAOAEPHSMDecapResult(ctx unsafe.Pointer) (int32, bool) {
	var out C.cmem_t
	code := goPVERSAOAEPHSMDecap(ctx, C.cmem_t{}, C.cmem_t{}, &out)
	zeroed := out.data == nil && out.size == 0
	freeCmem(out)
	return int32(code), zeroed
}

func testGoPVEECIESP256HSMECDHResult(ctx unsafe.Pointer) (int32, bool) {
	var out C.cmem_t
	code := goPVEECIESP256HSMECDH(ctx, C.cmem_t{}, C.cmem_t{}, &out)
	zeroed := out.data == nil && out.size == 0
	freeCmem(out)
	return int32(code), zeroed
}

func testGoPVEKEMEncapResult(ctx unsafe.Pointer) (int32, bool, bool) {
	var outCT C.cmem_t
	var outSS C.cmem_t
	code := goPVEKEMEncap(ctx, C.cmem_t{}, C.cmem_t{}, &outCT, &outSS)
	zeroCT := outCT.data == nil && outCT.size == 0
	zeroSS := outSS.data == nil && outSS.size == 0
	freeCmem(outCT)
	freeCmem(outSS)
	return int32(code), zeroCT, zeroSS
}

func testGoPVEKEMDecapResult(ctx unsafe.Pointer) (int32, bool) {
	var out C.cmem_t
	code := goPVEKEMDecap(ctx, C.cmem_t{}, C.cmem_t{}, &out)
	zeroed := out.data == nil && out.size == 0
	freeCmem(out)
	return int32(code), zeroed
}

//export goTransportSend
func goTransportSend(ctx unsafe.Pointer, receiver C.int32_t, data *C.uint8_t, size C.int) (code C.cbmpc_error_t) {
	code = C.CBMPC_SUCCESS
	defer recoverCallbackPanic(&code, C.CBMPC_E_NET_GENERAL, nil)

	if size < 0 {
		return C.CBMPC_E_BADARG
	}
	if size > C.int(maxTransportMessageSize) {
		return C.CBMPC_E_RANGE
	}
	tc, ok := getTransport(ctx)
	if !ok {
		return C.CBMPC_E_BADARG
	}
	if size > 0 && data == nil {
		return C.CBMPC_E_BADARG
	}

	var message []byte
	if size > 0 {
		message = C.GoBytes(unsafe.Pointer(data), size)
	}
	if err := tc.transport.Send(tc.ctx, int(receiver), message); err != nil {
		tc.storeTransportErr(err)
		return C.CBMPC_E_NET_GENERAL
	}
	return C.CBMPC_SUCCESS
}

//export goTransportReceive
func goTransportReceive(ctx unsafe.Pointer, sender C.int32_t, outMsg *C.cmem_t) (code C.cbmpc_error_t) {
	code = C.CBMPC_SUCCESS
	var buffer unsafe.Pointer
	defer recoverCallbackPanic(&code, C.CBMPC_E_NET_GENERAL, func() {
		freeOwnedCallbackAlloc(&buffer)
		zeroCmemOutput(outMsg)
	})

	if outMsg == nil {
		return C.CBMPC_E_BADARG
	}
	zeroCmemOutput(outMsg)

	tc, ok := getTransport(ctx)
	if !ok {
		return C.CBMPC_E_BADARG
	}

	message, err := tc.transport.Receive(tc.ctx, int(sender))
	if err != nil {
		tc.storeTransportErr(err)
		return C.CBMPC_E_NET_GENERAL
	}
	if len(message) > maxTransportMessageSize || len(message) > math.MaxInt32 {
		return C.CBMPC_E_RANGE
	}
	if len(message) == 0 {
		return C.CBMPC_SUCCESS
	}

	buffer = C.cbmpc_malloc(C.size_t(len(message)))
	if buffer == nil {
		return C.CBMPC_E_INSUFFICIENT
	}
	if testHookAfterTransportReceiveAlloc != nil {
		testHookAfterTransportReceiveAlloc()
	}

	copy(unsafe.Slice((*byte)(buffer), len(message)), message)
	outMsg.data = (*C.uint8_t)(buffer)
	outMsg.size = C.int(len(message))
	return C.CBMPC_SUCCESS
}

//export goTransportReceiveAll
func goTransportReceiveAll(ctx unsafe.Pointer, senders *C.int32_t, sendersCount C.int, outMsgs *C.cmems_t) (code C.cbmpc_error_t) {
	code = C.CBMPC_SUCCESS
	var sizesPtr unsafe.Pointer
	var dataPtr unsafe.Pointer
	defer recoverCallbackPanic(&code, C.CBMPC_E_NET_GENERAL, func() {
		freeOwnedCallbackAlloc(&dataPtr)
		freeOwnedCallbackAlloc(&sizesPtr)
		zeroCmemsOutput(outMsgs)
	})

	if outMsgs == nil {
		return C.CBMPC_E_BADARG
	}
	zeroCmemsOutput(outMsgs)

	if sendersCount < 0 {
		return C.CBMPC_E_BADARG
	}
	if sendersCount > 0 && senders == nil {
		return C.CBMPC_E_BADARG
	}

	tc, ok := getTransport(ctx)
	if !ok {
		return C.CBMPC_E_BADARG
	}

	count := int(sendersCount)
	goSenders := make([]int, count)
	if count > 0 {
		sendersSlice := unsafe.Slice((*C.int32_t)(unsafe.Pointer(senders)), count)
		for i := 0; i < count; i++ {
			goSenders[i] = int(sendersSlice[i])
		}
	}

	messages, err := tc.transport.ReceiveAll(tc.ctx, goSenders)
	if err != nil {
		tc.storeTransportErr(err)
		return C.CBMPC_E_NET_GENERAL
	}
	if len(messages) != count {
		return C.CBMPC_E_GENERAL
	}
	if count == 0 {
		return C.CBMPC_SUCCESS
	}

	total := 0
	for _, message := range messages {
		msgLen := len(message)
		if msgLen > maxTransportMessageSize || msgLen > math.MaxInt32 {
			return C.CBMPC_E_RANGE
		}
		if msgLen > 0 && total > math.MaxInt-msgLen {
			return C.CBMPC_E_INSUFFICIENT
		}
		total += msgLen
	}
	if total > maxTransportReceiveAllTotalSize {
		return C.CBMPC_E_RANGE
	}

	sizesPtr = C.cbmpc_malloc(C.size_t(count) * C.size_t(C.sizeof_int))
	if sizesPtr == nil {
		return C.CBMPC_E_INSUFFICIENT
	}

	if total > 0 {
		dataPtr = C.cbmpc_malloc(C.size_t(total))
		if dataPtr == nil {
			C.cbmpc_free(sizesPtr)
			return C.CBMPC_E_INSUFFICIENT
		}
	}
	if testHookAfterTransportReceiveAllAlloc != nil {
		testHookAfterTransportReceiveAllAlloc()
	}

	sizes := unsafe.Slice((*C.int)(unsafe.Pointer(sizesPtr)), count)
	offset := 0
	for i, message := range messages {
		sizes[i] = C.int(len(message))
		if len(message) > 0 {
			dst := unsafe.Pointer(uintptr(dataPtr) + uintptr(offset))
			copy(unsafe.Slice((*byte)(dst), len(message)), message)
			offset += len(message)
		}
	}

	outMsgs.count = C.int(count)
	outMsgs.sizes = (*C.int)(unsafe.Pointer(sizesPtr))
	outMsgs.data = (*C.uint8_t)(dataPtr)
	return C.CBMPC_SUCCESS
}

//export goTransportFree
func goTransportFree(_ unsafe.Pointer, ptr unsafe.Pointer) {
	if ptr != nil {
		C.cbmpc_free(ptr)
	}
}

//export goPVEBasePKEEncrypt
func goPVEBasePKEEncrypt(ctx unsafe.Pointer, ek C.cmem_t, label C.cmem_t, plain C.cmem_t, rho C.cmem_t, outCT *C.cmem_t) (code C.cbmpc_error_t) {
	code = C.CBMPC_SUCCESS
	defer recoverCallbackPanic(&code, C.CBMPC_E_GENERAL, func() {
		freeAndZeroCmemOutput(outCT)
	})

	if outCT == nil {
		return C.CBMPC_E_BADARG
	}
	zeroCmemOutput(outCT)

	cb, ok := getPVEBasePKECallbacks(ctx)
	if !ok || cb == nil || cb.Encrypt == nil {
		return C.CBMPC_E_BADARG
	}

	ct, err := cb.Encrypt(cmemToBytes(ek), cmemToBytes(label), cmemToBytes(plain), cmemToBytes(rho))
	if err != nil {
		return C.CBMPC_E_GENERAL
	}
	return setOwnedCmemOutput(outCT, ct)
}

//export goPVEBasePKEDecrypt
func goPVEBasePKEDecrypt(ctx unsafe.Pointer, dk C.cmem_t, label C.cmem_t, ct C.cmem_t, outPlain *C.cmem_t) (code C.cbmpc_error_t) {
	code = C.CBMPC_SUCCESS
	defer recoverCallbackPanic(&code, C.CBMPC_E_GENERAL, func() {
		freeAndZeroCmemOutput(outPlain)
	})

	if outPlain == nil {
		return C.CBMPC_E_BADARG
	}
	zeroCmemOutput(outPlain)

	cb, ok := getPVEBasePKECallbacks(ctx)
	if !ok || cb == nil || cb.Decrypt == nil {
		return C.CBMPC_E_BADARG
	}

	plain, err := cb.Decrypt(cmemToBytes(dk), cmemToBytes(label), cmemToBytes(ct))
	if err != nil {
		return C.CBMPC_E_GENERAL
	}
	return setOwnedCmemOutput(outPlain, plain)
}

//export goPVERSAOAEPHSMDecap
func goPVERSAOAEPHSMDecap(ctx unsafe.Pointer, dkHandle C.cmem_t, kemCT C.cmem_t, outKemSS *C.cmem_t) (code C.cbmpc_error_t) {
	code = C.CBMPC_SUCCESS
	defer recoverCallbackPanic(&code, C.CBMPC_E_GENERAL, func() {
		freeAndZeroCmemOutput(outKemSS)
	})

	if outKemSS == nil {
		return C.CBMPC_E_BADARG
	}
	zeroCmemOutput(outKemSS)

	cb, ok := getPVERSAOAEPHSMDecapCallbacks(ctx)
	if !ok || cb == nil || cb.Decap == nil {
		return C.CBMPC_E_BADARG
	}

	kemSS, err := cb.Decap(cmemToBytes(dkHandle), cmemToBytes(kemCT))
	if err != nil {
		return C.CBMPC_E_GENERAL
	}
	return setOwnedCmemOutput(outKemSS, kemSS)
}

//export goPVEECIESP256HSMECDH
func goPVEECIESP256HSMECDH(ctx unsafe.Pointer, dkHandle C.cmem_t, kemCT C.cmem_t, outDHX32 *C.cmem_t) (code C.cbmpc_error_t) {
	code = C.CBMPC_SUCCESS
	defer recoverCallbackPanic(&code, C.CBMPC_E_GENERAL, func() {
		freeAndZeroCmemOutput(outDHX32)
	})

	if outDHX32 == nil {
		return C.CBMPC_E_BADARG
	}
	zeroCmemOutput(outDHX32)

	cb, ok := getPVEECIESP256HSMECDHCallbacks(ctx)
	if !ok || cb == nil || cb.ECDH == nil {
		return C.CBMPC_E_BADARG
	}

	dhX32, err := cb.ECDH(cmemToBytes(dkHandle), cmemToBytes(kemCT))
	if err != nil {
		return C.CBMPC_E_GENERAL
	}
	return setOwnedCmemOutput(outDHX32, dhX32)
}

//export goPVEKEMEncap
func goPVEKEMEncap(ctx unsafe.Pointer, ek C.cmem_t, rho32 C.cmem_t, outKemCT *C.cmem_t, outKemSS *C.cmem_t) (code C.cbmpc_error_t) {
	code = C.CBMPC_SUCCESS
	defer recoverCallbackPanic(&code, C.CBMPC_E_GENERAL, func() {
		freeAndZeroCmemOutput(outKemCT)
		freeAndZeroCmemOutput(outKemSS)
	})

	if outKemCT == nil || outKemSS == nil {
		return C.CBMPC_E_BADARG
	}
	zeroCmemOutput(outKemCT)
	zeroCmemOutput(outKemSS)

	cb, ok := getPVEBaseKEMCallbacks(ctx)
	if !ok || cb == nil || cb.Encap == nil {
		return C.CBMPC_E_BADARG
	}

	kemCT, kemSS, err := cb.Encap(cmemToBytes(ek), cmemToBytes(rho32))
	if err != nil {
		return C.CBMPC_E_GENERAL
	}
	if code := setOwnedCmemOutput(outKemCT, kemCT); code != C.CBMPC_SUCCESS {
		return code
	}
	if code := setOwnedCmemOutput(outKemSS, kemSS); code != C.CBMPC_SUCCESS {
		freeAndZeroCmemOutput(outKemCT)
		return code
	}
	return C.CBMPC_SUCCESS
}

//export goPVEKEMDecap
func goPVEKEMDecap(ctx unsafe.Pointer, dk C.cmem_t, kemCT C.cmem_t, outKemSS *C.cmem_t) (code C.cbmpc_error_t) {
	code = C.CBMPC_SUCCESS
	defer recoverCallbackPanic(&code, C.CBMPC_E_GENERAL, func() {
		freeAndZeroCmemOutput(outKemSS)
	})

	if outKemSS == nil {
		return C.CBMPC_E_BADARG
	}
	zeroCmemOutput(outKemSS)

	cb, ok := getPVEBaseKEMCallbacks(ctx)
	if !ok || cb == nil || cb.Decap == nil {
		return C.CBMPC_E_BADARG
	}

	kemSS, err := cb.Decap(cmemToBytes(dk), cmemToBytes(kemCT))
	if err != nil {
		return C.CBMPC_E_GENERAL
	}
	return setOwnedCmemOutput(outKemSS, kemSS)
}

// -------------------------------
// ECDSA-2PC
// -------------------------------

func ECDSA2PDKG(ctx context.Context, job Job2P, curve CurveID) ([]byte, error) {
	var outKeyBlob C.cmem_t
	err := with2PCJob(ctx, job, func(jobPtr *C.cbmpc_2pc_job_t) error {
		return codeError("cbmpc_ecdsa_2p_dkg", C.cbmpc_ecdsa_2p_dkg(jobPtr, C.cbmpc_curve_id_t(curve), &outKeyBlob))
	})
	if err != nil {
		freeCmem(outKeyBlob)
		return nil, err
	}
	return cmemToBytesAndFree(outKeyBlob), nil
}

func ECDSA2PRefresh(ctx context.Context, job Job2P, keyBlob []byte) ([]byte, error) {
	var outNewKeyBlob C.cmem_t
	keyBlobMem, keyBlobCleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer keyBlobCleanup()
	err = with2PCJob(ctx, job, func(jobPtr *C.cbmpc_2pc_job_t) error {
		return codeError("cbmpc_ecdsa_2p_refresh", C.cbmpc_ecdsa_2p_refresh(jobPtr, keyBlobMem, &outNewKeyBlob))
	})
	if err != nil {
		freeCmem(outNewKeyBlob)
		return nil, err
	}
	return cmemToBytesAndFree(outNewKeyBlob), nil
}

func ECDSA2PSign(ctx context.Context, job Job2P, keyBlob []byte, msgHash []byte, sidIn []byte) ([]byte, []byte, error) {
	var sidOut C.cmem_t
	var sigOut C.cmem_t
	mems, cleanup, err := bytesToOwnedCmems(keyBlob, msgHash, sidIn)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	keyBlobMem := mems[0]
	msgHashMem := mems[1]
	sidInMem := mems[2]

	err = with2PCJob(ctx, job, func(jobPtr *C.cbmpc_2pc_job_t) error {
		return codeError("cbmpc_ecdsa_2p_sign", C.cbmpc_ecdsa_2p_sign(jobPtr, keyBlobMem, msgHashMem, sidInMem, &sidOut, &sigOut))
	})
	if err != nil {
		freeCmem(sidOut)
		freeCmem(sigOut)
		return nil, nil, err
	}
	return cmemToBytesAndFree(sidOut), cmemToBytesAndFree(sigOut), nil
}

func ECDSA2PGetPublicKeyCompressed(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_ecdsa_2p_get_public_key_compressed", C.cbmpc_ecdsa_2p_get_public_key_compressed(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func ECDSA2PGetPublicShareCompressed(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_ecdsa_2p_get_public_share_compressed", C.cbmpc_ecdsa_2p_get_public_share_compressed(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func ECDSA2PDetachPrivateScalar(keyBlob []byte) ([]byte, []byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	var outPublicKeyBlob C.cmem_t
	var outPrivateScalar C.cmem_t
	if err := codeError("cbmpc_ecdsa_2p_detach_private_scalar", C.cbmpc_ecdsa_2p_detach_private_scalar(in, &outPublicKeyBlob, &outPrivateScalar)); err != nil {
		freeCmem(outPublicKeyBlob)
		freeCmem(outPrivateScalar)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outPublicKeyBlob), cmemToBytesAndFree(outPrivateScalar), nil
}

func ECDSA2PAttachPrivateScalar(publicKeyBlob []byte, privateScalar []byte, publicShareCompressed []byte) ([]byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(publicKeyBlob, privateScalar, publicShareCompressed)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	publicKeyBlobMem := mems[0]
	privateScalarMem := mems[1]
	publicShareCompressedMem := mems[2]

	var outKeyBlob C.cmem_t
	if err := codeError("cbmpc_ecdsa_2p_attach_private_scalar", C.cbmpc_ecdsa_2p_attach_private_scalar(publicKeyBlobMem, privateScalarMem, publicShareCompressedMem, &outKeyBlob)); err != nil {
		freeCmem(outKeyBlob)
		return nil, err
	}
	return cmemToBytesAndFree(outKeyBlob), nil
}

// -------------------------------
// ECDSA-MP
// -------------------------------

func ECDSAMPDKGAdditive(ctx context.Context, job JobMP, curve CurveID) ([]byte, []byte, error) {
	var outKeyBlob C.cmem_t
	var outSID C.cmem_t
	err := withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_ecdsa_mp_dkg_additive", C.cbmpc_ecdsa_mp_dkg_additive(jobPtr, C.cbmpc_curve_id_t(curve), &outKeyBlob, &outSID))
	})
	if err != nil {
		freeCmem(outKeyBlob)
		freeCmem(outSID)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outKeyBlob), cmemToBytesAndFree(outSID), nil
}

func ECDSAMPDKGAC(ctx context.Context, job JobMP, curve CurveID, sidIn []byte, access *AccessStructure, quorumPartyNames []string) ([]byte, []byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, nil, err
	}
	if err := validatePartyNameSubset("quorumPartyNames", quorumPartyNames, job.PartyNames); err != nil {
		return nil, nil, err
	}
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, nil, err
	}
	defer compiledAccess.free()

	quorumArray, quorumCStrings, err := createCStringArray(quorumPartyNames)
	if err != nil {
		return nil, nil, err
	}
	defer freeCStringArray(quorumArray, quorumCStrings)

	var outACKeyBlob C.cmem_t
	var outSID C.cmem_t
	sidInMem, sidInCleanup, err := bytesToOwnedCmem(sidIn)
	if err != nil {
		return nil, nil, err
	}
	defer sidInCleanup()
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_ecdsa_mp_dkg_ac", C.cbmpc_ecdsa_mp_dkg_ac(jobPtr, C.cbmpc_curve_id_t(curve), sidInMem, &compiledAccess.cStruct, (**C.char)(quorumArray), C.int(len(quorumPartyNames)), &outACKeyBlob, &outSID))
	})
	if err != nil {
		freeCmem(outACKeyBlob)
		freeCmem(outSID)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outACKeyBlob), cmemToBytesAndFree(outSID), nil
}

func ECDSAMPRefreshAdditive(ctx context.Context, job JobMP, sidIn []byte, keyBlob []byte) ([]byte, []byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(sidIn, keyBlob)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	sidInMem := mems[0]
	keyBlobMem := mems[1]
	var sidOut C.cmem_t
	var outNewKeyBlob C.cmem_t

	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_ecdsa_mp_refresh_additive", C.cbmpc_ecdsa_mp_refresh_additive(jobPtr, sidInMem, keyBlobMem, &sidOut, &outNewKeyBlob))
	})
	if err != nil {
		freeCmem(sidOut)
		freeCmem(outNewKeyBlob)
		return nil, nil, err
	}
	return cmemToBytesAndFree(sidOut), cmemToBytesAndFree(outNewKeyBlob), nil
}

func ECDSAMPRefreshAC(ctx context.Context, job JobMP, sidIn []byte, acKeyBlob []byte, access *AccessStructure, quorumPartyNames []string) ([]byte, []byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, nil, err
	}
	if err := validatePartyNameSubset("quorumPartyNames", quorumPartyNames, job.PartyNames); err != nil {
		return nil, nil, err
	}
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, nil, err
	}
	defer compiledAccess.free()

	quorumArray, quorumCStrings, err := createCStringArray(quorumPartyNames)
	if err != nil {
		return nil, nil, err
	}
	defer freeCStringArray(quorumArray, quorumCStrings)

	mems, cleanup, err := bytesToOwnedCmems(sidIn, acKeyBlob)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	sidInMem := mems[0]
	acKeyBlobMem := mems[1]
	var sidOut C.cmem_t
	var outNewACKeyBlob C.cmem_t
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_ecdsa_mp_refresh_ac", C.cbmpc_ecdsa_mp_refresh_ac(jobPtr, sidInMem, acKeyBlobMem, &compiledAccess.cStruct, (**C.char)(quorumArray), C.int(len(quorumPartyNames)), &sidOut, &outNewACKeyBlob))
	})
	if err != nil {
		freeCmem(sidOut)
		freeCmem(outNewACKeyBlob)
		return nil, nil, err
	}
	return cmemToBytesAndFree(sidOut), cmemToBytesAndFree(outNewACKeyBlob), nil
}

func ECDSAMPSignAdditive(ctx context.Context, job JobMP, keyBlob []byte, msgHash []byte, sigReceiver int) ([]byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, err
	}
	if err := validatePartyIndex("sigReceiver", sigReceiver, len(job.PartyNames)); err != nil {
		return nil, err
	}
	mems, cleanup, err := bytesToOwnedCmems(keyBlob, msgHash)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	keyBlobMem := mems[0]
	msgHashMem := mems[1]
	var sigOut C.cmem_t
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_ecdsa_mp_sign_additive", C.cbmpc_ecdsa_mp_sign_additive(jobPtr, keyBlobMem, msgHashMem, C.int32_t(sigReceiver), &sigOut))
	})
	if err != nil {
		freeCmem(sigOut)
		return nil, err
	}
	return cmemToBytesAndFree(sigOut), nil
}

func ECDSAMPSignAC(ctx context.Context, job JobMP, acKeyBlob []byte, access *AccessStructure, msgHash []byte, sigReceiver int) ([]byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, err
	}
	if err := validatePartyIndex("sigReceiver", sigReceiver, len(job.PartyNames)); err != nil {
		return nil, err
	}
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, err
	}
	defer compiledAccess.free()

	mems, cleanup, err := bytesToOwnedCmems(acKeyBlob, msgHash)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	acKeyBlobMem := mems[0]
	msgHashMem := mems[1]
	var sigOut C.cmem_t
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_ecdsa_mp_sign_ac", C.cbmpc_ecdsa_mp_sign_ac(jobPtr, acKeyBlobMem, &compiledAccess.cStruct, msgHashMem, C.int32_t(sigReceiver), &sigOut))
	})
	if err != nil {
		freeCmem(sigOut)
		return nil, err
	}
	return cmemToBytesAndFree(sigOut), nil
}

func ECDSAMPGetPublicKeyCompressed(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_ecdsa_mp_get_public_key_compressed", C.cbmpc_ecdsa_mp_get_public_key_compressed(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func ECDSAMPGetPublicShareCompressed(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_ecdsa_mp_get_public_share_compressed", C.cbmpc_ecdsa_mp_get_public_share_compressed(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func ECDSAMPDetachPrivateScalar(keyBlob []byte) ([]byte, []byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	var outPublicKeyBlob C.cmem_t
	var outPrivateScalar C.cmem_t
	if err := codeError("cbmpc_ecdsa_mp_detach_private_scalar", C.cbmpc_ecdsa_mp_detach_private_scalar(in, &outPublicKeyBlob, &outPrivateScalar)); err != nil {
		freeCmem(outPublicKeyBlob)
		freeCmem(outPrivateScalar)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outPublicKeyBlob), cmemToBytesAndFree(outPrivateScalar), nil
}

func ECDSAMPAttachPrivateScalar(publicKeyBlob []byte, privateScalarFixed []byte, publicShareCompressed []byte) ([]byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(publicKeyBlob, privateScalarFixed, publicShareCompressed)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	publicKeyBlobMem := mems[0]
	privateScalarFixedMem := mems[1]
	publicShareCompressedMem := mems[2]
	var outKeyBlob C.cmem_t
	if err := codeError("cbmpc_ecdsa_mp_attach_private_scalar", C.cbmpc_ecdsa_mp_attach_private_scalar(publicKeyBlobMem, privateScalarFixedMem, publicShareCompressedMem, &outKeyBlob)); err != nil {
		freeCmem(outKeyBlob)
		return nil, err
	}
	return cmemToBytesAndFree(outKeyBlob), nil
}

// -------------------------------
// EdDSA-2PC
// -------------------------------

func EdDSA2PDKG(ctx context.Context, job Job2P, curve CurveID) ([]byte, error) {
	var outKeyBlob C.cmem_t
	err := with2PCJob(ctx, job, func(jobPtr *C.cbmpc_2pc_job_t) error {
		return codeError("cbmpc_eddsa_2p_dkg", C.cbmpc_eddsa_2p_dkg(jobPtr, C.cbmpc_curve_id_t(curve), &outKeyBlob))
	})
	if err != nil {
		freeCmem(outKeyBlob)
		return nil, err
	}
	return cmemToBytesAndFree(outKeyBlob), nil
}

func EdDSA2PRefresh(ctx context.Context, job Job2P, keyBlob []byte) ([]byte, error) {
	var outNewKeyBlob C.cmem_t
	keyBlobMem, keyBlobCleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer keyBlobCleanup()
	err = with2PCJob(ctx, job, func(jobPtr *C.cbmpc_2pc_job_t) error {
		return codeError("cbmpc_eddsa_2p_refresh", C.cbmpc_eddsa_2p_refresh(jobPtr, keyBlobMem, &outNewKeyBlob))
	})
	if err != nil {
		freeCmem(outNewKeyBlob)
		return nil, err
	}
	return cmemToBytesAndFree(outNewKeyBlob), nil
}

func EdDSA2PSign(ctx context.Context, job Job2P, keyBlob []byte, msg []byte) ([]byte, error) {
	var sigOut C.cmem_t
	mems, cleanup, err := bytesToOwnedCmems(keyBlob, msg)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	keyBlobMem := mems[0]
	msgMem := mems[1]
	err = with2PCJob(ctx, job, func(jobPtr *C.cbmpc_2pc_job_t) error {
		return codeError("cbmpc_eddsa_2p_sign", C.cbmpc_eddsa_2p_sign(jobPtr, keyBlobMem, msgMem, &sigOut))
	})
	if err != nil {
		freeCmem(sigOut)
		return nil, err
	}
	return cmemToBytesAndFree(sigOut), nil
}

func EdDSA2PGetPublicKeyCompressed(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_eddsa_2p_get_public_key_compressed", C.cbmpc_eddsa_2p_get_public_key_compressed(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func EdDSA2PGetPublicShareCompressed(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_eddsa_2p_get_public_share_compressed", C.cbmpc_eddsa_2p_get_public_share_compressed(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func EdDSA2PDetachPrivateScalar(keyBlob []byte) ([]byte, []byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	var outPublicKeyBlob C.cmem_t
	var outPrivateScalar C.cmem_t
	if err := codeError("cbmpc_eddsa_2p_detach_private_scalar", C.cbmpc_eddsa_2p_detach_private_scalar(in, &outPublicKeyBlob, &outPrivateScalar)); err != nil {
		freeCmem(outPublicKeyBlob)
		freeCmem(outPrivateScalar)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outPublicKeyBlob), cmemToBytesAndFree(outPrivateScalar), nil
}

func EdDSA2PAttachPrivateScalar(publicKeyBlob []byte, privateScalarFixed []byte, publicShareCompressed []byte) ([]byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(publicKeyBlob, privateScalarFixed, publicShareCompressed)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	publicKeyBlobMem := mems[0]
	privateScalarMem := mems[1]
	publicShareCompressedMem := mems[2]
	var outKeyBlob C.cmem_t
	if err := codeError("cbmpc_eddsa_2p_attach_private_scalar", C.cbmpc_eddsa_2p_attach_private_scalar(publicKeyBlobMem, privateScalarMem, publicShareCompressedMem, &outKeyBlob)); err != nil {
		freeCmem(outKeyBlob)
		return nil, err
	}
	return cmemToBytesAndFree(outKeyBlob), nil
}

// -------------------------------
// EdDSA-MP
// -------------------------------

func EdDSAMPDKGAdditive(ctx context.Context, job JobMP, curve CurveID) ([]byte, []byte, error) {
	var outKeyBlob C.cmem_t
	var outSID C.cmem_t
	err := withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_eddsa_mp_dkg_additive", C.cbmpc_eddsa_mp_dkg_additive(jobPtr, C.cbmpc_curve_id_t(curve), &outKeyBlob, &outSID))
	})
	if err != nil {
		freeCmem(outKeyBlob)
		freeCmem(outSID)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outKeyBlob), cmemToBytesAndFree(outSID), nil
}

func EdDSAMPDKGAC(ctx context.Context, job JobMP, curve CurveID, sidIn []byte, access *AccessStructure, quorumPartyNames []string) ([]byte, []byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, nil, err
	}
	if err := validatePartyNameSubset("quorumPartyNames", quorumPartyNames, job.PartyNames); err != nil {
		return nil, nil, err
	}
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, nil, err
	}
	defer compiledAccess.free()

	quorumArray, quorumCStrings, err := createCStringArray(quorumPartyNames)
	if err != nil {
		return nil, nil, err
	}
	defer freeCStringArray(quorumArray, quorumCStrings)

	var outACKeyBlob C.cmem_t
	var outSID C.cmem_t
	sidInMem, sidInCleanup, err := bytesToOwnedCmem(sidIn)
	if err != nil {
		return nil, nil, err
	}
	defer sidInCleanup()
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_eddsa_mp_dkg_ac", C.cbmpc_eddsa_mp_dkg_ac(jobPtr, C.cbmpc_curve_id_t(curve), sidInMem, &compiledAccess.cStruct, (**C.char)(quorumArray), C.int(len(quorumPartyNames)), &outACKeyBlob, &outSID))
	})
	if err != nil {
		freeCmem(outACKeyBlob)
		freeCmem(outSID)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outACKeyBlob), cmemToBytesAndFree(outSID), nil
}

func EdDSAMPRefreshAdditive(ctx context.Context, job JobMP, sidIn []byte, keyBlob []byte) ([]byte, []byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(sidIn, keyBlob)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	sidInMem := mems[0]
	keyBlobMem := mems[1]
	var sidOut C.cmem_t
	var outNewKeyBlob C.cmem_t
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_eddsa_mp_refresh_additive", C.cbmpc_eddsa_mp_refresh_additive(jobPtr, sidInMem, keyBlobMem, &sidOut, &outNewKeyBlob))
	})
	if err != nil {
		freeCmem(sidOut)
		freeCmem(outNewKeyBlob)
		return nil, nil, err
	}
	return cmemToBytesAndFree(sidOut), cmemToBytesAndFree(outNewKeyBlob), nil
}

func EdDSAMPRefreshAC(ctx context.Context, job JobMP, sidIn []byte, acKeyBlob []byte, access *AccessStructure, quorumPartyNames []string) ([]byte, []byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, nil, err
	}
	if err := validatePartyNameSubset("quorumPartyNames", quorumPartyNames, job.PartyNames); err != nil {
		return nil, nil, err
	}
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, nil, err
	}
	defer compiledAccess.free()

	quorumArray, quorumCStrings, err := createCStringArray(quorumPartyNames)
	if err != nil {
		return nil, nil, err
	}
	defer freeCStringArray(quorumArray, quorumCStrings)

	mems, cleanup, err := bytesToOwnedCmems(sidIn, acKeyBlob)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	sidInMem := mems[0]
	acKeyBlobMem := mems[1]
	var sidOut C.cmem_t
	var outNewACKeyBlob C.cmem_t
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_eddsa_mp_refresh_ac", C.cbmpc_eddsa_mp_refresh_ac(jobPtr, sidInMem, acKeyBlobMem, &compiledAccess.cStruct, (**C.char)(quorumArray), C.int(len(quorumPartyNames)), &sidOut, &outNewACKeyBlob))
	})
	if err != nil {
		freeCmem(sidOut)
		freeCmem(outNewACKeyBlob)
		return nil, nil, err
	}
	return cmemToBytesAndFree(sidOut), cmemToBytesAndFree(outNewACKeyBlob), nil
}

func EdDSAMPSignAdditive(ctx context.Context, job JobMP, keyBlob []byte, msg []byte, sigReceiver int) ([]byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, err
	}
	if err := validatePartyIndex("sigReceiver", sigReceiver, len(job.PartyNames)); err != nil {
		return nil, err
	}
	mems, cleanup, err := bytesToOwnedCmems(keyBlob, msg)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	keyBlobMem := mems[0]
	msgMem := mems[1]
	var sigOut C.cmem_t
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_eddsa_mp_sign_additive", C.cbmpc_eddsa_mp_sign_additive(jobPtr, keyBlobMem, msgMem, C.int32_t(sigReceiver), &sigOut))
	})
	if err != nil {
		freeCmem(sigOut)
		return nil, err
	}
	return cmemToBytesAndFree(sigOut), nil
}

func EdDSAMPSignAC(ctx context.Context, job JobMP, acKeyBlob []byte, access *AccessStructure, msg []byte, sigReceiver int) ([]byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, err
	}
	if err := validatePartyIndex("sigReceiver", sigReceiver, len(job.PartyNames)); err != nil {
		return nil, err
	}
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, err
	}
	defer compiledAccess.free()

	mems, cleanup, err := bytesToOwnedCmems(acKeyBlob, msg)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	acKeyBlobMem := mems[0]
	msgMem := mems[1]
	var sigOut C.cmem_t
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_eddsa_mp_sign_ac", C.cbmpc_eddsa_mp_sign_ac(jobPtr, acKeyBlobMem, &compiledAccess.cStruct, msgMem, C.int32_t(sigReceiver), &sigOut))
	})
	if err != nil {
		freeCmem(sigOut)
		return nil, err
	}
	return cmemToBytesAndFree(sigOut), nil
}

func EdDSAMPGetPublicKeyCompressed(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_eddsa_mp_get_public_key_compressed", C.cbmpc_eddsa_mp_get_public_key_compressed(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func EdDSAMPGetPublicShareCompressed(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_eddsa_mp_get_public_share_compressed", C.cbmpc_eddsa_mp_get_public_share_compressed(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func EdDSAMPDetachPrivateScalar(keyBlob []byte) ([]byte, []byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	var outPublicKeyBlob C.cmem_t
	var outPrivateScalar C.cmem_t
	if err := codeError("cbmpc_eddsa_mp_detach_private_scalar", C.cbmpc_eddsa_mp_detach_private_scalar(in, &outPublicKeyBlob, &outPrivateScalar)); err != nil {
		freeCmem(outPublicKeyBlob)
		freeCmem(outPrivateScalar)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outPublicKeyBlob), cmemToBytesAndFree(outPrivateScalar), nil
}

func EdDSAMPAttachPrivateScalar(publicKeyBlob []byte, privateScalarFixed []byte, publicShareCompressed []byte) ([]byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(publicKeyBlob, privateScalarFixed, publicShareCompressed)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	publicKeyBlobMem := mems[0]
	privateScalarMem := mems[1]
	publicShareCompressedMem := mems[2]
	var outKeyBlob C.cmem_t
	if err := codeError("cbmpc_eddsa_mp_attach_private_scalar", C.cbmpc_eddsa_mp_attach_private_scalar(publicKeyBlobMem, privateScalarMem, publicShareCompressedMem, &outKeyBlob)); err != nil {
		freeCmem(outKeyBlob)
		return nil, err
	}
	return cmemToBytesAndFree(outKeyBlob), nil
}

// -------------------------------
// Schnorr-2PC
// -------------------------------

func Schnorr2PDKG(ctx context.Context, job Job2P, curve CurveID) ([]byte, error) {
	var outKeyBlob C.cmem_t
	err := with2PCJob(ctx, job, func(jobPtr *C.cbmpc_2pc_job_t) error {
		return codeError("cbmpc_schnorr_2p_dkg", C.cbmpc_schnorr_2p_dkg(jobPtr, C.cbmpc_curve_id_t(curve), &outKeyBlob))
	})
	if err != nil {
		freeCmem(outKeyBlob)
		return nil, err
	}
	return cmemToBytesAndFree(outKeyBlob), nil
}

func Schnorr2PRefresh(ctx context.Context, job Job2P, keyBlob []byte) ([]byte, error) {
	var outNewKeyBlob C.cmem_t
	keyBlobMem, keyBlobCleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer keyBlobCleanup()
	err = with2PCJob(ctx, job, func(jobPtr *C.cbmpc_2pc_job_t) error {
		return codeError("cbmpc_schnorr_2p_refresh", C.cbmpc_schnorr_2p_refresh(jobPtr, keyBlobMem, &outNewKeyBlob))
	})
	if err != nil {
		freeCmem(outNewKeyBlob)
		return nil, err
	}
	return cmemToBytesAndFree(outNewKeyBlob), nil
}

func Schnorr2PSign(ctx context.Context, job Job2P, keyBlob []byte, msg []byte) ([]byte, error) {
	var sigOut C.cmem_t
	mems, cleanup, err := bytesToOwnedCmems(keyBlob, msg)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	keyBlobMem := mems[0]
	msgMem := mems[1]
	err = with2PCJob(ctx, job, func(jobPtr *C.cbmpc_2pc_job_t) error {
		return codeError("cbmpc_schnorr_2p_sign", C.cbmpc_schnorr_2p_sign(jobPtr, keyBlobMem, msgMem, &sigOut))
	})
	if err != nil {
		freeCmem(sigOut)
		return nil, err
	}
	return cmemToBytesAndFree(sigOut), nil
}

func Schnorr2PGetPublicKeyCompressed(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_schnorr_2p_get_public_key_compressed", C.cbmpc_schnorr_2p_get_public_key_compressed(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func Schnorr2PExtractPublicKeyXOnly(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_schnorr_2p_extract_public_key_xonly", C.cbmpc_schnorr_2p_extract_public_key_xonly(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func Schnorr2PGetPublicShareCompressed(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_schnorr_2p_get_public_share_compressed", C.cbmpc_schnorr_2p_get_public_share_compressed(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func Schnorr2PDetachPrivateScalar(keyBlob []byte) ([]byte, []byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	var outPublicKeyBlob C.cmem_t
	var outPrivateScalar C.cmem_t
	if err := codeError("cbmpc_schnorr_2p_detach_private_scalar", C.cbmpc_schnorr_2p_detach_private_scalar(in, &outPublicKeyBlob, &outPrivateScalar)); err != nil {
		freeCmem(outPublicKeyBlob)
		freeCmem(outPrivateScalar)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outPublicKeyBlob), cmemToBytesAndFree(outPrivateScalar), nil
}

func Schnorr2PAttachPrivateScalar(publicKeyBlob []byte, privateScalarFixed []byte, publicShareCompressed []byte) ([]byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(publicKeyBlob, privateScalarFixed, publicShareCompressed)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	publicKeyBlobMem := mems[0]
	privateScalarMem := mems[1]
	publicShareCompressedMem := mems[2]
	var outKeyBlob C.cmem_t
	if err := codeError("cbmpc_schnorr_2p_attach_private_scalar", C.cbmpc_schnorr_2p_attach_private_scalar(publicKeyBlobMem, privateScalarMem, publicShareCompressedMem, &outKeyBlob)); err != nil {
		freeCmem(outKeyBlob)
		return nil, err
	}
	return cmemToBytesAndFree(outKeyBlob), nil
}

// -------------------------------
// Schnorr-MP
// -------------------------------

func SchnorrMPDKGAdditive(ctx context.Context, job JobMP, curve CurveID) ([]byte, []byte, error) {
	var outKeyBlob C.cmem_t
	var outSID C.cmem_t
	err := withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_schnorr_mp_dkg_additive", C.cbmpc_schnorr_mp_dkg_additive(jobPtr, C.cbmpc_curve_id_t(curve), &outKeyBlob, &outSID))
	})
	if err != nil {
		freeCmem(outKeyBlob)
		freeCmem(outSID)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outKeyBlob), cmemToBytesAndFree(outSID), nil
}

func SchnorrMPDKGAC(ctx context.Context, job JobMP, curve CurveID, sidIn []byte, access *AccessStructure, quorumPartyNames []string) ([]byte, []byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, nil, err
	}
	if err := validatePartyNameSubset("quorumPartyNames", quorumPartyNames, job.PartyNames); err != nil {
		return nil, nil, err
	}
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, nil, err
	}
	defer compiledAccess.free()

	quorumArray, quorumCStrings, err := createCStringArray(quorumPartyNames)
	if err != nil {
		return nil, nil, err
	}
	defer freeCStringArray(quorumArray, quorumCStrings)

	var outACKeyBlob C.cmem_t
	var outSID C.cmem_t
	sidInMem, sidInCleanup, err := bytesToOwnedCmem(sidIn)
	if err != nil {
		return nil, nil, err
	}
	defer sidInCleanup()
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_schnorr_mp_dkg_ac", C.cbmpc_schnorr_mp_dkg_ac(jobPtr, C.cbmpc_curve_id_t(curve), sidInMem, &compiledAccess.cStruct, (**C.char)(quorumArray), C.int(len(quorumPartyNames)), &outACKeyBlob, &outSID))
	})
	if err != nil {
		freeCmem(outACKeyBlob)
		freeCmem(outSID)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outACKeyBlob), cmemToBytesAndFree(outSID), nil
}

func SchnorrMPRefreshAdditive(ctx context.Context, job JobMP, sidIn []byte, keyBlob []byte) ([]byte, []byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(sidIn, keyBlob)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	sidInMem := mems[0]
	keyBlobMem := mems[1]
	var sidOut C.cmem_t
	var outNewKeyBlob C.cmem_t
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_schnorr_mp_refresh_additive", C.cbmpc_schnorr_mp_refresh_additive(jobPtr, sidInMem, keyBlobMem, &sidOut, &outNewKeyBlob))
	})
	if err != nil {
		freeCmem(sidOut)
		freeCmem(outNewKeyBlob)
		return nil, nil, err
	}
	return cmemToBytesAndFree(sidOut), cmemToBytesAndFree(outNewKeyBlob), nil
}

func SchnorrMPRefreshAC(ctx context.Context, job JobMP, sidIn []byte, acKeyBlob []byte, access *AccessStructure, quorumPartyNames []string) ([]byte, []byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, nil, err
	}
	if err := validatePartyNameSubset("quorumPartyNames", quorumPartyNames, job.PartyNames); err != nil {
		return nil, nil, err
	}
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, nil, err
	}
	defer compiledAccess.free()

	quorumArray, quorumCStrings, err := createCStringArray(quorumPartyNames)
	if err != nil {
		return nil, nil, err
	}
	defer freeCStringArray(quorumArray, quorumCStrings)

	mems, cleanup, err := bytesToOwnedCmems(sidIn, acKeyBlob)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	sidInMem := mems[0]
	acKeyBlobMem := mems[1]
	var sidOut C.cmem_t
	var outNewACKeyBlob C.cmem_t
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_schnorr_mp_refresh_ac", C.cbmpc_schnorr_mp_refresh_ac(jobPtr, sidInMem, acKeyBlobMem, &compiledAccess.cStruct, (**C.char)(quorumArray), C.int(len(quorumPartyNames)), &sidOut, &outNewACKeyBlob))
	})
	if err != nil {
		freeCmem(sidOut)
		freeCmem(outNewACKeyBlob)
		return nil, nil, err
	}
	return cmemToBytesAndFree(sidOut), cmemToBytesAndFree(outNewACKeyBlob), nil
}

func SchnorrMPSignAdditive(ctx context.Context, job JobMP, keyBlob []byte, msg []byte, sigReceiver int) ([]byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, err
	}
	if err := validatePartyIndex("sigReceiver", sigReceiver, len(job.PartyNames)); err != nil {
		return nil, err
	}
	mems, cleanup, err := bytesToOwnedCmems(keyBlob, msg)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	keyBlobMem := mems[0]
	msgMem := mems[1]
	var sigOut C.cmem_t
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_schnorr_mp_sign_additive", C.cbmpc_schnorr_mp_sign_additive(jobPtr, keyBlobMem, msgMem, C.int32_t(sigReceiver), &sigOut))
	})
	if err != nil {
		freeCmem(sigOut)
		return nil, err
	}
	return cmemToBytesAndFree(sigOut), nil
}

func SchnorrMPSignAC(ctx context.Context, job JobMP, acKeyBlob []byte, access *AccessStructure, msg []byte, sigReceiver int) ([]byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, err
	}
	if err := validatePartyIndex("sigReceiver", sigReceiver, len(job.PartyNames)); err != nil {
		return nil, err
	}
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, err
	}
	defer compiledAccess.free()

	mems, cleanup, err := bytesToOwnedCmems(acKeyBlob, msg)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	acKeyBlobMem := mems[0]
	msgMem := mems[1]
	var sigOut C.cmem_t
	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_schnorr_mp_sign_ac", C.cbmpc_schnorr_mp_sign_ac(jobPtr, acKeyBlobMem, &compiledAccess.cStruct, msgMem, C.int32_t(sigReceiver), &sigOut))
	})
	if err != nil {
		freeCmem(sigOut)
		return nil, err
	}
	return cmemToBytesAndFree(sigOut), nil
}

func SchnorrMPGetPublicKeyCompressed(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_schnorr_mp_get_public_key_compressed", C.cbmpc_schnorr_mp_get_public_key_compressed(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func SchnorrMPExtractPublicKeyXOnly(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_schnorr_mp_extract_public_key_xonly", C.cbmpc_schnorr_mp_extract_public_key_xonly(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func SchnorrMPGetPublicShareCompressed(keyBlob []byte) ([]byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var out C.cmem_t
	if err := codeError("cbmpc_schnorr_mp_get_public_share_compressed", C.cbmpc_schnorr_mp_get_public_share_compressed(in, &out)); err != nil {
		freeCmem(out)
		return nil, err
	}
	return cmemToBytesAndFree(out), nil
}

func SchnorrMPDetachPrivateScalar(keyBlob []byte) ([]byte, []byte, error) {
	in, cleanup, err := bytesToOwnedCmem(keyBlob)
	if err != nil {
		return nil, nil, err
	}
	defer cleanup()
	var outPublicKeyBlob C.cmem_t
	var outPrivateScalar C.cmem_t
	if err := codeError("cbmpc_schnorr_mp_detach_private_scalar", C.cbmpc_schnorr_mp_detach_private_scalar(in, &outPublicKeyBlob, &outPrivateScalar)); err != nil {
		freeCmem(outPublicKeyBlob)
		freeCmem(outPrivateScalar)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outPublicKeyBlob), cmemToBytesAndFree(outPrivateScalar), nil
}

func SchnorrMPAttachPrivateScalar(publicKeyBlob []byte, privateScalarFixed []byte, publicShareCompressed []byte) ([]byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(publicKeyBlob, privateScalarFixed, publicShareCompressed)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	publicKeyBlobMem := mems[0]
	privateScalarMem := mems[1]
	publicShareCompressedMem := mems[2]
	var outKeyBlob C.cmem_t
	if err := codeError("cbmpc_schnorr_mp_attach_private_scalar", C.cbmpc_schnorr_mp_attach_private_scalar(publicKeyBlobMem, privateScalarMem, publicShareCompressedMem, &outKeyBlob)); err != nil {
		freeCmem(outKeyBlob)
		return nil, err
	}
	return cmemToBytesAndFree(outKeyBlob), nil
}

// -------------------------------
// TDH2
// -------------------------------

func TDH2DKGAdditive(ctx context.Context, job JobMP, curve CurveID) ([]byte, [][]byte, []byte, []byte, error) {
	var outPublicKey C.cmem_t
	var outPublicShares C.cmems_t
	var outPrivateShare C.cmem_t
	var outSID C.cmem_t

	err := withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_tdh2_dkg_additive", C.cbmpc_tdh2_dkg_additive(jobPtr, C.cbmpc_curve_id_t(curve), &outPublicKey, &outPublicShares, &outPrivateShare, &outSID))
	})
	if err != nil {
		freeCmem(outPublicKey)
		freeCmems(outPublicShares)
		freeCmem(outPrivateShare)
		freeCmem(outSID)
		return nil, nil, nil, nil, err
	}

	publicShares, err := cmemsToByteSlicesAndFree(outPublicShares)
	if err != nil {
		freeCmem(outPublicKey)
		freeCmem(outPrivateShare)
		freeCmem(outSID)
		return nil, nil, nil, nil, err
	}

	return cmemToBytesAndFree(outPublicKey), publicShares, cmemToBytesAndFree(outPrivateShare), cmemToBytesAndFree(outSID), nil
}

func TDH2DKGAC(ctx context.Context, job JobMP, curve CurveID, sidIn []byte, access *AccessStructure, quorumPartyNames []string) ([]byte, [][]byte, []byte, []byte, error) {
	if err := validateJobMP(job); err != nil {
		return nil, nil, nil, nil, err
	}
	if err := validatePartyNameSubset("quorumPartyNames", quorumPartyNames, job.PartyNames); err != nil {
		return nil, nil, nil, nil, err
	}
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, nil, nil, nil, err
	}
	defer compiledAccess.free()

	quorumArray, quorumCStrings, err := createCStringArray(quorumPartyNames)
	if err != nil {
		return nil, nil, nil, nil, err
	}
	defer freeCStringArray(quorumArray, quorumCStrings)

	var outPublicKey C.cmem_t
	var outPublicShares C.cmems_t
	var outPrivateShare C.cmem_t
	var outSID C.cmem_t
	sidInMem, sidInCleanup, err := bytesToOwnedCmem(sidIn)
	if err != nil {
		return nil, nil, nil, nil, err
	}
	defer sidInCleanup()

	err = withMPJob(ctx, job, func(jobPtr *C.cbmpc_mp_job_t) error {
		return codeError("cbmpc_tdh2_dkg_ac", C.cbmpc_tdh2_dkg_ac(jobPtr, C.cbmpc_curve_id_t(curve), sidInMem, &compiledAccess.cStruct, (**C.char)(quorumArray), C.int(len(quorumPartyNames)), &outPublicKey, &outPublicShares, &outPrivateShare, &outSID))
	})
	if err != nil {
		freeCmem(outPublicKey)
		freeCmems(outPublicShares)
		freeCmem(outPrivateShare)
		freeCmem(outSID)
		return nil, nil, nil, nil, err
	}

	publicShares, err := cmemsToByteSlicesAndFree(outPublicShares)
	if err != nil {
		freeCmem(outPublicKey)
		freeCmem(outPrivateShare)
		freeCmem(outSID)
		return nil, nil, nil, nil, err
	}

	return cmemToBytesAndFree(outPublicKey), publicShares, cmemToBytesAndFree(outPrivateShare), cmemToBytesAndFree(outSID), nil
}

func TDH2Encrypt(publicKey []byte, plaintext []byte, label []byte) ([]byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(publicKey, plaintext, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	publicKeyMem := mems[0]
	plaintextMem := mems[1]
	labelMem := mems[2]
	var outCiphertext C.cmem_t
	if err := codeError("cbmpc_tdh2_encrypt", C.cbmpc_tdh2_encrypt(publicKeyMem, plaintextMem, labelMem, &outCiphertext)); err != nil {
		freeCmem(outCiphertext)
		return nil, err
	}
	return cmemToBytesAndFree(outCiphertext), nil
}

func TDH2Verify(publicKey []byte, ciphertext []byte, label []byte) error {
	mems, cleanup, err := bytesToOwnedCmems(publicKey, ciphertext, label)
	if err != nil {
		return err
	}
	defer cleanup()
	publicKeyMem := mems[0]
	ciphertextMem := mems[1]
	labelMem := mems[2]
	return codeError("cbmpc_tdh2_verify", C.cbmpc_tdh2_verify(publicKeyMem, ciphertextMem, labelMem))
}

func TDH2PartialDecrypt(privateShare []byte, ciphertext []byte, label []byte) ([]byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(privateShare, ciphertext, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	privateShareMem := mems[0]
	ciphertextMem := mems[1]
	labelMem := mems[2]
	var outPartial C.cmem_t
	if err := codeError("cbmpc_tdh2_partial_decrypt", C.cbmpc_tdh2_partial_decrypt(privateShareMem, ciphertextMem, labelMem, &outPartial)); err != nil {
		freeCmem(outPartial)
		return nil, err
	}
	return cmemToBytesAndFree(outPartial), nil
}

func TDH2CombineAdditive(publicKey []byte, publicShares [][]byte, label []byte, partialDecryptions [][]byte, ciphertext []byte) ([]byte, error) {
	publicSharesMems, cleanupPublicShares, err := byteSlicesToOwnedCmems(publicShares)
	if err != nil {
		return nil, err
	}
	defer cleanupPublicShares()

	partialsMems, cleanupPartials, err := byteSlicesToOwnedCmems(partialDecryptions)
	if err != nil {
		return nil, err
	}
	defer cleanupPartials()

	mems, cleanup, err := bytesToOwnedCmems(publicKey, label, ciphertext)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	publicKeyMem := mems[0]
	labelMem := mems[1]
	ciphertextMem := mems[2]
	var outPlaintext C.cmem_t
	if err := codeError("cbmpc_tdh2_combine_additive", C.cbmpc_tdh2_combine_additive(publicKeyMem, publicSharesMems, labelMem, partialsMems, ciphertextMem, &outPlaintext)); err != nil {
		freeCmem(outPlaintext)
		return nil, err
	}
	return cmemToBytesAndFree(outPlaintext), nil
}

func TDH2CombineAC(access *AccessStructure, publicKey []byte, partyNames []string, publicShares [][]byte, label []byte, partialPartyNames []string, partialDecryptions [][]byte, ciphertext []byte) ([]byte, error) {
	if err := validateEqualLengths("partyNames", len(partyNames), "publicShares", len(publicShares)); err != nil {
		return nil, err
	}
	if err := validateEqualLengths("partialPartyNames", len(partialPartyNames), "partialDecryptions", len(partialDecryptions)); err != nil {
		return nil, err
	}
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, err
	}
	defer compiledAccess.free()

	partyNamesArray, partyNamesCStrings, err := createCStringArray(partyNames)
	if err != nil {
		return nil, err
	}
	defer freeCStringArray(partyNamesArray, partyNamesCStrings)

	partialPartyNamesArray, partialPartyNamesCStrings, err := createCStringArray(partialPartyNames)
	if err != nil {
		return nil, err
	}
	defer freeCStringArray(partialPartyNamesArray, partialPartyNamesCStrings)

	publicSharesMems, cleanupPublicShares, err := byteSlicesToOwnedCmems(publicShares)
	if err != nil {
		return nil, err
	}
	defer cleanupPublicShares()

	partialsMems, cleanupPartials, err := byteSlicesToOwnedCmems(partialDecryptions)
	if err != nil {
		return nil, err
	}
	defer cleanupPartials()

	mems, cleanup, err := bytesToOwnedCmems(publicKey, label, ciphertext)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	publicKeyMem := mems[0]
	labelMem := mems[1]
	ciphertextMem := mems[2]
	var outPlaintext C.cmem_t
	if err := codeError("cbmpc_tdh2_combine_ac", C.cbmpc_tdh2_combine_ac(&compiledAccess.cStruct, publicKeyMem, (**C.char)(partyNamesArray), C.int(len(partyNames)), publicSharesMems, labelMem, (**C.char)(partialPartyNamesArray), C.int(len(partialPartyNames)), partialsMems, ciphertextMem, &outPlaintext)); err != nil {
		freeCmem(outPlaintext)
		return nil, err
	}
	return cmemToBytesAndFree(outPlaintext), nil
}

// -------------------------------
// PVE (single)
// -------------------------------

func PVEGenerateBasePKERSAKeypair() ([]byte, []byte, error) {
	var outEK C.cmem_t
	var outDK C.cmem_t
	if err := codeError("cbmpc_pve_generate_base_pke_rsa_keypair", C.cbmpc_pve_generate_base_pke_rsa_keypair(&outEK, &outDK)); err != nil {
		freeCmem(outEK)
		freeCmem(outDK)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outEK), cmemToBytesAndFree(outDK), nil
}

func PVEGenerateBasePKEECIESP256Keypair() ([]byte, []byte, error) {
	var outEK C.cmem_t
	var outDK C.cmem_t
	if err := codeError("cbmpc_pve_generate_base_pke_ecies_p256_keypair", C.cbmpc_pve_generate_base_pke_ecies_p256_keypair(&outEK, &outDK)); err != nil {
		freeCmem(outEK)
		freeCmem(outDK)
		return nil, nil, err
	}
	return cmemToBytesAndFree(outEK), cmemToBytesAndFree(outDK), nil
}

func PVEBasePKEECIESP256EKFromOCT(pubKeyOct []byte) ([]byte, error) {
	pubKeyOctMem, cleanup, err := bytesToOwnedCmem(pubKeyOct)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var outEK C.cmem_t
	if err := codeError("cbmpc_pve_base_pke_ecies_p256_ek_from_oct", C.cbmpc_pve_base_pke_ecies_p256_ek_from_oct(pubKeyOctMem, &outEK)); err != nil {
		freeCmem(outEK)
		return nil, err
	}
	return cmemToBytesAndFree(outEK), nil
}

func PVEBasePKERSAEKFromModulus(modulus []byte) ([]byte, error) {
	modulusMem, cleanup, err := bytesToOwnedCmem(modulus)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var outEK C.cmem_t
	if err := codeError("cbmpc_pve_base_pke_rsa_ek_from_modulus", C.cbmpc_pve_base_pke_rsa_ek_from_modulus(modulusMem, &outEK)); err != nil {
		freeCmem(outEK)
		return nil, err
	}
	return cmemToBytesAndFree(outEK), nil
}

func PVEEncrypt(basePKE *PVEBasePKECallbacks, curve CurveID, ek []byte, label []byte, x []byte) ([]byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(ek, label, x)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	ekMem := mems[0]
	labelMem := mems[1]
	xMem := mems[2]
	var outCiphertext C.cmem_t

	var cBasePKE *C.cbmpc_pve_base_pke_t
	var basePKECtx unsafe.Pointer
	if basePKE != nil {
		var err error
		basePKECtx, err = registerPVEBasePKECallbacks(basePKE)
		if err != nil {
			return nil, err
		}
		defer unregisterPVEBasePKECallbacks(basePKECtx)
		cBasePKEValue := C.cbmpc_pve_base_pke_t{
			ctx:     basePKECtx,
			encrypt: (C.cbmpc_pve_base_pke_encrypt_fn)(C.goPVEBasePKEEncrypt),
			decrypt: (C.cbmpc_pve_base_pke_decrypt_fn)(C.goPVEBasePKEDecrypt),
		}
		cBasePKE = &cBasePKEValue
		if err := codeError("cbmpc_pve_encrypt", C.cbmpc_pve_encrypt(cBasePKE, C.cbmpc_curve_id_t(curve), ekMem, labelMem, xMem, &outCiphertext)); err != nil {
			freeCmem(outCiphertext)
			return nil, err
		}
		runtime.KeepAlive(cBasePKEValue)
		return cmemToBytesAndFree(outCiphertext), nil
	}

	if err := codeError("cbmpc_pve_encrypt", C.cbmpc_pve_encrypt(nil, C.cbmpc_curve_id_t(curve), ekMem, labelMem, xMem, &outCiphertext)); err != nil {
		freeCmem(outCiphertext)
		return nil, err
	}
	return cmemToBytesAndFree(outCiphertext), nil
}

func PVEVerify(basePKE *PVEBasePKECallbacks, curve CurveID, ek []byte, ciphertext []byte, qCompressed []byte, label []byte) error {
	mems, cleanup, err := bytesToOwnedCmems(ek, ciphertext, qCompressed, label)
	if err != nil {
		return err
	}
	defer cleanup()
	ekMem := mems[0]
	ciphertextMem := mems[1]
	qCompressedMem := mems[2]
	labelMem := mems[3]

	if basePKE != nil {
		basePKECtx, err := registerPVEBasePKECallbacks(basePKE)
		if err != nil {
			return err
		}
		defer unregisterPVEBasePKECallbacks(basePKECtx)

		cBasePKE := C.cbmpc_pve_base_pke_t{
			ctx:     basePKECtx,
			encrypt: (C.cbmpc_pve_base_pke_encrypt_fn)(C.goPVEBasePKEEncrypt),
			decrypt: (C.cbmpc_pve_base_pke_decrypt_fn)(C.goPVEBasePKEDecrypt),
		}
		err = codeError("cbmpc_pve_verify", C.cbmpc_pve_verify(&cBasePKE, C.cbmpc_curve_id_t(curve), ekMem, ciphertextMem, qCompressedMem, labelMem))
		runtime.KeepAlive(cBasePKE)
		return err
	}

	return codeError("cbmpc_pve_verify", C.cbmpc_pve_verify(nil, C.cbmpc_curve_id_t(curve), ekMem, ciphertextMem, qCompressedMem, labelMem))
}

func PVEDecrypt(basePKE *PVEBasePKECallbacks, curve CurveID, dk []byte, ek []byte, ciphertext []byte, label []byte) ([]byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(dk, ek, ciphertext, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	dkMem := mems[0]
	ekMem := mems[1]
	ciphertextMem := mems[2]
	labelMem := mems[3]
	var outX C.cmem_t

	if basePKE != nil {
		basePKECtx, err := registerPVEBasePKECallbacks(basePKE)
		if err != nil {
			return nil, err
		}
		defer unregisterPVEBasePKECallbacks(basePKECtx)

		cBasePKE := C.cbmpc_pve_base_pke_t{
			ctx:     basePKECtx,
			encrypt: (C.cbmpc_pve_base_pke_encrypt_fn)(C.goPVEBasePKEEncrypt),
			decrypt: (C.cbmpc_pve_base_pke_decrypt_fn)(C.goPVEBasePKEDecrypt),
		}
		if err := codeError("cbmpc_pve_decrypt", C.cbmpc_pve_decrypt(&cBasePKE, C.cbmpc_curve_id_t(curve), dkMem, ekMem, ciphertextMem, labelMem, &outX)); err != nil {
			freeCmem(outX)
			return nil, err
		}
		runtime.KeepAlive(cBasePKE)
		return cmemToBytesAndFree(outX), nil
	}

	if err := codeError("cbmpc_pve_decrypt", C.cbmpc_pve_decrypt(nil, C.cbmpc_curve_id_t(curve), dkMem, ekMem, ciphertextMem, labelMem, &outX)); err != nil {
		freeCmem(outX)
		return nil, err
	}
	return cmemToBytesAndFree(outX), nil
}

func PVEDecryptRSAOAEPHSM(curve CurveID, dkHandle []byte, ek []byte, ciphertext []byte, label []byte, callbacks *PVERSAOAEPHSMDecapCallbacks) ([]byte, error) {
	if callbacks == nil {
		return nil, fmt.Errorf("RSA-OAEP HSM callbacks cannot be nil")
	}

	cbCtx, err := registerPVERSAOAEPHSMDecapCallbacks(callbacks)
	if err != nil {
		return nil, err
	}
	defer unregisterPVERSAOAEPHSMDecapCallbacks(cbCtx)

	mems, cleanup, err := bytesToOwnedCmems(dkHandle, ek, ciphertext, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	dkHandleMem := mems[0]
	ekMem := mems[1]
	ciphertextMem := mems[2]
	labelMem := mems[3]
	cCallbacks := C.cbmpc_pve_rsa_oaep_hsm_decap_t{
		ctx:   cbCtx,
		decap: (C.cbmpc_pve_rsa_oaep_hsm_decap_fn)(C.goPVERSAOAEPHSMDecap),
	}

	var outX C.cmem_t
	if err := codeError("cbmpc_pve_decrypt_rsa_oaep_hsm", C.cbmpc_pve_decrypt_rsa_oaep_hsm(C.cbmpc_curve_id_t(curve), dkHandleMem, ekMem, ciphertextMem, labelMem, &cCallbacks, &outX)); err != nil {
		freeCmem(outX)
		return nil, err
	}
	runtime.KeepAlive(cCallbacks)
	return cmemToBytesAndFree(outX), nil
}

func PVEDecryptECIESP256HSM(curve CurveID, dkHandle []byte, ek []byte, ciphertext []byte, label []byte, callbacks *PVEECIESP256HSMECDHCallbacks) ([]byte, error) {
	if callbacks == nil {
		return nil, fmt.Errorf("ECIES P256 HSM callbacks cannot be nil")
	}

	cbCtx, err := registerPVEECIESP256HSMECDHCallbacks(callbacks)
	if err != nil {
		return nil, err
	}
	defer unregisterPVEECIESP256HSMECDHCallbacks(cbCtx)

	mems, cleanup, err := bytesToOwnedCmems(dkHandle, ek, ciphertext, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	dkHandleMem := mems[0]
	ekMem := mems[1]
	ciphertextMem := mems[2]
	labelMem := mems[3]
	cCallbacks := C.cbmpc_pve_ecies_p256_hsm_ecdh_t{
		ctx:  cbCtx,
		ecdh: (C.cbmpc_pve_ecies_p256_hsm_ecdh_fn)(C.goPVEECIESP256HSMECDH),
	}

	var outX C.cmem_t
	if err := codeError("cbmpc_pve_decrypt_ecies_p256_hsm", C.cbmpc_pve_decrypt_ecies_p256_hsm(C.cbmpc_curve_id_t(curve), dkHandleMem, ekMem, ciphertextMem, labelMem, &cCallbacks, &outX)); err != nil {
		freeCmem(outX)
		return nil, err
	}
	runtime.KeepAlive(cCallbacks)
	return cmemToBytesAndFree(outX), nil
}

func PVEEncryptWithKEM(curve CurveID, kemCallbacks *PVEBaseKEMCallbacks, ek []byte, label []byte, x []byte) ([]byte, error) {
	if kemCallbacks == nil {
		return nil, fmt.Errorf("base KEM callbacks cannot be nil")
	}

	kemCtx, err := registerPVEBaseKEMCallbacks(kemCallbacks)
	if err != nil {
		return nil, err
	}
	defer unregisterPVEBaseKEMCallbacks(kemCtx)

	mems, cleanup, err := bytesToOwnedCmems(ek, label, x)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	ekMem := mems[0]
	labelMem := mems[1]
	xMem := mems[2]
	cKEM := C.cbmpc_pve_base_kem_t{
		ctx:   kemCtx,
		encap: (C.cbmpc_pve_kem_encap_fn)(C.goPVEKEMEncap),
		decap: (C.cbmpc_pve_kem_decap_fn)(C.goPVEKEMDecap),
	}

	var outCiphertext C.cmem_t
	if err := codeError("cbmpc_pve_encrypt_with_kem", C.cbmpc_pve_encrypt_with_kem(&cKEM, C.cbmpc_curve_id_t(curve), ekMem, labelMem, xMem, &outCiphertext)); err != nil {
		freeCmem(outCiphertext)
		return nil, err
	}
	runtime.KeepAlive(cKEM)
	return cmemToBytesAndFree(outCiphertext), nil
}

func PVEVerifyWithKEM(curve CurveID, kemCallbacks *PVEBaseKEMCallbacks, ek []byte, ciphertext []byte, qCompressed []byte, label []byte) error {
	if kemCallbacks == nil {
		return fmt.Errorf("base KEM callbacks cannot be nil")
	}

	kemCtx, err := registerPVEBaseKEMCallbacks(kemCallbacks)
	if err != nil {
		return err
	}
	defer unregisterPVEBaseKEMCallbacks(kemCtx)

	mems, cleanup, err := bytesToOwnedCmems(ek, ciphertext, qCompressed, label)
	if err != nil {
		return err
	}
	defer cleanup()
	ekMem := mems[0]
	ciphertextMem := mems[1]
	qCompressedMem := mems[2]
	labelMem := mems[3]
	cKEM := C.cbmpc_pve_base_kem_t{
		ctx:   kemCtx,
		encap: (C.cbmpc_pve_kem_encap_fn)(C.goPVEKEMEncap),
		decap: (C.cbmpc_pve_kem_decap_fn)(C.goPVEKEMDecap),
	}
	err = codeError("cbmpc_pve_verify_with_kem", C.cbmpc_pve_verify_with_kem(&cKEM, C.cbmpc_curve_id_t(curve), ekMem, ciphertextMem, qCompressedMem, labelMem))
	runtime.KeepAlive(cKEM)
	return err
}

func PVEDecryptWithKEM(curve CurveID, kemCallbacks *PVEBaseKEMCallbacks, dk []byte, ek []byte, ciphertext []byte, label []byte) ([]byte, error) {
	if kemCallbacks == nil {
		return nil, fmt.Errorf("base KEM callbacks cannot be nil")
	}

	kemCtx, err := registerPVEBaseKEMCallbacks(kemCallbacks)
	if err != nil {
		return nil, err
	}
	defer unregisterPVEBaseKEMCallbacks(kemCtx)

	mems, cleanup, err := bytesToOwnedCmems(dk, ek, ciphertext, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	dkMem := mems[0]
	ekMem := mems[1]
	ciphertextMem := mems[2]
	labelMem := mems[3]
	cKEM := C.cbmpc_pve_base_kem_t{
		ctx:   kemCtx,
		encap: (C.cbmpc_pve_kem_encap_fn)(C.goPVEKEMEncap),
		decap: (C.cbmpc_pve_kem_decap_fn)(C.goPVEKEMDecap),
	}

	var outX C.cmem_t
	if err := codeError("cbmpc_pve_decrypt_with_kem", C.cbmpc_pve_decrypt_with_kem(&cKEM, C.cbmpc_curve_id_t(curve), dkMem, ekMem, ciphertextMem, labelMem, &outX)); err != nil {
		freeCmem(outX)
		return nil, err
	}
	runtime.KeepAlive(cKEM)
	return cmemToBytesAndFree(outX), nil
}

func PVEGetQ(ciphertext []byte) ([]byte, error) {
	ciphertextMem, cleanup, err := bytesToOwnedCmem(ciphertext)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var outQ C.cmem_t
	if err := codeError("cbmpc_pve_get_Q", C.cbmpc_pve_get_Q(ciphertextMem, &outQ)); err != nil {
		freeCmem(outQ)
		return nil, err
	}
	return cmemToBytesAndFree(outQ), nil
}

func PVEGetLabel(ciphertext []byte) ([]byte, error) {
	ciphertextMem, cleanup, err := bytesToOwnedCmem(ciphertext)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var outLabel C.cmem_t
	if err := codeError("cbmpc_pve_get_Label", C.cbmpc_pve_get_Label(ciphertextMem, &outLabel)); err != nil {
		freeCmem(outLabel)
		return nil, err
	}
	return cmemToBytesAndFree(outLabel), nil
}

// -------------------------------
// PVE-Batch
// -------------------------------

func PVEBatchEncrypt(basePKE *PVEBasePKECallbacks, curve CurveID, ek []byte, label []byte, xs [][]byte) ([]byte, error) {
	xsMems, cleanupXS, err := byteSlicesToOwnedCmems(xs)
	if err != nil {
		return nil, err
	}
	defer cleanupXS()

	mems, cleanup, err := bytesToOwnedCmems(ek, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	ekMem := mems[0]
	labelMem := mems[1]
	var outCiphertext C.cmem_t

	if basePKE != nil {
		basePKECtx, err := registerPVEBasePKECallbacks(basePKE)
		if err != nil {
			return nil, err
		}
		defer unregisterPVEBasePKECallbacks(basePKECtx)

		cBasePKE := C.cbmpc_pve_base_pke_t{
			ctx:     basePKECtx,
			encrypt: (C.cbmpc_pve_base_pke_encrypt_fn)(C.goPVEBasePKEEncrypt),
			decrypt: (C.cbmpc_pve_base_pke_decrypt_fn)(C.goPVEBasePKEDecrypt),
		}
		if err := codeError("cbmpc_pve_batch_encrypt", C.cbmpc_pve_batch_encrypt(&cBasePKE, C.cbmpc_curve_id_t(curve), ekMem, labelMem, xsMems, &outCiphertext)); err != nil {
			freeCmem(outCiphertext)
			return nil, err
		}
		runtime.KeepAlive(cBasePKE)
		return cmemToBytesAndFree(outCiphertext), nil
	}

	if err := codeError("cbmpc_pve_batch_encrypt", C.cbmpc_pve_batch_encrypt(nil, C.cbmpc_curve_id_t(curve), ekMem, labelMem, xsMems, &outCiphertext)); err != nil {
		freeCmem(outCiphertext)
		return nil, err
	}
	return cmemToBytesAndFree(outCiphertext), nil
}

func PVEBatchVerify(basePKE *PVEBasePKECallbacks, curve CurveID, ek []byte, ciphertext []byte, qsCompressed [][]byte, label []byte) error {
	qsMems, cleanupQS, err := byteSlicesToOwnedCmems(qsCompressed)
	if err != nil {
		return err
	}
	defer cleanupQS()

	mems, cleanup, err := bytesToOwnedCmems(ek, ciphertext, label)
	if err != nil {
		return err
	}
	defer cleanup()
	ekMem := mems[0]
	ciphertextMem := mems[1]
	labelMem := mems[2]

	if basePKE != nil {
		basePKECtx, err := registerPVEBasePKECallbacks(basePKE)
		if err != nil {
			return err
		}
		defer unregisterPVEBasePKECallbacks(basePKECtx)

		cBasePKE := C.cbmpc_pve_base_pke_t{
			ctx:     basePKECtx,
			encrypt: (C.cbmpc_pve_base_pke_encrypt_fn)(C.goPVEBasePKEEncrypt),
			decrypt: (C.cbmpc_pve_base_pke_decrypt_fn)(C.goPVEBasePKEDecrypt),
		}
		err = codeError("cbmpc_pve_batch_verify", C.cbmpc_pve_batch_verify(&cBasePKE, C.cbmpc_curve_id_t(curve), ekMem, ciphertextMem, qsMems, labelMem))
		runtime.KeepAlive(cBasePKE)
		return err
	}

	return codeError("cbmpc_pve_batch_verify", C.cbmpc_pve_batch_verify(nil, C.cbmpc_curve_id_t(curve), ekMem, ciphertextMem, qsMems, labelMem))
}

func PVEBatchDecrypt(basePKE *PVEBasePKECallbacks, curve CurveID, dk []byte, ek []byte, ciphertext []byte, label []byte) ([][]byte, error) {
	mems, cleanup, err := bytesToOwnedCmems(dk, ek, ciphertext, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	dkMem := mems[0]
	ekMem := mems[1]
	ciphertextMem := mems[2]
	labelMem := mems[3]
	var outXS C.cmems_t

	if basePKE != nil {
		basePKECtx, err := registerPVEBasePKECallbacks(basePKE)
		if err != nil {
			return nil, err
		}
		defer unregisterPVEBasePKECallbacks(basePKECtx)

		cBasePKE := C.cbmpc_pve_base_pke_t{
			ctx:     basePKECtx,
			encrypt: (C.cbmpc_pve_base_pke_encrypt_fn)(C.goPVEBasePKEEncrypt),
			decrypt: (C.cbmpc_pve_base_pke_decrypt_fn)(C.goPVEBasePKEDecrypt),
		}
		if err := codeError("cbmpc_pve_batch_decrypt", C.cbmpc_pve_batch_decrypt(&cBasePKE, C.cbmpc_curve_id_t(curve), dkMem, ekMem, ciphertextMem, labelMem, &outXS)); err != nil {
			freeCmems(outXS)
			return nil, err
		}
		runtime.KeepAlive(cBasePKE)
		return cmemsToByteSlicesAndFree(outXS)
	}

	if err := codeError("cbmpc_pve_batch_decrypt", C.cbmpc_pve_batch_decrypt(nil, C.cbmpc_curve_id_t(curve), dkMem, ekMem, ciphertextMem, labelMem, &outXS)); err != nil {
		freeCmems(outXS)
		return nil, err
	}
	return cmemsToByteSlicesAndFree(outXS)
}

func PVEBatchDecryptRSAOAEPHSM(curve CurveID, dkHandle []byte, ek []byte, ciphertext []byte, label []byte, callbacks *PVERSAOAEPHSMDecapCallbacks) ([][]byte, error) {
	if callbacks == nil {
		return nil, fmt.Errorf("RSA-OAEP HSM callbacks cannot be nil")
	}

	cbCtx, err := registerPVERSAOAEPHSMDecapCallbacks(callbacks)
	if err != nil {
		return nil, err
	}
	defer unregisterPVERSAOAEPHSMDecapCallbacks(cbCtx)

	mems, cleanup, err := bytesToOwnedCmems(dkHandle, ek, ciphertext, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	dkHandleMem := mems[0]
	ekMem := mems[1]
	ciphertextMem := mems[2]
	labelMem := mems[3]
	cCallbacks := C.cbmpc_pve_rsa_oaep_hsm_decap_t{
		ctx:   cbCtx,
		decap: (C.cbmpc_pve_rsa_oaep_hsm_decap_fn)(C.goPVERSAOAEPHSMDecap),
	}

	var outXS C.cmems_t
	if err := codeError("cbmpc_pve_batch_decrypt_rsa_oaep_hsm", C.cbmpc_pve_batch_decrypt_rsa_oaep_hsm(C.cbmpc_curve_id_t(curve), dkHandleMem, ekMem, ciphertextMem, labelMem, &cCallbacks, &outXS)); err != nil {
		freeCmems(outXS)
		return nil, err
	}
	runtime.KeepAlive(cCallbacks)
	return cmemsToByteSlicesAndFree(outXS)
}

func PVEBatchDecryptECIESP256HSM(curve CurveID, dkHandle []byte, ek []byte, ciphertext []byte, label []byte, callbacks *PVEECIESP256HSMECDHCallbacks) ([][]byte, error) {
	if callbacks == nil {
		return nil, fmt.Errorf("ECIES P256 HSM callbacks cannot be nil")
	}

	cbCtx, err := registerPVEECIESP256HSMECDHCallbacks(callbacks)
	if err != nil {
		return nil, err
	}
	defer unregisterPVEECIESP256HSMECDHCallbacks(cbCtx)

	mems, cleanup, err := bytesToOwnedCmems(dkHandle, ek, ciphertext, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	dkHandleMem := mems[0]
	ekMem := mems[1]
	ciphertextMem := mems[2]
	labelMem := mems[3]
	cCallbacks := C.cbmpc_pve_ecies_p256_hsm_ecdh_t{
		ctx:  cbCtx,
		ecdh: (C.cbmpc_pve_ecies_p256_hsm_ecdh_fn)(C.goPVEECIESP256HSMECDH),
	}

	var outXS C.cmems_t
	if err := codeError("cbmpc_pve_batch_decrypt_ecies_p256_hsm", C.cbmpc_pve_batch_decrypt_ecies_p256_hsm(C.cbmpc_curve_id_t(curve), dkHandleMem, ekMem, ciphertextMem, labelMem, &cCallbacks, &outXS)); err != nil {
		freeCmems(outXS)
		return nil, err
	}
	runtime.KeepAlive(cCallbacks)
	return cmemsToByteSlicesAndFree(outXS)
}

func PVEBatchGetCount(ciphertext []byte) (int, error) {
	ciphertextMem, cleanup, err := bytesToOwnedCmem(ciphertext)
	if err != nil {
		return 0, err
	}
	defer cleanup()
	var outCount C.int
	if err := codeError("cbmpc_pve_batch_get_count", C.cbmpc_pve_batch_get_count(ciphertextMem, &outCount)); err != nil {
		return 0, err
	}
	return int(outCount), nil
}

func PVEBatchGetQs(ciphertext []byte) ([][]byte, error) {
	ciphertextMem, cleanup, err := bytesToOwnedCmem(ciphertext)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var outQs C.cmems_t
	if err := codeError("cbmpc_pve_batch_get_Qs", C.cbmpc_pve_batch_get_Qs(ciphertextMem, &outQs)); err != nil {
		freeCmems(outQs)
		return nil, err
	}
	return cmemsToByteSlicesAndFree(outQs)
}

func PVEBatchGetLabel(ciphertext []byte) ([]byte, error) {
	ciphertextMem, cleanup, err := bytesToOwnedCmem(ciphertext)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var outLabel C.cmem_t
	if err := codeError("cbmpc_pve_batch_get_Label", C.cbmpc_pve_batch_get_Label(ciphertextMem, &outLabel)); err != nil {
		freeCmem(outLabel)
		return nil, err
	}
	return cmemToBytesAndFree(outLabel), nil
}

// -------------------------------
// PVE-AC
// -------------------------------

func PVEACEncrypt(basePKE *PVEBasePKECallbacks, curve CurveID, access *AccessStructure, leafNames []string, leafEKs [][]byte, label []byte, xs [][]byte) ([]byte, error) {
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, err
	}
	defer compiledAccess.free()

	if len(leafNames) != len(leafEKs) {
		return nil, fmt.Errorf("leafNames and leafEKs must have the same length")
	}

	leafNamesArray, leafNamesCStrings, err := createCStringArray(leafNames)
	if err != nil {
		return nil, err
	}
	defer freeCStringArray(leafNamesArray, leafNamesCStrings)

	leafEKsArray, _, cleanupLeafEKs, err := byteSlicesToOwnedCmemArray(leafEKs)
	if err != nil {
		return nil, err
	}
	defer cleanupLeafEKs()

	xsMems, cleanupXS, err := byteSlicesToOwnedCmems(xs)
	if err != nil {
		return nil, err
	}
	defer cleanupXS()

	labelMem, cleanupLabel, err := bytesToOwnedCmem(label)
	if err != nil {
		return nil, err
	}
	defer cleanupLabel()
	var outCiphertext C.cmem_t

	if basePKE != nil {
		basePKECtx, err := registerPVEBasePKECallbacks(basePKE)
		if err != nil {
			return nil, err
		}
		defer unregisterPVEBasePKECallbacks(basePKECtx)

		cBasePKE := C.cbmpc_pve_base_pke_t{
			ctx:     basePKECtx,
			encrypt: (C.cbmpc_pve_base_pke_encrypt_fn)(C.goPVEBasePKEEncrypt),
			decrypt: (C.cbmpc_pve_base_pke_decrypt_fn)(C.goPVEBasePKEDecrypt),
		}
		if err := codeError("cbmpc_pve_ac_encrypt", C.cbmpc_pve_ac_encrypt(&cBasePKE, C.cbmpc_curve_id_t(curve), &compiledAccess.cStruct, (**C.char)(leafNamesArray), (*C.cmem_t)(leafEKsArray), C.int(len(leafNames)), labelMem, xsMems, &outCiphertext)); err != nil {
			freeCmem(outCiphertext)
			return nil, err
		}
		runtime.KeepAlive(cBasePKE)
		return cmemToBytesAndFree(outCiphertext), nil
	}

	if err := codeError("cbmpc_pve_ac_encrypt", C.cbmpc_pve_ac_encrypt(nil, C.cbmpc_curve_id_t(curve), &compiledAccess.cStruct, (**C.char)(leafNamesArray), (*C.cmem_t)(leafEKsArray), C.int(len(leafNames)), labelMem, xsMems, &outCiphertext)); err != nil {
		freeCmem(outCiphertext)
		return nil, err
	}
	return cmemToBytesAndFree(outCiphertext), nil
}

func PVEACVerify(basePKE *PVEBasePKECallbacks, curve CurveID, access *AccessStructure, leafNames []string, leafEKs [][]byte, ciphertext []byte, qsCompressed [][]byte, label []byte) error {
	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return err
	}
	defer compiledAccess.free()

	if len(leafNames) != len(leafEKs) {
		return fmt.Errorf("leafNames and leafEKs must have the same length")
	}

	leafNamesArray, leafNamesCStrings, err := createCStringArray(leafNames)
	if err != nil {
		return err
	}
	defer freeCStringArray(leafNamesArray, leafNamesCStrings)

	leafEKsArray, _, cleanupLeafEKs, err := byteSlicesToOwnedCmemArray(leafEKs)
	if err != nil {
		return err
	}
	defer cleanupLeafEKs()

	qsMems, cleanupQS, err := byteSlicesToOwnedCmems(qsCompressed)
	if err != nil {
		return err
	}
	defer cleanupQS()

	mems, cleanup, err := bytesToOwnedCmems(ciphertext, label)
	if err != nil {
		return err
	}
	defer cleanup()
	ciphertextMem := mems[0]
	labelMem := mems[1]

	if basePKE != nil {
		basePKECtx, err := registerPVEBasePKECallbacks(basePKE)
		if err != nil {
			return err
		}
		defer unregisterPVEBasePKECallbacks(basePKECtx)

		cBasePKE := C.cbmpc_pve_base_pke_t{
			ctx:     basePKECtx,
			encrypt: (C.cbmpc_pve_base_pke_encrypt_fn)(C.goPVEBasePKEEncrypt),
			decrypt: (C.cbmpc_pve_base_pke_decrypt_fn)(C.goPVEBasePKEDecrypt),
		}
		err = codeError("cbmpc_pve_ac_verify", C.cbmpc_pve_ac_verify(&cBasePKE, C.cbmpc_curve_id_t(curve), &compiledAccess.cStruct, (**C.char)(leafNamesArray), (*C.cmem_t)(leafEKsArray), C.int(len(leafNames)), ciphertextMem, qsMems, labelMem))
		runtime.KeepAlive(cBasePKE)
		return err
	}

	return codeError("cbmpc_pve_ac_verify", C.cbmpc_pve_ac_verify(nil, C.cbmpc_curve_id_t(curve), &compiledAccess.cStruct, (**C.char)(leafNamesArray), (*C.cmem_t)(leafEKsArray), C.int(len(leafNames)), ciphertextMem, qsMems, labelMem))
}

func PVEACPartyDecryptRow(basePKE *PVEBasePKECallbacks, curve CurveID, access *AccessStructure, ciphertext []byte, rowIndex int, leafName string, dk []byte, label []byte) ([]byte, error) {
	if rowIndex < 0 || rowIndex > math.MaxInt32 {
		return nil, fmt.Errorf("rowIndex %d out of range [0, %d]", rowIndex, math.MaxInt32)
	}
	if leafName == "" {
		return nil, fmt.Errorf("leafName cannot be empty")
	}
	if strings.IndexByte(leafName, 0) != -1 {
		return nil, fmt.Errorf("leafName cannot contain NUL bytes")
	}

	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, err
	}
	defer compiledAccess.free()

	mems, cleanup, err := bytesToOwnedCmems(ciphertext, dk, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	ciphertextMem := mems[0]
	dkMem := mems[1]
	labelMem := mems[2]
	cLeafName := C.CString(leafName)
	if cLeafName == nil {
		return nil, fmt.Errorf("failed to allocate C string for leafName")
	}
	defer C.free(unsafe.Pointer(cLeafName))

	var outShare C.cmem_t

	if basePKE != nil {
		basePKECtx, err := registerPVEBasePKECallbacks(basePKE)
		if err != nil {
			return nil, err
		}
		defer unregisterPVEBasePKECallbacks(basePKECtx)

		cBasePKE := C.cbmpc_pve_base_pke_t{
			ctx:     basePKECtx,
			encrypt: (C.cbmpc_pve_base_pke_encrypt_fn)(C.goPVEBasePKEEncrypt),
			decrypt: (C.cbmpc_pve_base_pke_decrypt_fn)(C.goPVEBasePKEDecrypt),
		}
		if err := codeError("cbmpc_pve_ac_partial_decrypt_attempt", C.cbmpc_pve_ac_partial_decrypt_attempt(&cBasePKE, C.cbmpc_curve_id_t(curve), &compiledAccess.cStruct, ciphertextMem, C.int(rowIndex), cLeafName, dkMem, labelMem, &outShare)); err != nil {
			freeCmem(outShare)
			return nil, err
		}
		runtime.KeepAlive(cBasePKE)
		return cmemToBytesAndFree(outShare), nil
	}

	if err := codeError("cbmpc_pve_ac_partial_decrypt_attempt", C.cbmpc_pve_ac_partial_decrypt_attempt(nil, C.cbmpc_curve_id_t(curve), &compiledAccess.cStruct, ciphertextMem, C.int(rowIndex), cLeafName, dkMem, labelMem, &outShare)); err != nil {
		freeCmem(outShare)
		return nil, err
	}
	return cmemToBytesAndFree(outShare), nil
}

func PVEACPartyDecryptRowRSAOAEPHSM(curve CurveID, access *AccessStructure, ciphertext []byte, rowIndex int, leafName string, dkHandle []byte, ek []byte, label []byte, callbacks *PVERSAOAEPHSMDecapCallbacks) ([]byte, error) {
	if callbacks == nil {
		return nil, fmt.Errorf("RSA-OAEP HSM callbacks cannot be nil")
	}
	if rowIndex < 0 || rowIndex > math.MaxInt32 {
		return nil, fmt.Errorf("rowIndex %d out of range [0, %d]", rowIndex, math.MaxInt32)
	}
	if leafName == "" {
		return nil, fmt.Errorf("leafName cannot be empty")
	}
	if strings.IndexByte(leafName, 0) != -1 {
		return nil, fmt.Errorf("leafName cannot contain NUL bytes")
	}

	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, err
	}
	defer compiledAccess.free()

	cbCtx, err := registerPVERSAOAEPHSMDecapCallbacks(callbacks)
	if err != nil {
		return nil, err
	}
	defer unregisterPVERSAOAEPHSMDecapCallbacks(cbCtx)

	mems, cleanup, err := bytesToOwnedCmems(ciphertext, dkHandle, ek, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	ciphertextMem := mems[0]
	dkHandleMem := mems[1]
	ekMem := mems[2]
	labelMem := mems[3]
	cLeafName := C.CString(leafName)
	if cLeafName == nil {
		return nil, fmt.Errorf("failed to allocate C string for leafName")
	}
	defer C.free(unsafe.Pointer(cLeafName))

	cCallbacks := C.cbmpc_pve_rsa_oaep_hsm_decap_t{
		ctx:   cbCtx,
		decap: (C.cbmpc_pve_rsa_oaep_hsm_decap_fn)(C.goPVERSAOAEPHSMDecap),
	}

	var outShare C.cmem_t
	if err := codeError("cbmpc_pve_ac_partial_decrypt_attempt_rsa_oaep_hsm", C.cbmpc_pve_ac_partial_decrypt_attempt_rsa_oaep_hsm(C.cbmpc_curve_id_t(curve), &compiledAccess.cStruct, ciphertextMem, C.int(rowIndex), cLeafName, dkHandleMem, ekMem, labelMem, &cCallbacks, &outShare)); err != nil {
		freeCmem(outShare)
		return nil, err
	}
	runtime.KeepAlive(cCallbacks)
	return cmemToBytesAndFree(outShare), nil
}

func PVEACPartyDecryptRowECIESP256HSM(curve CurveID, access *AccessStructure, ciphertext []byte, rowIndex int, leafName string, dkHandle []byte, ek []byte, label []byte, callbacks *PVEECIESP256HSMECDHCallbacks) ([]byte, error) {
	if callbacks == nil {
		return nil, fmt.Errorf("ECIES P256 HSM callbacks cannot be nil")
	}
	if rowIndex < 0 || rowIndex > math.MaxInt32 {
		return nil, fmt.Errorf("rowIndex %d out of range [0, %d]", rowIndex, math.MaxInt32)
	}
	if leafName == "" {
		return nil, fmt.Errorf("leafName cannot be empty")
	}
	if strings.IndexByte(leafName, 0) != -1 {
		return nil, fmt.Errorf("leafName cannot contain NUL bytes")
	}

	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, err
	}
	defer compiledAccess.free()

	cbCtx, err := registerPVEECIESP256HSMECDHCallbacks(callbacks)
	if err != nil {
		return nil, err
	}
	defer unregisterPVEECIESP256HSMECDHCallbacks(cbCtx)

	mems, cleanup, err := bytesToOwnedCmems(ciphertext, dkHandle, ek, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	ciphertextMem := mems[0]
	dkHandleMem := mems[1]
	ekMem := mems[2]
	labelMem := mems[3]
	cLeafName := C.CString(leafName)
	if cLeafName == nil {
		return nil, fmt.Errorf("failed to allocate C string for leafName")
	}
	defer C.free(unsafe.Pointer(cLeafName))

	cCallbacks := C.cbmpc_pve_ecies_p256_hsm_ecdh_t{
		ctx:  cbCtx,
		ecdh: (C.cbmpc_pve_ecies_p256_hsm_ecdh_fn)(C.goPVEECIESP256HSMECDH),
	}

	var outShare C.cmem_t
	if err := codeError("cbmpc_pve_ac_partial_decrypt_attempt_ecies_p256_hsm", C.cbmpc_pve_ac_partial_decrypt_attempt_ecies_p256_hsm(C.cbmpc_curve_id_t(curve), &compiledAccess.cStruct, ciphertextMem, C.int(rowIndex), cLeafName, dkHandleMem, ekMem, labelMem, &cCallbacks, &outShare)); err != nil {
		freeCmem(outShare)
		return nil, err
	}
	runtime.KeepAlive(cCallbacks)
	return cmemToBytesAndFree(outShare), nil
}

func PVEACAggregateToRestoreRow(basePKE *PVEBasePKECallbacks, curve CurveID, access *AccessStructure, quorumLeafNames []string, quorumShares [][]byte, ciphertext []byte, rowIndex int, label []byte) ([][]byte, error) {
	if len(quorumLeafNames) != len(quorumShares) {
		return nil, fmt.Errorf("quorumLeafNames and quorumShares must have the same length")
	}
	if rowIndex < 0 || rowIndex > math.MaxInt32 {
		return nil, fmt.Errorf("rowIndex %d out of range [0, %d]", rowIndex, math.MaxInt32)
	}

	compiledAccess, err := compileAccessStructure(access)
	if err != nil {
		return nil, err
	}
	defer compiledAccess.free()

	quorumLeafNamesArray, quorumLeafNamesCStrings, err := createCStringArray(quorumLeafNames)
	if err != nil {
		return nil, err
	}
	defer freeCStringArray(quorumLeafNamesArray, quorumLeafNamesCStrings)

	quorumSharesArray, _, cleanupQuorumShares, err := byteSlicesToOwnedCmemArray(quorumShares)
	if err != nil {
		return nil, err
	}
	defer cleanupQuorumShares()

	mems, cleanup, err := bytesToOwnedCmems(ciphertext, label)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	ciphertextMem := mems[0]
	labelMem := mems[1]
	var outXS C.cmems_t

	if basePKE != nil {
		basePKECtx, err := registerPVEBasePKECallbacks(basePKE)
		if err != nil {
			return nil, err
		}
		defer unregisterPVEBasePKECallbacks(basePKECtx)

		cBasePKE := C.cbmpc_pve_base_pke_t{
			ctx:     basePKECtx,
			encrypt: (C.cbmpc_pve_base_pke_encrypt_fn)(C.goPVEBasePKEEncrypt),
			decrypt: (C.cbmpc_pve_base_pke_decrypt_fn)(C.goPVEBasePKEDecrypt),
		}
		if err := codeError("cbmpc_pve_ac_combine", C.cbmpc_pve_ac_combine(&cBasePKE, C.cbmpc_curve_id_t(curve), &compiledAccess.cStruct, (**C.char)(quorumLeafNamesArray), (*C.cmem_t)(quorumSharesArray), C.int(len(quorumLeafNames)), ciphertextMem, C.int(rowIndex), labelMem, &outXS)); err != nil {
			freeCmems(outXS)
			return nil, err
		}
		runtime.KeepAlive(cBasePKE)
		return cmemsToByteSlicesAndFree(outXS)
	}

	if err := codeError("cbmpc_pve_ac_combine", C.cbmpc_pve_ac_combine(nil, C.cbmpc_curve_id_t(curve), &compiledAccess.cStruct, (**C.char)(quorumLeafNamesArray), (*C.cmem_t)(quorumSharesArray), C.int(len(quorumLeafNames)), ciphertextMem, C.int(rowIndex), labelMem, &outXS)); err != nil {
		freeCmems(outXS)
		return nil, err
	}
	return cmemsToByteSlicesAndFree(outXS)
}

func PVEACGetCount(ciphertext []byte) (int, error) {
	ciphertextMem, cleanup, err := bytesToOwnedCmem(ciphertext)
	if err != nil {
		return 0, err
	}
	defer cleanup()
	var outCount C.int
	if err := codeError("cbmpc_pve_ac_get_count", C.cbmpc_pve_ac_get_count(ciphertextMem, &outCount)); err != nil {
		return 0, err
	}
	return int(outCount), nil
}

func PVEACGetQs(ciphertext []byte) ([][]byte, error) {
	ciphertextMem, cleanup, err := bytesToOwnedCmem(ciphertext)
	if err != nil {
		return nil, err
	}
	defer cleanup()
	var outQs C.cmems_t
	if err := codeError("cbmpc_pve_ac_get_Qs", C.cbmpc_pve_ac_get_Qs(ciphertextMem, &outQs)); err != nil {
		freeCmems(outQs)
		return nil, err
	}
	return cmemsToByteSlicesAndFree(outQs)
}
