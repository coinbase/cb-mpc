package cgobinding

/*
#include "ecdsa2p.h"
*/
import "C"

import (
	"fmt"
)

// -------------------------- Type Wrappers --------------------------

type Mpc_ecdsa2pc_key_ref C.mpc_ecdsa2pc_key_ref

// Free releases the underlying native key structure.
func (k *Mpc_ecdsa2pc_key_ref) Free() {
	C.free_mpc_ecdsa2p_key(C.mpc_ecdsa2pc_key_ref(*k))
}

// -------------------------- ECDSA 2PC ------------------------------

// DistributedKeyGenCurve performs the two-party ECDSA distributed key
// generation using a native curve reference instead of a numeric identifier.
func DistributedKeyGenCurve(job Job2P, curveRef ECurveRef) (Mpc_ecdsa2pc_key_ref, error) {
	var key Mpc_ecdsa2pc_key_ref
	curveCode := ECurveGetCurveCode(curveRef)
	cErr := C.mpc_ecdsa2p_dkg(job.GetCJob(), C.int(curveCode), (*C.mpc_ecdsa2pc_key_ref)(&key))
	if cErr != 0 {
		return key, fmt.Errorf("ECDSA-2p keygen failed, %v", cErr)
	}
	return key, nil
}

// Refresh re-shares an existing 2-party ECDSA key.
func Refresh(job Job2P, key Mpc_ecdsa2pc_key_ref) (Mpc_ecdsa2pc_key_ref, error) {
	var newKey Mpc_ecdsa2pc_key_ref
	cErr := C.mpc_ecdsa2p_refresh(job.GetCJob(), (*C.mpc_ecdsa2pc_key_ref)(&key), (*C.mpc_ecdsa2pc_key_ref)(&newKey))
	if cErr != 0 {
		return newKey, fmt.Errorf("ECDSA-2p refresh failed, %v", cErr)
	}
	return newKey, nil
}

// Sign produces batch signatures using the two-party ECDSA key.
func Sign(job Job2P, sid []byte, key Mpc_ecdsa2pc_key_ref, msgs [][]byte) ([][]byte, error) {
	var sigs CMEMS
	cErr := C.mpc_ecdsa2p_sign(job.GetCJob(), cmem(sid), (*C.mpc_ecdsa2pc_key_ref)(&key), cmems(msgs), &sigs)
	if cErr != 0 {
		return nil, fmt.Errorf("ECDSA-2p sign failed, %v", cErr)
	}
	return CMEMSGet(sigs), nil
}

// KeyRoleIndex returns the role index (e.g., 0 or 1) for the provided key share.
// A negative return value indicates an error at the native layer.
func KeyRoleIndex(key Mpc_ecdsa2pc_key_ref) (int, error) {
	idx := int(C.mpc_ecdsa2p_key_get_role_index((*C.mpc_ecdsa2pc_key_ref)(&key)))
	if idx < 0 {
		return -1, fmt.Errorf("failed to get role index: %d", idx)
	}
	return idx, nil
}

// KeyQ returns a reference to the public key point Q inside the 2PC key.
// The caller must eventually free the returned ECCPointRef via its Free method.
func KeyQ(key Mpc_ecdsa2pc_key_ref) (ECCPointRef, error) {
	cPoint := C.mpc_ecdsa2p_key_get_Q((*C.mpc_ecdsa2pc_key_ref)(&key))
	if cPoint.opaque == nil {
		return ECCPointRef{}, fmt.Errorf("failed to retrieve Q from key")
	}
	return ECCPointRef(cPoint), nil
}

// KeyXShare returns the secret scalar share x_i as raw bytes (big-endian).
func KeyXShare(key Mpc_ecdsa2pc_key_ref) ([]byte, error) {
	cMem := C.mpc_ecdsa2p_key_get_x_share((*C.mpc_ecdsa2pc_key_ref)(&key))
	if cMem.data == nil || cMem.size == 0 {
		return nil, fmt.Errorf("failed to retrieve x_share from key")
	}
	return CMEMGet(cMem), nil
}

// KeyCurveCode returns the OpenSSL NID of the curve used by the provided key.
func KeyCurveCode(key Mpc_ecdsa2pc_key_ref) (int, error) {
	code := int(C.mpc_ecdsa2p_key_get_curve_code((*C.mpc_ecdsa2pc_key_ref)(&key)))
	if code < 0 {
		return 0, fmt.Errorf("failed to get curve code from key")
	}
	return code, nil
}
