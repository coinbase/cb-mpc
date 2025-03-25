package cblib

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
#cgo CFLAGS:    -I${SRCDIR}/../network
#cgo CXXFLAGS:  -I${SRCDIR}/../network
#cgo CFLAGS:    -I/usr/local/opt/cbmpc/include
#cgo CXXFLAGS:  -I/usr/local/opt/cbmpc/include
#cgo LDFLAGS:   -L/usr/local/opt/cbmpc/lib
#cgo LDFLAGS:   -lcbmpc
#cgo linux,!android                 LDFLAGS:   /usr/local/lib64/libcrypto.a

#include <string.h>
#include "cblib.h"

*/
import "C"
import (
	"bytes"
	"fmt"
	"unsafe"

	"github.com/coinbase/cb-mpc/cb-mpc-go/network"
)

// CGO generates separate C types for each Go package, so we need the conversion functions.
func cjob(job network.JobSession2P) *C.JOB_SESSION_2P_PTR {
	return (*C.JOB_SESSION_2P_PTR)(unsafe.Pointer(job.GetCJob()))
}

func cjobmp(job network.JobSessionMP) *C.JOB_SESSION_MP_PTR {
	return (*C.JOB_SESSION_MP_PTR)(unsafe.Pointer(job.GetCJob()))
}

// ---------- Utility functions that create equivalent of cmem_t and cmems_t types in go

type CMEM = C.cmem_t

func cmem(in []byte) CMEM {
	var mem CMEM
	mem.size = C.int(len(in))
	if mem.size != 0 {
		mem.data = (*C.uchar)(&in[0])
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
				for j := 0; j < l; j++ {
					data[k+j] = in[i][j]
				}
				k += l
			}
			mems.data = (*C.uchar)(&data[0])
		}
	}
	return mems
}

var cIntSize = int(unsafe.Sizeof(C.int(0)))

func arrGetIntC(arr unsafe.Pointer, index int) int {
	ptrValue := uintptr(arr) + uintptr(index*cIntSize)
	ptr := (*C.int)(unsafe.Pointer(ptrValue))
	return int(*ptr)
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
		l := arrGetIntC(unsafe.Pointer(cmems.sizes), i)
		out[i] = C.GoBytes(unsafe.Pointer(p), C.int(l))
		p += uintptr(l)
		n += l
	}
	C.memset(unsafe.Pointer(cmems.data), 0, C.ulong(n))
	C.free(unsafe.Pointer(cmems.data))
	C.free(unsafe.Pointer(cmems.sizes))
	return out
}

// ------------------------------ Go Wrappers --------------------------------------
// conversion:
//  - Create a go type for example: type MPC_ECDSA2PC_KEY_PTR C.MPC_ECDSA2PC_KEY_PTR
//  - Create go wrapper functions
//  - Useful tools for converting types are as follows:
//     - C.int
//     - cjob
//     - CMEM and CMEMS
//     - the _PTR types
//     - []byte

type MPC_ECDSA2PC_KEY_PTR C.MPC_ECDSA2PC_KEY_PTR
type MPC_ECDSAMPC_KEY_PTR C.MPC_ECDSAMPC_KEY_PTR
type CRYPTO_SS_NODE_PTR C.CRYPTO_SS_NODE_PTR
type CRYPTO_PRV_KEY_PTR C.CRYPTO_PRV_KEY_PTR

func (k *MPC_ECDSA2PC_KEY_PTR) Free() {
	C.free_mpc_ecdsa2p_key(C.MPC_ECDSA2PC_KEY_PTR(*k))
}

// =========== ECDSA2PC =====================

func DistributedKeyGen(job network.JobSession2P, curveCode int) (MPC_ECDSA2PC_KEY_PTR, error) {
	var key MPC_ECDSA2PC_KEY_PTR
	cErr := C.mpc_ecdsa2p_dkg(cjob(job), C.int(curveCode), (*C.MPC_ECDSA2PC_KEY_PTR)(&key))
	if cErr != 0 {
		return key, fmt.Errorf("ECDSA-2p keygen failed, %v", cErr)
	}
	return key, nil
}

