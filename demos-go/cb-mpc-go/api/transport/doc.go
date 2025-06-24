// Package transport defines the abstractions that glue MPC protocols to the
// underlying network.
//
// The core interface is `Messenger` which provides a minimal set of primitives
// understood by the native C++ engine:
//
//	MessageSend(ctx, receiver, data)
//	MessageReceive(ctx, sender)
//	MessagesReceive(ctx, senders)
//
// A Messenger implementation does *not* need to care about protocol details –
// it simply delivers opaque byte slices between numbered parties.  This
// deliberate design choice lets applications swap transport mechanisms without
// touching any of the cryptography.
//
// Out of the box the repository provides two implementations:
//
//   - mocknet – an in-process, fully deterministic transport ideal for tests
//   - mtls    – a production-ready TCP transport that uses mutual-TLS for
//     authentication and encryption
//
// You are encouraged to implement your own Messenger for custom deployment
// scenarios (e.g. gRPC, libp2p, message queues, …).
package transport
