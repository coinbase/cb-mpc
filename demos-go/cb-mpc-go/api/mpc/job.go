package mpc

import (
	"fmt"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// Job2P is an opaque handle for a 2-party MPC job.
// Users create it via NewJob2P and pass it to protocol APIs.
// Always call Free when finished.
type Job2P struct {
	inner cgobinding.Job2P
}

// NewJob2P constructs a two-party job.
// messenger – network layer implementation.
// roleIndex – 0 or 1 for the local party.
// pnames    – names of the two parties (len == 2).
func NewJob2P(messenger transport.Messenger, roleIndex int, pnames []string) (*Job2P, error) {
	inner, err := cgobinding.NewJob2P(messenger, roleIndex, pnames)
	if err != nil {
		return nil, err
	}
	return &Job2P{inner: inner}, nil
}

// Free releases C-side resources.
func (j *Job2P) Free() { j.inner.Free() }

// Close satisfies io.Closer by delegating to Free().
func (j *Job2P) Close() error {
	j.Free()
	return nil
}

// BroadcastToOthers sends the provided payload to the other party.
// For Job2P this means exactly one peer (1 - selfIndex).
func (j *Job2P) BroadcastToOthers(payload []byte) error {
	sender := j.GetRoleIndex()
	if sender < 0 || sender > 1 {
		return fmt.Errorf("invalid role index %d", sender)
	}
	receiver := 1 - sender
	_, err := j.inner.Message(sender, receiver, payload)
	return err
}

// IsRoleIndex returns true if the given index matches this party.
func (j *Job2P) IsRoleIndex(idx int) bool { return j.inner.IsRoleIndex(idx) }

// GetRoleIndex returns the current party index (0 or 1).
func (j *Job2P) GetRoleIndex() int { return j.inner.GetRoleIndex() }

// cgo exposes the underlying binding (internal).
func (j *Job2P) cgo() cgobinding.Job2P { return j.inner }
