package mpctest

import (
	"container/list"
	"context"
	"errors"
	"net"
	"sync"
)

// MockTransport provides an in-memory transport implementation for tests and local
// demos. It intentionally provides no authentication or confidentiality.
type MockTransport struct {
	myIndex int
	others  []*MockTransport
	mutex   sync.Mutex
	cond    *sync.Cond
	queues  []list.List
	closed  bool
}

// NewMockTransport creates a new MockTransport for the specified party index.
func NewMockTransport(myIndex int) *MockTransport {
	s := &MockTransport{
		myIndex: myIndex,
		closed:  false,
	}
	s.cond = sync.NewCond(&s.mutex)
	return s
}

// setOthers configures connections to other parties.
// This is called by NewMockNetwork to wire up all parties.
func (s *MockTransport) setOthers(sessions []*MockTransport) {
	s.others = sessions
	s.queues = make([]list.List, len(sessions))
}

// Send sends a message to the specified party.
func (s *MockTransport) Send(_ context.Context, toParty int, msg []byte) error {
	if toParty == s.myIndex {
		return errors.New("cannot send to self")
	}

	if toParty < 0 || toParty >= len(s.others) {
		return errors.New("invalid party index")
	}

	receiver := s.others[toParty]
	copied := make([]byte, len(msg))
	copy(copied, msg)
	receiver.mutex.Lock()
	receiver.queues[s.myIndex].PushBack(copied)
	receiver.mutex.Unlock()
	receiver.cond.Broadcast()

	return nil
}

// Receive receives a message from the specified party (blocking).
func (s *MockTransport) Receive(ctx context.Context, fromParty int) ([]byte, error) {
	if ctx == nil {
		ctx = context.Background()
	}
	if fromParty == s.myIndex {
		return nil, errors.New("cannot receive from self")
	}

	if fromParty < 0 || fromParty >= len(s.queues) {
		return nil, errors.New("invalid party index")
	}

	s.mutex.Lock()
	defer s.mutex.Unlock()

	if s.closed {
		return nil, net.ErrClosed
	}

	queue := &s.queues[fromParty]
	if queue.Len() == 0 && ctx.Done() != nil {
		stop := context.AfterFunc(ctx, func() {
			s.mutex.Lock()
			s.cond.Broadcast()
			s.mutex.Unlock()
		})
		defer stop()
	}
	for queue.Len() == 0 {
		if err := ctx.Err(); err != nil {
			return nil, err
		}
		s.cond.Wait()
		if s.closed {
			return nil, net.ErrClosed
		}
	}

	front := queue.Front()
	msg := front.Value.([]byte)
	queue.Remove(front)
	return msg, nil
}

// ReceiveAll receives messages from multiple parties concurrently.
func (s *MockTransport) ReceiveAll(ctx context.Context, fromParties []int) ([][]byte, error) {
	n := len(fromParties)
	messages := make([][]byte, n)
	errs := make([]error, n)

	var wg sync.WaitGroup
	wg.Add(n)

	for i, fromParty := range fromParties {
		go func(idx int, party int) {
			defer wg.Done()
			msg, err := s.Receive(ctx, party)
			messages[idx] = msg
			errs[idx] = err
		}(i, fromParty)
	}

	wg.Wait()

	// Return first error encountered.
	for _, err := range errs {
		if err != nil {
			return nil, err
		}
	}

	return messages, nil
}

// MyIndex returns this party's index.
func (s *MockTransport) MyIndex() int {
	return s.myIndex
}

// PartyCount returns the total number of parties.
func (s *MockTransport) PartyCount() int {
	return len(s.others)
}

// Close closes the session and unblocks any waiting receives.
func (s *MockTransport) Close() error {
	s.mutex.Lock()
	s.closed = true
	s.mutex.Unlock()
	s.cond.Broadcast()
	return nil
}

// NewMockNetwork creates a complete mock network for tests and local demos.
// It returns a slice of MockTransport instances, one for each party.
func NewMockNetwork(nParties int) []*MockTransport {
	sessions := make([]*MockTransport, nParties)
	for i := 0; i < nParties; i++ {
		sessions[i] = NewMockTransport(i)
	}
	for i := 0; i < nParties; i++ {
		sessions[i].setOthers(sessions)
	}
	return sessions
}
