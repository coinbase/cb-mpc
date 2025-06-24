package cgobinding

/*
#include "ecdsamp.h"
#include "eckeymp.h"
*/
import "C"

import (
	"fmt"
)

func MPC_ecdsampc_sign(job JobMP, key Mpc_eckey_mp_ref, msgMem []byte, sigReceiver int) ([]byte, error) {
	var sigMem CMEM
	cErr := C.mpc_ecdsampc_sign(job.GetCJob(), (*C.mpc_eckey_mp_ref)(&key), cmem(msgMem), C.int(sigReceiver), &sigMem)
	if cErr != 0 {
		return nil, fmt.Errorf("ECDSA-mp sign failed, %v", cErr)
	}
	return CMEMGet(sigMem), nil
}

// -----------------------------------------------------------------------------
// Signing with default OT-role map
// -----------------------------------------------------------------------------

func DefaultOTRoleMap(nParties int) [][]int {
	const (
		OT_NO_ROLE  = -1
		OT_SENDER   = 0
		OT_RECEIVER = 1
	)

	otRoleMap := make([][]int, nParties)
	for i := 0; i < nParties; i++ {
		otRoleMap[i] = make([]int, nParties)
		otRoleMap[i][i] = OT_NO_ROLE
	}

	for i := 0; i < nParties; i++ {
		for j := i + 1; j < nParties; j++ {
			otRoleMap[i][j] = OT_SENDER
			otRoleMap[j][i] = OT_RECEIVER
		}
	}

	return otRoleMap
}

func MPC_ecdsampc_sign_default_ot_roles(job JobMP, key Mpc_eckey_mp_ref, msgMem []byte, sigReceiver int, nParties int) ([]byte, error) {
	otRoleMap := DefaultOTRoleMap(nParties)
	// Convert OT role map to the required format (flattened byte slices)
	roleData := make([][]byte, nParties)
	for i := 0; i < nParties; i++ {
		roleData[i] = make([]byte, nParties*4) // 4 bytes per int (little endian)
		for j := 0; j < nParties && j < len(otRoleMap[i]); j++ {
			role := otRoleMap[i][j]
			roleData[i][j*4+0] = byte(role)
			roleData[i][j*4+1] = byte(role >> 8)
			roleData[i][j*4+2] = byte(role >> 16)
			roleData[i][j*4+3] = byte(role >> 24)
		}
	}

	var sigMem CMEM
	cErr := C.mpc_ecdsampc_sign_with_ot_roles(
		job.GetCJob(),
		(*C.mpc_eckey_mp_ref)(&key),
		cmem(msgMem),
		C.int(sigReceiver),
		cmems(roleData),
		C.int(nParties),
		&sigMem)
	if cErr != 0 {
		return nil, fmt.Errorf("ECDSA-mp sign with OT roles failed, %v", cErr)
	}
	return CMEMGet(sigMem), nil
}
