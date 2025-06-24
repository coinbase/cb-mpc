package mocknet

import (
	"fmt"
	"sync"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// MPCIO represents input/output data for MPC operations
type MPCIO struct {
	Opaque interface{}
}

// MPCPeer represents a single party in the MPC protocol
type MPCPeer struct {
	nParties      int
	roleIndex     int
	dataTransport *MockMessenger
}

// MPCRunner provides utilities for running MPC protocols in a test environment
type MPCRunner struct {
	nParties int
	pnames   []string
	peers    []*MPCPeer
	isAbort  bool
}

// GeneratePartyNames returns the default party name list ("party_0", "party_1", ...)
// for the given number of parties. It is handy for tests and examples that do not
// require custom naming.
func GeneratePartyNames(n int) []string {
	names := make([]string, n)
	for i := 0; i < n; i++ {
		names[i] = fmt.Sprintf("party_%d", i)
	}
	return names
}

// NewMPCRunner creates a new MPCRunner with the specified party names.  The caller
// should pass one name per party, e.g.:
//
//	r := mocknet.NewMPCRunner("alice", "bob")
//
// For convenience, callers can generate the default names via GeneratePartyNames.
func NewMPCRunner(pnames ...string) *MPCRunner {
	n := len(pnames)
	if n == 0 {
		panic("NewMPCRunner requires at least one party name")
	}

	runner := &MPCRunner{nParties: n, pnames: pnames}
	runner.peers = make([]*MPCPeer, n)

	// Create the mock network
	transports := NewMockNetwork(n)

	// Create peers with their respective transports
	for i := 0; i < n; i++ {
		runner.peers[i] = &MPCPeer{
			nParties:      n,
			roleIndex:     i,
			dataTransport: transports[i],
		}
	}
	return runner
}

// MPCFunction2P represents a function for two-party MPC protocols
type MPCFunction2P func(net cgobinding.Job2P, input *MPCIO) (*MPCIO, error)

// MPCFunctionMP represents a function for multi-party MPC protocols
type MPCFunctionMP func(net cgobinding.JobMP, input *MPCIO) (*MPCIO, error)

// Run2P executes a two-party MPC protocol with the given function and inputs
func (runner *MPCRunner) MPCRun2P(f MPCFunction2P, inputs []*MPCIO) ([]*MPCIO, error) {
	if runner.nParties != 2 {
		return nil, fmt.Errorf("Run2P only supports 2 parties, got %d", runner.nParties)
	}
	errs := make([]error, runner.nParties)
	outs := make([]*MPCIO, runner.nParties)

	runner.isAbort = false

	var wg sync.WaitGroup
	wg.Add(runner.nParties)
	for i := 0; i < runner.nParties; i++ {
		go func(i int) {
			defer wg.Done()
			pnames := runner.pnames
			job, err := cgobinding.NewJob2P(runner.peers[i].dataTransport, i, pnames)
			if err != nil {
				errs[i] = fmt.Errorf("failed to create Job2P: %w", err)
				return
			}
			defer job.Free()
			outs[i], errs[i] = f(job, inputs[i])
			if errs[i] != nil { // abort job
				runner.isAbort = true
				for j := 0; j < runner.nParties; j++ {
					runner.peers[j].dataTransport.cond.Broadcast()
				}
			}
		}(i)
	}
	wg.Wait()

	// Clean up after job
	runner.cleanup()

	for _, err := range errs {
		if err != nil {
			return nil, err
		}
	}
	return outs, nil
}

// RunMP executes a multi-party MPC protocol with the given function and inputs
func (runner *MPCRunner) MPCRunMP(f MPCFunctionMP, inputs []*MPCIO) ([]*MPCIO, error) {
	errs := make([]error, runner.nParties)
	outs := make([]*MPCIO, runner.nParties)

	runner.isAbort = false

	var wg sync.WaitGroup
	wg.Add(runner.nParties)
	for i := 0; i < runner.nParties; i++ {
		go func(i int) {
			defer wg.Done()
			// Use the configured party names directly
			job, err := cgobinding.NewJobMP(runner.peers[i].dataTransport, runner.nParties, i, runner.pnames)
			if err != nil {
				errs[i] = fmt.Errorf("failed to create JobMP: %w", err)
				return
			}
			defer job.Free()
			outs[i], errs[i] = f(job, inputs[i])
			if errs[i] != nil { // abort job
				runner.isAbort = true
				for j := 0; j < runner.nParties; j++ {
					runner.peers[j].dataTransport.cond.Broadcast()
				}
			}
		}(i)
	}
	wg.Wait()

	// Clean up after job
	runner.cleanup()

	for _, err := range errs {
		if err != nil {
			return nil, err
		}
	}
	return outs, nil
}

// cleanup resets the runner state and clears message queues
func (runner *MPCRunner) cleanup() {
	runner.isAbort = false
	for i := 0; i < runner.nParties; i++ {
		for j := 0; j < runner.nParties; j++ {
			runner.peers[i].dataTransport.queues[j].Init()
		}
	}
}
