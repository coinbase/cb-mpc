package mpctest

import (
	"context"
	"io"
)

// Transport mirrors the network contract used by interactive MPC protocols.
// Concrete types in this package also satisfy `mpc.Transport`.
type Transport interface {
	io.Closer
	Send(ctx context.Context, toParty int, msg []byte) error
	Receive(ctx context.Context, fromParty int) ([]byte, error)
	ReceiveAll(ctx context.Context, fromParties []int) ([][]byte, error)
}
