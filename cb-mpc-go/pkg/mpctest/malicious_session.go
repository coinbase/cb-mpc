package mpctest

import (
	"context"
	"errors"
	"sync"
)

// MaliciousTransport wraps a normal transport with adversarial behavior for tests.
type MaliciousTransport struct {
	inner    Transport
	behavior MaliciousBehavior
	mutex    sync.Mutex

	// State tracking.
	sendCount    int
	receiveCount int
}

// MaliciousBehavior defines what kind of malicious action to perform.
type MaliciousBehavior struct {
	// DropAllSends drops all outgoing messages (simulates a network partition).
	DropAllSends bool

	// DropAllReceives fails all receive operations.
	DropAllReceives bool

	// CorruptMessages modifies message contents before sending.
	CorruptMessages bool

	// SendWrongSize sends messages with incorrect size.
	SendWrongSize bool

	// FailAfterNSends fails after N send operations.
	FailAfterNSends int

	// FailAfterNReceives fails after N receive operations.
	FailAfterNReceives int

	// SendEmptyMessages sends empty messages instead of real data.
	SendEmptyMessages bool

	// SendGarbage sends all-0xFF garbage instead of protocol messages.
	SendGarbage bool

	// ReplayFirstMessage replays the first sent message repeatedly.
	ReplayFirstMessage bool
	firstMessage       []byte

	// FlipRandomBits flips a bit in the middle of the message.
	FlipRandomBits bool
}

// NewMaliciousTransport wraps a transport with malicious behavior.
func NewMaliciousTransport(inner Transport, behavior MaliciousBehavior) *MaliciousTransport {
	return &MaliciousTransport{
		inner:    inner,
		behavior: behavior,
	}
}

// Send implements Transport.Send with malicious behavior.
func (m *MaliciousTransport) Send(ctx context.Context, toParty int, msg []byte) error {
	if m.inner == nil {
		return errors.New("malicious: inner transport is nil")
	}

	m.mutex.Lock()
	m.sendCount++
	count := m.sendCount
	behavior := m.behavior
	if behavior.ReplayFirstMessage {
		if m.behavior.firstMessage == nil {
			m.behavior.firstMessage = append([]byte(nil), msg...)
		}
		behavior.firstMessage = m.behavior.firstMessage
	}
	m.mutex.Unlock()

	// Check if we should fail after N sends.
	if behavior.FailAfterNSends > 0 && count > behavior.FailAfterNSends {
		return errors.New("malicious: failing after N sends")
	}

	// Drop all sends.
	if behavior.DropAllSends {
		// Pretend success but don't actually send.
		return nil
	}

	// Send empty messages.
	if behavior.SendEmptyMessages {
		return m.inner.Send(ctx, toParty, []byte{})
	}

	// Send garbage.
	if behavior.SendGarbage {
		garbage := make([]byte, len(msg))
		for i := range garbage {
			garbage[i] = 0xFF
		}
		return m.inner.Send(ctx, toParty, garbage)
	}

	// Replay first message.
	if behavior.ReplayFirstMessage {
		return m.inner.Send(ctx, toParty, behavior.firstMessage)
	}

	// Corrupt messages.
	if behavior.CorruptMessages {
		corrupted := make([]byte, len(msg))
		copy(corrupted, msg)
		if len(corrupted) > 0 {
			corrupted[0] ^= 0xFF
		}
		return m.inner.Send(ctx, toParty, corrupted)
	}

	// Send wrong size.
	if behavior.SendWrongSize && len(msg) > 0 {
		return m.inner.Send(ctx, toParty, msg[:len(msg)/2])
	}

	// Flip random bits.
	if behavior.FlipRandomBits {
		corrupted := make([]byte, len(msg))
		copy(corrupted, msg)
		if len(corrupted) > 4 {
			corrupted[len(corrupted)/2] ^= 0x01
		}
		return m.inner.Send(ctx, toParty, corrupted)
	}

	// Normal send.
	return m.inner.Send(ctx, toParty, msg)
}

// Receive implements Transport.Receive with malicious behavior.
func (m *MaliciousTransport) Receive(ctx context.Context, fromParty int) ([]byte, error) {
	if m.inner == nil {
		return nil, errors.New("malicious: inner transport is nil")
	}

	m.mutex.Lock()
	m.receiveCount++
	count := m.receiveCount
	behavior := m.behavior
	m.mutex.Unlock()

	// Check if we should fail after N receives.
	if behavior.FailAfterNReceives > 0 && count > behavior.FailAfterNReceives {
		return nil, errors.New("malicious: failing after N receives")
	}

	// Drop all receives.
	if behavior.DropAllReceives {
		return nil, errors.New("malicious: dropping all receives")
	}

	// Normal receive.
	return m.inner.Receive(ctx, fromParty)
}

// ReceiveAll implements Transport.ReceiveAll with malicious behavior.
func (m *MaliciousTransport) ReceiveAll(ctx context.Context, fromParties []int) ([][]byte, error) {
	if m.inner == nil {
		return nil, errors.New("malicious: inner transport is nil")
	}

	m.mutex.Lock()
	m.receiveCount += len(fromParties)
	count := m.receiveCount
	behavior := m.behavior
	m.mutex.Unlock()

	// Check if we should fail after N receives.
	if behavior.FailAfterNReceives > 0 && count > behavior.FailAfterNReceives {
		return nil, errors.New("malicious: failing after N receives")
	}

	// Drop all receives.
	if behavior.DropAllReceives {
		return nil, errors.New("malicious: dropping all receives")
	}

	// Normal receive.
	return m.inner.ReceiveAll(ctx, fromParties)
}

// MyIndex exposes the wrapped transport's local index when available.
func (m *MaliciousTransport) MyIndex() int {
	indexed, ok := m.inner.(interface{ MyIndex() int })
	if !ok {
		return -1
	}
	return indexed.MyIndex()
}

// PartyCount exposes the wrapped transport's party count when available.
func (m *MaliciousTransport) PartyCount() int {
	counted, ok := m.inner.(interface{ PartyCount() int })
	if !ok {
		return -1
	}
	return counted.PartyCount()
}

// Close implements Transport.Close.
func (m *MaliciousTransport) Close() error {
	return m.inner.Close()
}

// NewMaliciousNetwork creates a mock network where one party is malicious.
func NewMaliciousNetwork(nParties int, maliciousParty int, behavior MaliciousBehavior) []Transport {
	mockTransports := NewMockNetwork(nParties)

	transports := make([]Transport, nParties)
	for i := 0; i < nParties; i++ {
		if i == maliciousParty {
			transports[i] = NewMaliciousTransport(mockTransports[i], behavior)
		} else {
			transports[i] = mockTransports[i]
		}
	}

	return transports
}
