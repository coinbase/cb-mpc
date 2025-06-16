package mocknet

import (
	"fmt"
	"sync"

	"github.com/coinbase/cb-mpc/cb-mpc-go/network"
)

type MPCIO struct {
	Opaque interface{}
}

type MPCPeer struct {
	nParties      int
	roleIndex     int
	dataTransport *DemoDataTransport
}

type MPCRunner struct {
	nParties int
	peers    []*MPCPeer
	isAbort  bool
}

func NewMPCRunner(n int) *MPCRunner {
	runner := &MPCRunner{nParties: n}
	runner.peers = make([]*MPCPeer, n)

	dataTransports := make([]*DemoDataTransport, n)
	for i := 0; i < n; i++ {
		dataTransports[i] = NewDemoDataTransport(i)
	}
	for i := 0; i < n; i++ {
		dataTransports[i].setOuts(dataTransports)
	}

	for i := 0; i < n; i++ {
		runner.peers[i] = &MPCPeer{
			nParties:      n,
			roleIndex:     i,
			dataTransport: dataTransports[i],
		}
	}
	return runner
}

type MPCFunction2P func(net network.JobSession2P, input *MPCIO) (*MPCIO, error)

type MPCFunctionMP func(net network.JobSessionMP, input *MPCIO) (*MPCIO, error)

func (runner *MPCRunner) MPCRun2P(f MPCFunction2P, inputs []*MPCIO) ([]*MPCIO, error) {
	if runner.nParties != 2 {
		return nil, fmt.Errorf("MPCRun2P only supports 2 parties")
	}
	errs := make([]error, runner.nParties)
	outs := make([]*MPCIO, runner.nParties)

	runner.isAbort = false

	var wg sync.WaitGroup
	wg.Add(runner.nParties)
	for i := 0; i < runner.nParties; i++ {
		go func(i int) {
			pids := []string{"party_0", "party_1"}
			job := network.NewJobSession2P(runner.peers[i].dataTransport, i, pids)
			defer job.Free()
			outs[i], errs[i] = f(job, inputs[i])
			if errs[i] != nil { // abort job
				runner.isAbort = true
				for j := 0; j < runner.nParties; j++ {
					runner.peers[j].dataTransport.cond.Broadcast()
				}

			}
			wg.Done()
		}(i)

	}
	wg.Wait()

	// stop job
	runner.isAbort = false
	for i := 0; i < runner.nParties; i++ {
		for j := 0; j < runner.nParties; j++ {
			runner.peers[i].dataTransport.queues[j].Init()
		}
	}

	for _, err := range errs {
		if err != nil {
			return nil, err
		}
	}
	return outs, nil
}

func (runner *MPCRunner) MPCRunMP(f MPCFunctionMP, inputs []*MPCIO) ([]*MPCIO, error) {
	errs := make([]error, runner.nParties)
	outs := make([]*MPCIO, runner.nParties)

	runner.isAbort = false

	var wg sync.WaitGroup
	wg.Add(runner.nParties)
	jobSessionId := 0
	for i := 0; i < runner.nParties; i++ {
		go func(i int) {
			// Generate pid array for multi-party
			pids := make([]string, runner.nParties)
			for j := 0; j < runner.nParties; j++ {
				pids[j] = fmt.Sprintf("party_%d", j)
			}
			job := network.NewJobSessionMP(runner.peers[i].dataTransport, runner.nParties, i, jobSessionId, pids)
			defer job.Free()
			outs[i], errs[i] = f(job, inputs[i])
			if errs[i] != nil { // abort job
				runner.isAbort = true
				for j := 0; j < runner.nParties; j++ {
					runner.peers[j].dataTransport.cond.Broadcast()
				}

			}
			wg.Done()
		}(i)

	}
	wg.Wait()

	// stop job
	runner.isAbort = false
	for i := 0; i < runner.nParties; i++ {
		for j := 0; j < runner.nParties; j++ {
			runner.peers[i].dataTransport.queues[j].Init()
		}
	}

	for _, err := range errs {
		if err != nil {
			return nil, err
		}
	}
	return outs, nil
}
