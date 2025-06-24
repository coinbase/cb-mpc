package cgobinding

import (
	"fmt"
)

/*
#include <stdint.h>
#include "zk.h"
*/
import "C"

func ZK_DL_Prove(Q ECCPointRef, w []byte, sessionID []byte, aux uint64) ([]byte, error) {
	var proof C.cmem_t

	cErr := C.zk_dl_prove(
		(*C.ecc_point_ref)(&Q),
		cmem(w),
		cmem(sessionID),
		C.uint64_t(aux),
		&proof,
	)

	if cErr != 0 {
		return nil, fmt.Errorf("ZK DL prove failed: %d", int(cErr))
	}

	return CMEMGet(proof), nil
}

func ZK_DL_Verify(Q ECCPointRef, proof []byte, sessionID []byte, aux uint64) (bool, error) {
	cErr := C.zk_dl_verify(
		(*C.ecc_point_ref)(&Q),
		cmem(proof),
		cmem(sessionID),
		C.uint64_t(aux),
	)

	if cErr == 0 {
		return true, nil
	} else {
		return false, nil // Not an error, just verification failed
	}
}
