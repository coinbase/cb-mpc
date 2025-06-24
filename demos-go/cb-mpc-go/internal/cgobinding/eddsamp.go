package cgobinding

/*
#include "eddsamp.h"
*/
import "C"

import "fmt"

// -----------------------------------------------------------------------------
// EdDSA-MPC signing binding (key management shared with ECDSA)
// -----------------------------------------------------------------------------

type Mpc_eddsampc_key_ref = Mpc_eckey_mp_ref // underlying type is identical

// MPC_eddsampc_sign performs the N-party EdDSA signing protocol.
// It mirrors MPC_ecdsampc_sign but uses the EdDSA Schnorr variant internally.
func MPC_eddsampc_sign(job JobMP, key Mpc_eckey_mp_ref, msgMem []byte, sigReceiver int) ([]byte, error) {
	var sigMem CMEM
	cErr := C.mpc_eddsampc_sign(job.GetCJob(), (*C.mpc_eckey_mp_ref)(&key), cmem(msgMem), C.int(sigReceiver), &sigMem)
	if cErr != 0 {
		return nil, fmt.Errorf("EdDSA-mp sign failed, %v", cErr)
	}
	return CMEMGet(sigMem), nil
}
