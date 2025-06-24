package transport

import "context"

// Messenger defines the interface for data transport in the CB-MPC system.
// Implementations of this interface handle message passing between MPC parties.
type Messenger interface {
	// MessageSend sends a message buffer to the specified receiver party.
	MessageSend(ctx context.Context, receiver int, buffer []byte) error

	// MessageReceive receives a message from the specified sender party.
	MessageReceive(ctx context.Context, sender int) ([]byte, error)

	// MessagesReceive receives messages from multiple sender parties. It waits
	// until all messages are ready and returns them in the same order as the
	// provided senders slice.
	MessagesReceive(ctx context.Context, senders []int) ([][]byte, error)
}
