package cgobinding

import (
	"context"
	"crypto/x509"
	"fmt"
	"sync"
	"unsafe"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mtls"
)

/*
#include <stdlib.h>
#include <string.h>
#include "network.h"

extern int callback_send(void*, int, cmem_t);
extern int callback_receive(void*, int, cmem_t*);
extern int callback_receive_all(void*, int*, int, cmems_t*);

static void set_callbacks(data_transport_callbacks_t* dt_callbacks)
{
	dt_callbacks->send_fun = callback_send;
	dt_callbacks->receive_fun = callback_receive;
	dt_callbacks->receive_all_fun = callback_receive_all;
}
*/
import "C"

// Error constants matching C++ definitions
const (
	NetworkSuccess      = 0
	NetworkError        = -1
	NetworkParamError   = -2
	NetworkMemoryError  = -3
	NetworkInvalidState = -4
)

// Transport interface aliases for backward compatibility
type IDataTransport = transport.Messenger
type MTLSDataTransport = mtls.MTLSMessenger
type PartyConfig = mtls.PartyConfig
type Config = mtls.Config

type MpcPartySetRef C.mpc_party_set_ref

// ---------------------------------------------------------------------------
// Party-set helpers (moved from ecdsamp.go)

// NewPartySet allocates a new party set and returns its opaque reference.
func NewPartySet() MpcPartySetRef {
	set := C.new_party_set()
	return MpcPartySetRef(set)
}

// Add inserts a party index into the set.
func (ps *MpcPartySetRef) Add(partyIdx int) {
	C.party_set_add((*C.mpc_party_set_ref)(ps), C.int(partyIdx))
}

// Free releases the underlying C++ party_set_t instance.
func (ps *MpcPartySetRef) Free() {
	C.free_party_set(C.mpc_party_set_ref(*ps))
}

// Data transport implementation management with better type safety
var dtImplMap = sync.Map{}

func SetDTImpl(dtImpl any) (unsafe.Pointer, error) {
	if dtImpl == nil {
		return nil, fmt.Errorf("data transport implementation cannot be nil")
	}
	ptr := C.malloc(1)
	if ptr == nil {
		return nil, fmt.Errorf("failed to allocate memory for data transport pointer")
	}
	dtImplMap.Store(ptr, dtImpl)
	return ptr, nil
}

func FreeDTImpl(ptr unsafe.Pointer) error {
	if ptr == nil {
		return nil // Not an error to free a nil pointer
	}
	_, loaded := dtImplMap.LoadAndDelete(ptr)
	if !loaded {
		return fmt.Errorf("attempt to free unknown data transport pointer")
	}
	C.free(ptr)
	return nil
}

func GetDTImpl(ptr unsafe.Pointer) (any, error) {
	if ptr == nil {
		return nil, fmt.Errorf("cannot get implementation from nil pointer")
	}
	dtImpl, ok := dtImplMap.Load(ptr)
	if !ok {
		return nil, fmt.Errorf("failed to load dtImpl from pointer")
	}
	return dtImpl, nil
}

// Callback functions
var callbacks C.data_transport_callbacks_t

//export callback_send
func callback_send(ptr unsafe.Pointer, receiver C.int, message C.cmem_t) C.int {
	dtImpl, err := GetDTImpl(ptr)
	if err != nil {
		return C.int(NetworkError)
	}

	transport, ok := dtImpl.(*IDataTransport)
	if !ok {
		return C.int(NetworkError)
	}

	var goBytes []byte
	if message.size > 0 && message.data != nil {
		goBytes = C.GoBytes(unsafe.Pointer(message.data), message.size)
	}

	if err := (*transport).MessageSend(context.Background(), int(receiver), goBytes); err != nil {
		return C.int(NetworkError)
	}

	return C.int(NetworkSuccess)
}

