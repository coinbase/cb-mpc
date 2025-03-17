package mocknet

import (
	"fmt"
	"testing"

	"github.com/coinbase/cb-mpc/cb-mpc-go/network"
	"github.com/stretchr/testify/assert"
)

func Test2PCRunner(t *testing.T) {
	runner := NewMPCRunner(2)
	outputs, err := runner.MPCRun2P(func(job network.JobSession2P, input *MPCIO) (*MPCIO, error) {
		var testMessage []byte
		var err error
		if job.IsPeer1() {
			testMessage = []byte("hi")
		}
		if job.IsPeer2() {
			assert.NotEqual(t, testMessage, []byte("hi"))
		}
		testMessage, err = job.Message(0, 1, testMessage)
		if err != nil {
			return nil, fmt.Errorf("sending hi: %v", err)
		}
		assert.Equal(t, testMessage, []byte("hi"))
		return &MPCIO{Opaque: testMessage}, nil
	}, []*MPCIO{nil, nil})
	assert.NoError(t, err)
	assert.Len(t, outputs, 2)
}
