package mpc

import (
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// JobMP is an opaque handle for an N-party MPC job (N>2).
type JobMP struct {
	inner cgobinding.JobMP
}

// NewJobMP constructs a multi-party job.
func NewJobMP(messenger transport.Messenger, partyCount, roleIndex int, pnames []string) (*JobMP, error) {
	inner, err := cgobinding.NewJobMP(messenger, partyCount, roleIndex, pnames)
	if err != nil {
		return nil, err
	}
	return &JobMP{inner: inner}, nil
}

// Free releases resources.
func (j *JobMP) Free() { j.inner.Free() }

// Close implements io.Closer.
func (j *JobMP) Close() error { j.Free(); return nil }

// GetPartyIndex returns this party's index.
func (j *JobMP) GetPartyIndex() int { return j.inner.GetPartyIndex() }

// IsParty checks if the given index matches this party.
func (j *JobMP) IsParty(idx int) bool { return j.inner.IsParty(idx) }

// cgo exposes the underlying binding (internal).
func (j *JobMP) cgo() cgobinding.JobMP { return j.inner }

// NParties returns the total number of parties in this MPC job.
func (j *JobMP) NParties() int { return j.inner.GetNParties() }
