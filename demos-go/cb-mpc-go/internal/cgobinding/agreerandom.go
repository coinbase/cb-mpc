package cgobinding

import (
	"fmt"
)

/*
#include <stdlib.h>
#include <string.h>
#include "agree_random.h"
#include "cblib.h"
*/
import "C"

// AgreeRandom executes the agree random protocol between two parties
func AgreeRandom(job Job2P, bitLen int) ([]byte, error) {
	var out CMEM
	cErr := C.mpc_agree_random(job.GetCJob(), C.int(bitLen), &out)
	if cErr != 0 {
		return nil, fmt.Errorf("mpc_agree_random failed, %v", cErr)
	}
	return CMEMGet(out), nil
}