//export callback_receive
func callback_receive(ptr unsafe.Pointer, sender C.int, message *C.cmem_t) C.int {
	dtImpl, err := GetDTImpl(ptr)
	if err != nil {
		return C.int(NetworkError)
	}

	transport, ok := dtImpl.(*IDataTransport)
	if !ok {
		return C.int(NetworkError)
	}

	received, err := (*transport).MessageReceive(context.Background(), int(sender))
	if err != nil {
		return C.int(NetworkError)
	}

	message.size = C.int(len(received))
	if len(received) > 0 {
		buf := C.malloc(C.size_t(len(received)))
		if buf == nil {
			return C.int(NetworkMemoryError)
		}
		C.memcpy(buf, unsafe.Pointer(&received[0]), C.size_t(len(received)))
		message.data = (*C.uint8_t)(buf)
	} else {
		message.data = nil
	}

	return C.int(NetworkSuccess)
}

// Array manipulation utilities - optimized for performance
var (
	cIntSize = int(unsafe.Sizeof(C.int(0)))
	cPtrSize = int(unsafe.Sizeof(unsafe.Pointer(nil)))
)

func arrGetIntC(arr unsafe.Pointer, index int) int {
	ptr := (*C.int)(unsafe.Pointer(uintptr(arr) + uintptr(index*cIntSize)))
	return int(*ptr)
}

func arrSetIntC(arr unsafe.Pointer, index int, value int) {
	ptr := (*C.int)(unsafe.Pointer(uintptr(arr) + uintptr(index*cIntSize)))
	*ptr = C.int(value)
}

func arrSetBytePtrC(arr unsafe.Pointer, index int, value unsafe.Pointer) {
	ptr := (*unsafe.Pointer)(unsafe.Pointer(uintptr(arr) + uintptr(index*cPtrSize)))
	*ptr = value
}

//export callback_receive_all
func callback_receive_all(ptr unsafe.Pointer, senders *C.int, senderCount C.int, messages *C.cmems_t) C.int {
	dtImpl, err := GetDTImpl(ptr)
	if err != nil {
		return C.int(NetworkError)
	}

	transport, ok := dtImpl.(*IDataTransport)
	if !ok {
		return C.int(NetworkError)
	}

	count := int(senderCount)
	if count == 0 {
		messages.count = 0
		messages.data = nil
		messages.sizes = nil
		return C.int(NetworkSuccess)
	}

	sendersArray := make([]int, count)
	for i := 0; i < count; i++ {
		sendersArray[i] = arrGetIntC(unsafe.Pointer(senders), i)
	}

	received, err := (*transport).MessagesReceive(context.Background(), sendersArray)
	if err != nil {
		return C.int(NetworkError)
	}

	if len(received) != count {
		return C.int(NetworkError)
	}

	// Build flattened cmems_t
	total := 0
	for i := 0; i < count; i++ {
		total += len(received[i])
	}

	var dataPtr unsafe.Pointer
	if total > 0 {
		dataPtr = C.malloc(C.size_t(total))
		if dataPtr == nil {
			return C.int(NetworkMemoryError)
		}
	}

	sizesPtr := C.malloc(C.size_t(count) * C.size_t(cIntSize))
	if sizesPtr == nil && count > 0 {
		if dataPtr != nil {
			C.free(dataPtr)
		}
		return C.int(NetworkMemoryError)
	}

	offset := 0
	for i := 0; i < count; i++ {
		arrSetIntC(sizesPtr, i, len(received[i]))
		if len(received[i]) > 0 {
			C.memcpy(unsafe.Pointer(uintptr(dataPtr)+uintptr(offset)), unsafe.Pointer(&received[i][0]), C.size_t(len(received[i])))
			offset += len(received[i])
		}
	}

	messages.count = C.int(count)
	messages.data = (*C.uint8_t)(dataPtr)
	messages.sizes = (*C.int)(sizesPtr)

	return C.int(NetworkSuccess)
}