func Refresh(job network.JobSession2P, key MPC_ECDSA2PC_KEY_PTR) (MPC_ECDSA2PC_KEY_PTR, error) {
	var newKey MPC_ECDSA2PC_KEY_PTR
	cErr := C.mpc_ecdsa2p_refresh(cjob(job), (*C.MPC_ECDSA2PC_KEY_PTR)(&key), (*C.MPC_ECDSA2PC_KEY_PTR)(&newKey))
	if cErr != 0 {
		return newKey, fmt.Errorf("ECDSA-2p refresh failed, %v", cErr)
	}
	return newKey, nil
}

func Sign(job network.JobSession2P, sessionID []byte, key MPC_ECDSA2PC_KEY_PTR, msgs [][]byte) ([][]byte, error) {
	var sigs CMEMS
	cErr := C.mpc_ecdsa2p_sign(cjob(job), cmem(sessionID), (*C.MPC_ECDSA2PC_KEY_PTR)(&key), cmems(msgs), &sigs)
	if cErr != 0 {
		return nil, fmt.Errorf("ECDSA-2p sign failed, %v", cErr)
	}
	return CMEMSGet(sigs), nil
}

// =========== ECDSAMPC =====================
func MPC_ecdsampc_dkg(job network.JobSessionMP, curveCode int) (MPC_ECDSAMPC_KEY_PTR, error) {
	var key MPC_ECDSAMPC_KEY_PTR
	cErr := C.mpc_ecdsampc_dkg(cjobmp(job), C.int(curveCode), (*C.MPC_ECDSAMPC_KEY_PTR)(&key))
	if cErr != 0 {
		return key, fmt.Errorf("ECDSA-mp keygen failed, %v", cErr)
	}
	return key, nil
}

func MPC_ecdsampc_sign(job network.JobSessionMP, key MPC_ECDSAMPC_KEY_PTR, msgMem []byte, sigReceiver int) ([]byte, error) {
	var sigMem CMEM
	cErr := C.mpc_ecdsampc_sign(cjobmp(job), (*C.MPC_ECDSAMPC_KEY_PTR)(&key), cmem(msgMem), C.int(sigReceiver), &sigMem)
	if cErr != 0 {
		return nil, fmt.Errorf("ECDSA-mp sign failed, %v", cErr)
	}
	return CMEMGet(sigMem), nil
}

// =========== ZKP =====================
func ZK_DL_Example() int {
	return int(C.ZK_DL_Example())
}

// =========== PVE =====================

type NodeType int

const (
	NodeType_NONE = iota
	NodeType_LEAF
	NodeType_AND
	NodeType_OR
	NodeType_THRESHOLD
)

func NewEncKeyPairs(count int) ([][]byte, [][]byte, error) {
	var privateKeys CMEMS
	var publicKeys CMEMS
	err := C.get_n_enc_keypairs(C.int(count), &privateKeys, &publicKeys)
	if err != 0 {
		return nil, nil, fmt.Errorf("generating key pairs failed, %v", err)
	}
	return CMEMSGet(privateKeys), CMEMSGet(publicKeys), nil
}

func NewECKeyPairs(count int) ([][]byte, [][]byte, error) {
	var privateKeys CMEMS
	var publicKeys CMEMS
	err := C.get_n_ec_keypairs(C.int(count), &privateKeys, &publicKeys)
	if err != 0 {
		return nil, nil, fmt.Errorf("generating key pairs failed, %v", err)
	}
	return CMEMSGet(privateKeys), CMEMSGet(publicKeys), nil
}

func NewNode(nodeType NodeType, nodeName string, threshold int) CRYPTO_SS_NODE_PTR {
	node := C.new_node(C.int(nodeType), cmem([]byte(nodeName)), C.int(threshold))
	return CRYPTO_SS_NODE_PTR(node)
}

func AddChild(parent, child CRYPTO_SS_NODE_PTR) {
	C.add_child((*C.CRYPTO_SS_NODE_PTR)(&parent), (*C.CRYPTO_SS_NODE_PTR)(&child))
}

func PVE_quorum_encrypt(root CRYPTO_SS_NODE_PTR, publicKeys [][]byte, publicKeysCount int, xs [][]byte, xsCount int, label string) ([]byte, error) {
	var out CMEM
	err := C.pve_quorum_encrypt((*C.CRYPTO_SS_NODE_PTR)(&root), cmems(publicKeys), C.int(publicKeysCount), cmems(xs), C.int(xsCount), C.CString(label), &out)
	if err != 0 {
		return nil, fmt.Errorf("pve quorum encrypt failed: %v", err)
	}

	return CMEMGet(out), nil
}

