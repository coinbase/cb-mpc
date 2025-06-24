package mocknet

import (
	"context"
	"testing"

	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/require"
)

func TestNewRunner(t *testing.T) {
	// Test creating a runner with 2 parties
	runner := NewMPCRunner(GeneratePartyNames(2)...)
	assert.NotNil(t, runner)
	assert.Equal(t, 2, runner.nParties)
	assert.Len(t, runner.peers, 2)

	// Verify peers are properly initialized
	for i, peer := range runner.peers {
		assert.Equal(t, i, peer.roleIndex)
		assert.Equal(t, 2, peer.nParties)
		assert.NotNil(t, peer.dataTransport)
	}
}

func TestMockMessenger(t *testing.T) {
	// Create a mock network with 3 parties
	messengers := NewMockNetwork(3)
	require.Len(t, messengers, 3)

	// Test message sending and receiving
	message := []byte("test message")

	// Party 0 sends to party 1
	err := messengers[0].MessageSend(context.Background(), 1, message)
	assert.NoError(t, err)

	// Party 1 receives from party 0
	received, err := messengers[1].MessageReceive(context.Background(), 0)
	assert.NoError(t, err)
	assert.Equal(t, message, received)
}

func TestMockMessengerMultipleMessages(t *testing.T) {
	// Create a mock network with 2 parties
	messengers := NewMockNetwork(2)

	// Test receiving messages one by one to avoid order issues
	message1 := []byte("message 1")
	message2 := []byte("message 2")

	// Party 1 sends first message to party 0
	err := messengers[1].MessageSend(context.Background(), 0, message1)
	assert.NoError(t, err)

	// Party 0 receives first message from party 1
	received1, err := messengers[0].MessageReceive(context.Background(), 1)
	assert.NoError(t, err)
	assert.Equal(t, message1, received1)

	// Party 1 sends second message to party 0
	err = messengers[1].MessageSend(context.Background(), 0, message2)
	assert.NoError(t, err)

	// Party 0 receives second message from party 1
	received2, err := messengers[0].MessageReceive(context.Background(), 1)
	assert.NoError(t, err)
	assert.Equal(t, message2, received2)
}