// Helper function to create C string arrays safely
func createCStringArray(strings []string) (unsafe.Pointer, []*C.char, error) {
	if len(strings) == 0 {
		return nil, nil, fmt.Errorf("string array cannot be empty")
	}

	cArray := C.malloc(C.size_t(len(strings)) * C.size_t(unsafe.Sizeof(uintptr(0))))
	if cArray == nil {
		return nil, nil, fmt.Errorf("failed to allocate memory for string array")
	}

	cSlice := (*[1 << 30]unsafe.Pointer)(cArray)[:len(strings):len(strings)]
	cStrs := make([]*C.char, len(strings))

	for i, str := range strings {
		if str == "" {
			C.free(cArray)
			for j := 0; j < i; j++ {
				C.free(unsafe.Pointer(cStrs[j]))
			}
			return nil, nil, fmt.Errorf("string at index %d cannot be empty", i)
		}
		cStrs[i] = C.CString(str)
		cSlice[i] = unsafe.Pointer(cStrs[i])
	}

	return cArray, cStrs, nil
}

func freeCStringArray(cArray unsafe.Pointer, cStrs []*C.char) {
	if cArray != nil {
		C.free(cArray)
	}
	for _, cStr := range cStrs {
		if cStr != nil {
			C.free(unsafe.Pointer(cStr))
		}
	}
}

// Job2P represents a 2-party job with improved resource management
type Job2P struct {
	dtImplPtr unsafe.Pointer
	cJob      *C.job_2p_ref
}

func (j *Job2P) GetCJob() *C.job_2p_ref {
	return j.cJob
}

func NewJob2P(dt IDataTransport, roleIndex int, pnames []string) (Job2P, error) {
	if len(pnames) != 2 {
		return Job2P{}, fmt.Errorf("NewJob2P requires exactly 2 pnames, got %d", len(pnames))
	}

	if dt == nil {
		return Job2P{}, fmt.Errorf("data transport cannot be nil")
	}

	ptr, err := SetDTImpl(&dt)
	if err != nil {
		return Job2P{}, fmt.Errorf("failed to set data transport implementation: %w", err)
	}

	cArray, cStrs, err := createCStringArray(pnames)
	if err != nil {
		FreeDTImpl(ptr)
		return Job2P{}, fmt.Errorf("failed to create C string array: %w", err)
	}
	defer freeCStringArray(cArray, cStrs)

	cJobRef := C.new_job_2p(&callbacks, ptr, C.int(roleIndex), (**C.char)(cArray), C.int(len(pnames)))
	if cJobRef == nil {
		FreeDTImpl(ptr)
		return Job2P{}, fmt.Errorf("failed to create 2P job")
	}

	return Job2P{ptr, cJobRef}, nil
}

func (j *Job2P) Free() {
	if j.cJob != nil {
		C.free_job_2p(j.cJob)
		j.cJob = nil
	}
	if j.dtImplPtr != nil {
		FreeDTImpl(j.dtImplPtr) // Ignore error on cleanup
		j.dtImplPtr = nil
	}
}

func (j *Job2P) IsPeer1() bool {
	return j.cJob != nil && C.is_peer1(j.cJob) != 0
}

func (j *Job2P) IsPeer2() bool {
	return j.cJob != nil && C.is_peer2(j.cJob) != 0
}

func (j *Job2P) IsRoleIndex(roleIndex int) bool {
	return j.cJob != nil && C.is_role_index(j.cJob, C.int(roleIndex)) != 0
}

func (j *Job2P) GetRoleIndex() int {
	if j.cJob == nil {
		return -1
	}
	return int(C.get_role_index(j.cJob))
}