func PVE_quorum_decrypt(root CRYPTO_SS_NODE_PTR, privateKeys [][]byte, privateKeysCount int, publicKeys [][]byte, publicKeysCount int, pveBundle []byte, Xs [][]byte, xsCount int, label string) ([][]byte, error) {
	var out CMEMS
	err := C.pve_quorum_decrypt((*C.CRYPTO_SS_NODE_PTR)(&root), cmems(privateKeys), C.int(privateKeysCount), cmems(publicKeys), C.int(publicKeysCount), cmem(pveBundle), cmems(Xs), C.int(xsCount), C.CString(label), &out)
	if err != 0 {
		return nil, fmt.Errorf("pve quorum decrypt failed: %v", err)
	}

	return CMEMSGet(out), nil
}

func PVE_Test() error {
	root := NewNode(NodeType_AND, "root", 0)
	root_child_1 := NewNode(NodeType_LEAF, "leaf1", 0)
	root_child_2 := NewNode(NodeType_THRESHOLD, "th", 2)
	root_child_2_child_1 := NewNode(NodeType_LEAF, "leaf2", 0)
	root_child_2_child_2 := NewNode(NodeType_LEAF, "leaf3", 0)
	root_child_2_child_3 := NewNode(NodeType_LEAF, "leaf4", 0)
	AddChild(root_child_2, root_child_2_child_1)
	AddChild(root_child_2, root_child_2_child_2)
	AddChild(root_child_2, root_child_2_child_3)
	AddChild(root, root_child_1)
	AddChild(root, root_child_2)

	// Generate the same number of keys as there are leaves in the tree
	leafCount := 4
	privKeys, pubKeys, err := NewEncKeyPairs(leafCount)
	if err != nil {
		return err
	}
	// Generate the data to be backed up
	inputLabel := "test-data"
	dataCount := 20
	xs, Xs, err := NewECKeyPairs(dataCount)
	if err != nil {
		return err
	}
	pveBundle, err := PVE_quorum_encrypt(root, pubKeys, leafCount, xs, dataCount, inputLabel)
	if err != nil {
		return fmt.Errorf("PVE encrypt failed, %v", err)
	}
	decryptedxs, err := PVE_quorum_decrypt(root, privKeys, leafCount, pubKeys, leafCount, pveBundle, Xs, dataCount, inputLabel)
	if err != nil {
		return fmt.Errorf("PVE encrypt failed, %v", err)
	}
	for i := 0; i < dataCount; i++ {
		if !bytes.Equal(decryptedxs[i], xs[i]) {
			return fmt.Errorf("decrypted value does not match the original value")
		}
	}
	return nil
}

// =========== Utilities =====================
func SerializeECDSAShares(keyshares []MPC_ECDSAMPC_KEY_PTR) ([][]byte, [][]byte, error) {
	xs := make([][]byte, len(keyshares))
	Qs := make([][]byte, len(keyshares))
	for i := 0; i < len(keyshares); i++ {
		var xMem CMEM
		var QMem CMEM
		err := C.convert_ecdsa_share_to_bn_t_share((*C.MPC_ECDSAMPC_KEY_PTR)(&keyshares[i]), &xMem, &QMem)
		if err != 0 {
			return nil, nil, fmt.Errorf("pve quorum decrypt failed: %v", err)
		}
		xs[i] = CMEMGet(xMem)
		Qs[i] = CMEMGet(QMem)
	}
	return xs, Qs, nil
}

func MPC_ecdsa_mpc_public_key_to_string(key MPC_ECDSAMPC_KEY_PTR) ([]byte, []byte, error) {
	var xMem CMEM
	var yMem CMEM
	err := C.ecdsa_mpc_public_key_to_string((*C.MPC_ECDSAMPC_KEY_PTR)(&key), &xMem, &yMem)
	if err != 0 {
		return nil, nil, fmt.Errorf("getting ecdsa mpc public key failed, %v", err)
	}
	return CMEMGet(xMem), CMEMGet(yMem), nil
}
