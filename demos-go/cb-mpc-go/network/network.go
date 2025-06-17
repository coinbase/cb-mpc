package network

import (
	"fmt"
	"sync"
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
#include "network.h"

extern int callback_send(void*, int, uint8_t*, int);
extern int callback_receive(void*, int, uint8_t**, int*);
extern int callback_receive_all(void*, int*, int, uint8_t**, int*);

static void set_callbacks(data_transport_callbacks_t* dt_callbacks)
{
	dt_callbacks->send_fun = callback_send;
	dt_callbacks->receive_fun = callback_receive;
	dt_callbacks->receive_all_fun = callback_receive_all;
}
*/
import "C"

type IDataTransport interface {
	MessageSend(receiver int, buffer []byte) error
	MessageReceive(sender int) ([]byte, error)
	// MessagesReceive receive messages from a number of senders. It waits until all messages are ready.
	MessagesReceive(senders []int) ([][]byte, error)
}

// Map to store the pointer to IDataTransport implementations. Because we cannot pass a Go implementations of an interface to C.
var dtImplMap = sync.Map{}

func SetDTImpl(dtImpl any) any {
	ptr := C.malloc(1)
	dtImplMap.Store(ptr, dtImpl)
	return ptr
}

func FreeDTImpl(ptr any) {
	dtImplMap.LoadAndDelete(ptr)
	C.free(ptr.(unsafe.Pointer))
}

func GetDTImpl(ptr any) any {
	dtImpl, ok := dtImplMap.Load(ptr)
	if !ok {
		panic("failed to load dtImpl from ptr")
	}
	return dtImpl
}

// This is the single callback struct implementations that will be used by all IDataTransport implementations
var callbacks C.data_transport_callbacks_t

//export callback_send
func callback_send(ptr unsafe.Pointer, receiver C.int, message *C.uint8_t, message_size C.int) C.int {
	idtPtr := GetDTImpl(ptr).(*IDataTransport)

	goBytes := C.GoBytes(unsafe.Pointer(message), message_size)
	err := (*idtPtr).MessageSend(int(receiver), goBytes)
	if err != nil {
		return C.int(1)
	}

	return C.int(0)
}

//export callback_receive
func callback_receive(ptr unsafe.Pointer, sender C.int, message **C.uint8_t, messageSize *C.int) C.int {
	idtPtr := GetDTImpl(ptr).(*IDataTransport)

	received, err := (*idtPtr).MessageReceive(int(sender))
	if err != nil {
		return C.int(1)
	}

	*messageSize = C.int(len(received))
	*message = (*C.uint8_t)(&received[0])

	return C.int(0)
}

var cIntSize = int(unsafe.Sizeof(C.int(0)))
var cPtrSize = int(unsafe.Sizeof(unsafe.Pointer(nil)))

func arrGetIntC(arr unsafe.Pointer, index int) int {
	ptrValue := uintptr(arr) + uintptr(index*cIntSize)
	ptr := (*C.int)(unsafe.Pointer(ptrValue))
	return int(*ptr)
}

func arrSetIntC(arr unsafe.Pointer, index int, value int) {
	ptrValue := uintptr(arr) + uintptr(index*cIntSize)
	ptr := (*C.int)(unsafe.Pointer(ptrValue))
	*ptr = C.int(value)
}

func arrSetBytePtrC(arr unsafe.Pointer, index int, value unsafe.Pointer) {
	ptrValue := uintptr(arr) + uintptr(index*cPtrSize)
	ptr := (*unsafe.Pointer)(unsafe.Pointer(ptrValue))
	*ptr = value
}

//export callback_receive_all
func callback_receive_all(ptr unsafe.Pointer, senders *C.int, senderCount C.int, messages **C.uint8_t, messageSizes *C.int) C.int {
	idtPtr := GetDTImpl(ptr).(*IDataTransport)

	sendersArray := make([]int, senderCount)
	for i := 0; i < int(senderCount); i++ {
		sendersArray[i] = arrGetIntC(unsafe.Pointer(senders), i)
	}

	received, err := (*idtPtr).MessagesReceive(sendersArray)
	if err != nil {
		return C.int(1)
	}

	for i := 0; i < int(senderCount); i++ {
		arrSetIntC(unsafe.Pointer(messageSizes), i, len(received[i]))
		arrSetBytePtrC(unsafe.Pointer(messages), i, unsafe.Pointer(&received[i][0]))
	}

	return C.int(0)
}

func init() {
	C.set_callbacks(&callbacks)
}

// ---------------- JobSession2P

type JobSession2P struct {
	dtImplPtr unsafe.Pointer
	cJob      *C.JOB_SESSION_2P_PTR
}

func (js *JobSession2P) GetCJob() *C.JOB_SESSION_2P_PTR {
	return js.cJob
}

func NewJobSession2P(dt IDataTransport, roleIndex int, pnames []string) JobSession2P {
	if len(pnames) != 2 {
		panic("NewJobSession2P requires exactly 2 pnames")
	}

	ptr := SetDTImpl(&dt)

	// Create C string array manually using C.malloc
	cPidsArray := C.malloc(C.size_t(len(pnames)) * C.size_t(unsafe.Sizeof(uintptr(0))))
	defer C.free(cPidsArray)

	// Convert to slice of unsafe.Pointer
	cPidsSlice := (*[1 << 30]unsafe.Pointer)(cPidsArray)[:len(pnames):len(pnames)]

	// Convert each string and store pointer
	for i, pid := range pnames {
		cStr := C.CString(pid)
		defer C.free(unsafe.Pointer(cStr))
		cPidsSlice[i] = unsafe.Pointer(cStr)
	}

	return JobSession2P{ptr.(unsafe.Pointer), C.new_job_session_2p(&callbacks, ptr.(unsafe.Pointer), C.int(roleIndex), (**C.char)(cPidsArray), C.int(len(pnames)))}
}

func (js *JobSession2P) Free() {
	FreeDTImpl(js.dtImplPtr)
	C.free_job_session_2p(js.cJob)
}

func (js *JobSession2P) IsPeer1() bool {
	return C.is_peer1(js.cJob) != 0
}

func (js *JobSession2P) IsPeer2() bool {
	return C.is_peer2(js.cJob) != 0
}

func (js *JobSession2P) IsRoleIndex(roleIndex int) bool {
	return C.is_role_index(js.cJob, C.int(roleIndex)) != 0
}

func (js *JobSession2P) GetRoleIndex() int {
	return int(C.get_role_index(js.cJob))
}

func (js *JobSession2P) Message(sender, receiver int, msg []byte) ([]byte, error) {
	if js.IsRoleIndex(sender) {
		messageSize := C.int(len(msg))
		message := (*C.uint8_t)(&msg[0])
		cErr := C.mpc_2p_send(js.cJob, C.int(receiver), message, messageSize)
		if cErr != 0 {
			return nil, fmt.Errorf("2p send failed, %v", cErr)
		}
		return msg, nil
	} else if js.IsRoleIndex(receiver) {
		var message *C.uint8_t
		var messageSize C.int
		cErr := C.mpc_2p_receive(js.cJob, C.int(sender), &message, &messageSize)
		if cErr != 0 {
			return nil, fmt.Errorf("2p receive failed, %v", cErr)
		}
		return C.GoBytes(unsafe.Pointer(message), messageSize), nil
		//return nil, nil
	} else {
		return nil, fmt.Errorf("caller needs to be a sender or receiver")
	}
}

// ---------------- JobSessionMP

type JobSessionMP struct {
	dtImplPtr unsafe.Pointer
	cJob      *C.JOB_SESSION_MP_PTR
}

func (js *JobSessionMP) GetCJob() *C.JOB_SESSION_MP_PTR {
	return js.cJob
}

func NewJobSessionMP(dt IDataTransport, partyCount int, roleIndex int, jobSessionId int, pnames []string) JobSessionMP {
	if len(pnames) != partyCount {
		panic("NewJobSessionMP requires pnames array length to match partyCount")
	}

	ptr := SetDTImpl(&dt)

	// Create C string array manually using C.malloc
	cPnamesArray := C.malloc(C.size_t(len(pnames)) * C.size_t(unsafe.Sizeof(uintptr(0))))
	defer C.free(cPnamesArray)

	// Convert to slice of unsafe.Pointer
	cPnamesSlice := (*[1 << 30]unsafe.Pointer)(cPnamesArray)[:len(pnames):len(pnames)]

	// Convert each string and store pointer
	for i, pname := range pnames {
		cStr := C.CString(pname)
		defer C.free(unsafe.Pointer(cStr))
		cPnamesSlice[i] = unsafe.Pointer(cStr)
	}

	return JobSessionMP{ptr.(unsafe.Pointer), C.new_job_session_mp(&callbacks, ptr.(unsafe.Pointer), C.int(partyCount), C.int(roleIndex), C.int(jobSessionId), (**C.char)(cPnamesArray), C.int(len(pnames)))}
}

func (js *JobSessionMP) Free() {
	FreeDTImpl(js.dtImplPtr)
	C.free_job_session_mp(js.cJob)
}

func (js *JobSessionMP) IsParty(partyIndex int) bool {
	return C.is_party(js.cJob, C.int(partyIndex)) != 0
}

func (js *JobSessionMP) GetPartyIndex() int {
	return int(C.get_party_idx(js.cJob))
}

type CMEM = C.cmem_t

func CMEMGet(cmem CMEM) []byte {
	if cmem.data == nil {
		return nil
	}
	out := C.GoBytes(unsafe.Pointer(cmem.data), cmem.size)
	C.memset(unsafe.Pointer(cmem.data), 0, C.ulong(cmem.size))
	C.free(unsafe.Pointer(cmem.data))
	return out
}

func AgreeRandom(job JobSession2P, bitLen int) ([]byte, error) {
	var out CMEM
	cErr := C.mpc_agree_random(job.GetCJob(), C.int(bitLen), &out)
	if cErr != 0 {
		return nil, fmt.Errorf("mpc_agree_random failed, %v", cErr)
	}
	return CMEMGet(out), nil
}