func (j *Job2P) Message(sender, receiver int, msg []byte) ([]byte, error) {
	if j.cJob == nil {
		return nil, fmt.Errorf("job is not initialized")
	}

	if j.IsRoleIndex(sender) {
		var cmsg C.cmem_t
		if len(msg) > 0 {
			cmsg.data = (*C.uint8_t)(&msg[0])
			cmsg.size = C.int(len(msg))
		} else {
			cmsg.data = nil
			cmsg.size = 0
		}
		cErr := C.mpc_2p_send(j.cJob, C.int(receiver), cmsg)
		if cErr != NetworkSuccess {
			return nil, fmt.Errorf("2p send failed: error code %d", cErr)
		}
		return msg, nil
	} else if j.IsRoleIndex(receiver) {
		var cmsg C.cmem_t
		cErr := C.mpc_2p_receive(j.cJob, C.int(sender), &cmsg)
		if cErr != NetworkSuccess {
			return nil, fmt.Errorf("2p receive failed: error code %d", cErr)
		}

		if cmsg.data == nil || cmsg.size == 0 {
			return []byte{}, nil
		}

		result := C.GoBytes(unsafe.Pointer(cmsg.data), cmsg.size)
		C.free(unsafe.Pointer(cmsg.data))
		return result, nil
	}

	return nil, fmt.Errorf("caller needs to be either sender (%d) or receiver (%d), current role is %d",
		sender, receiver, j.GetRoleIndex())
}

// JobMP represents a multi-party job with improved resource management
type JobMP struct {
	dtImplPtr unsafe.Pointer
	cJob      *C.job_mp_ref
}

func (j *JobMP) GetCJob() *C.job_mp_ref {
	return j.cJob
}

func NewJobMP(dt IDataTransport, partyCount int, roleIndex int, pnames []string) (JobMP, error) {
	if len(pnames) != partyCount {
		return JobMP{}, fmt.Errorf("NewJobMP requires pnames array length (%d) to match partyCount (%d)",
			len(pnames), partyCount)
	}

	if dt == nil {
		return JobMP{}, fmt.Errorf("data transport cannot be nil")
	}

	if partyCount <= 0 {
		return JobMP{}, fmt.Errorf("partyCount must be positive, got %d", partyCount)
	}

	if roleIndex < 0 || roleIndex >= partyCount {
		return JobMP{}, fmt.Errorf("roleIndex (%d) must be in range [0, %d)", roleIndex, partyCount)
	}

	ptr, err := SetDTImpl(&dt)
	if err != nil {
		return JobMP{}, fmt.Errorf("failed to set data transport implementation: %w", err)
	}

	cArray, cStrs, err := createCStringArray(pnames)
	if err != nil {
		FreeDTImpl(ptr)
		return JobMP{}, fmt.Errorf("failed to create C string array: %w", err)
	}
	defer freeCStringArray(cArray, cStrs)

	cJobRef := C.new_job_mp(&callbacks, ptr, C.int(partyCount), C.int(roleIndex), (**C.char)(cArray), C.int(len(pnames)))
	if cJobRef == nil {
		FreeDTImpl(ptr)
		return JobMP{}, fmt.Errorf("failed to create MP job")
	}

	return JobMP{ptr, cJobRef}, nil
}

func (j *JobMP) Free() {
	if j.cJob != nil {
		C.free_job_mp(j.cJob)
		j.cJob = nil
	}
	if j.dtImplPtr != nil {
		FreeDTImpl(j.dtImplPtr) // Ignore error on cleanup
		j.dtImplPtr = nil
	}
}

func (j *JobMP) IsParty(partyIndex int) bool {
	return j.cJob != nil && C.is_party(j.cJob, C.int(partyIndex)) != 0
}

func (j *JobMP) GetPartyIndex() int {
	if j.cJob == nil {
		return -1
	}
	return int(C.get_party_idx(j.cJob))
}

func (j *JobMP) GetNParties() int {
	if j.cJob == nil {
		return -1
	}
	return int(C.get_n_parties(j.cJob))
}

// Transport factory functions
func NewMTLSDataTransport(config Config) (IDataTransport, error) {
	return mtls.NewMTLSMessenger(config)
}

func PartyNameFromCertificate(cert *x509.Certificate) (string, error) {
	return mtls.PartyNameFromCertificate(cert)
}

func init() {
	C.set_callbacks(&callbacks)
}
