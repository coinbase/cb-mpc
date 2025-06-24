package cgobinding

/*
#include "pve.h"
*/
import "C"

import (
	"fmt"
	"unsafe"
)

// NewEncKeyPairs generates |count| ECIES key pairs suitable for PVE encryption.
// It returns the private keys and the corresponding public keys as byte slices.
func NewEncKeyPairs(count int) ([][]byte, [][]byte, error) {
	var privateKeys CMEMS
	var publicKeys CMEMS
	if rv := C.get_n_enc_keypairs(C.int(count), &privateKeys, &publicKeys); rv != 0 {
		return nil, nil, fmt.Errorf("generating key pairs failed, %v", rv)
	}
	return CMEMSGet(privateKeys), CMEMSGet(publicKeys), nil
}

// NewECKeyPairs generates |count| raw scalar/point key pairs on the default P-256 curve.
func NewECKeyPairs(count int) ([][]byte, [][]byte, error) {
	var privateKeys CMEMS
	var publicKeys CMEMS
	if rv := C.get_n_ec_keypairs(C.int(count), &privateKeys, &publicKeys); rv != 0 {
		return nil, nil, fmt.Errorf("generating key pairs failed, %v", rv)
	}
	return CMEMSGet(privateKeys), CMEMSGet(publicKeys), nil
}

// GenerateBaseEncKeypair generates a single base encryption key pair (pub, prv).
// The returned byte slices hold serialized crypto::pub_key_t and crypto::prv_key_t, respectively.
func GenerateBaseEncKeypair() ([]byte, []byte, error) {
	var prv C.cmem_t
	var pub C.cmem_t

	if rv := C.generate_base_enc_keypair(&prv, &pub); rv != 0 {
		return nil, nil, fmt.Errorf("generate base encryption key pair failed: %v", rv)
	}

	return CMEMGet(pub), CMEMGet(prv), nil
}

// PVE_quorum_encrypt_map performs quorum encryption given an AccessStructure pointer.
func PVE_quorum_encrypt_map(ac C_AcPtr, names [][]byte, pubKeys [][]byte, count int, xs [][]byte, xsCount int, label string, curveCode int) ([]byte, error) {
	var out CMEM
	cLabel := C.CString(label)
	defer C.free(unsafe.Pointer(cLabel))

	rv := C.pve_quorum_encrypt_map((*C.crypto_ss_ac_ref)(&ac), cmems(names), cmems(pubKeys), C.int(count), cmems(xs), C.int(xsCount), cLabel, C.int(curveCode), &out)
	if rv != 0 {
		return nil, fmt.Errorf("pve quorum encrypt (map) failed: %v", rv)
	}
	return CMEMGet(out), nil
}

// PVE_quorum_decrypt_map performs quorum decryption given an AccessStructure pointer.
func PVE_quorum_decrypt_map(ac C_AcPtr, privateKeys [][]byte, privateKeysCount int, publicKeys [][]byte, publicKeysCount int, pveBundle []byte, Xs [][]byte, xsCount int, label string) ([][]byte, error) {
	var out CMEMS
	cLabel := C.CString(label)
	defer C.free(unsafe.Pointer(cLabel))

	rv := C.pve_quorum_decrypt_map((*C.crypto_ss_ac_ref)(&ac), cmems(privateKeys), C.int(privateKeysCount), cmems(publicKeys), C.int(publicKeysCount), cmem(pveBundle), cmems(Xs), C.int(xsCount), cLabel, &out)
	if rv != 0 {
		return nil, fmt.Errorf("pve quorum decrypt (map) failed: %v", rv)
	}
	return CMEMSGet(out), nil
}

// PVE_quorum_verify_map performs public verification (no private keys) given an AccessStructure pointer.
func PVE_quorum_verify_map(ac C_AcPtr, names [][]byte, pubKeys [][]byte, count int, pveBundle []byte, Xs [][]byte, xsCount int, label string) error {
	cLabel := C.CString(label)
	defer C.free(unsafe.Pointer(cLabel))

	rv := C.pve_quorum_verify_map((*C.crypto_ss_ac_ref)(&ac), cmems(names), cmems(pubKeys), C.int(count), cmem(pveBundle), cmems(Xs), C.int(xsCount), cLabel)
	if rv != 0 {
		return fmt.Errorf("pve quorum verify (map) failed: %v", rv)
	}
	return nil
}
