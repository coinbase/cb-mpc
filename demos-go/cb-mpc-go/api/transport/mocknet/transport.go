package mocknet

import (
	"container/list"
	"context"
	"errors"
	"sync"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport"
)

// MockMessenger provides a mock implementation of the Messenger interface for testing.
// It uses in-memory message queues to simulate network communication between parties.
type MockMessenger struct {
	roleIndex int
	outs      []*MockMessenger
	mutex     sync.Mutex
	cond      *sync.Cond
	queues    []list.List
	isAbort   bool
}

// Ensure MockMessenger implements the Messenger interface
var _ transport.Messenger = (*MockMessenger)(nil)

// NewMockMessenger creates a new MockMessenger instance for the specified party role
func NewMockMessenger(roleIndex int) *MockMessenger {
	ctx := &MockMessenger{roleIndex: roleIndex}
	ctx.cond = sync.NewCond(&ctx.mutex)
	ctx.isAbort = false
	return ctx
}

// setOuts configures the connections to other mock transport instances.
// This is used internally by NewMockNetwork to wire up all parties.
func (dt *MockMessenger) setOuts(dts []*MockMessenger) {
	dt.outs = dts
	dt.queues = make([]list.List, len(dts))
}

// MessageSend sends a message to the specified receiver party
func (dt *MockMessenger) MessageSend(_ context.Context, receiverIndex int, buffer []byte) error {
	if receiverIndex == dt.roleIndex {
		return errors.New("cannot send to self")
	}

	receiverDT := dt.outs[receiverIndex]
	receiverDT.mutex.Lock()
	receiverDT.queues[dt.roleIndex].PushBack(buffer)
	receiverDT.mutex.Unlock()
	receiverDT.cond.Broadcast()

	return nil
}

// MessageReceive receives a message from the specified sender party
func (dt *MockMessenger) MessageReceive(_ context.Context, senderIndex int) ([]byte, error) {
	if senderIndex == dt.roleIndex {
		return nil, errors.New("cannot receive from self")
	}

	dt.mutex.Lock()
	defer dt.mutex.Unlock()

	if dt.isAbort {
		return nil, errors.New("aborted")
	}
	queue := &dt.queues[senderIndex]
	for queue.Len() == 0 {
		dt.cond.Wait()
		if dt.isAbort {
			return nil, errors.New("aborted")
		}
	}
	front := queue.Front()
	receivedMsg := front.Value.([]byte)
	queue.Remove(front)
	return receivedMsg, nil
}

// MessagesReceive receives messages from multiple sender parties concurrently
func (dt *MockMessenger) MessagesReceive(ctx context.Context, senderIndices []int) (receivedMsgs [][]byte, err error) {
	n := len(senderIndices)
	receivedMsgs = make([][]byte, n)

	var wg sync.WaitGroup
	wg.Add(n)
	for i, senderIndex := range senderIndices {
		go func(i int, senderIndex int) {
			var e error
			receivedMsgs[i], e = dt.MessageReceive(ctx, senderIndex)
			if e != nil {
				err = e // Note: this is not thread-safe, but sufficient for testing
			}
			wg.Done()
		}(i, senderIndex)
	}
	wg.Wait()

	return receivedMsgs, nil
}

// NewMockNetwork creates a complete mock network with the specified number of parties.
// It returns a slice of MockMessenger instances, one for each party, already wired together.
func NewMockNetwork(nParties int) []*MockMessenger {
	messengers := make([]*MockMessenger, nParties)
	for i := 0; i < nParties; i++ {
		messengers[i] = NewMockMessenger(i)
	}
	for i := 0; i < nParties; i++ {
		messengers[i].setOuts(messengers)
	}
	return messengers
}
