// Package zk contains zero-knowledge protocols that can be used alongside the
// MPC primitives in CB-MPC.
//
// A zero-knowledge proof lets a prover convince a verifier that a statement is
// true without revealing *why* it is true.  The proofs implemented here are
// small, non-interactive and can be transmitted over any `transport.Messenger`.
//
// Currently implemented:
//
//   - ZK-DL – Proof of knowledge of a discrete-logarithm relative to a curve
//     generator (i.e. possession of an ECDSA private key).
//
// The Go API follows the same request/response design used by the `mpc`
// package which makes it trivial to marshal the data into JSON or protobuf and
// to plug the proofs into higher-level protocols.
//
// Example
//
//	// 1. Generate a fresh key pair.
//	kp, _ := zk.ZKDLGenerateKeyPair(&zk.ZKDLKeyGenRequest{})
//
//	// 2. Produce a proof that we know the private key.
//	proveResp, _ := zk.ZKUCDLProve(&zk.ZKUCDLProveRequest{
//	    PublicKey: kp.PublicKey,
//	    Witness:   kp.PrivateKey,
//	    SessionID: []byte("session-1"),
//	    Auxiliary: 42,
//	})
//
//	// 3. Verify the proof.
//	verifyResp, _ := zk.ZKUCDLVerify(&zk.ZKUCDLVerifyRequest{
//	    PublicKey: kp.PublicKey,
//	    Proof:     proveResp.Proof,
//	    SessionID: []byte("session-1"),
//	    Auxiliary: 42,
//	})
//
//	if !verifyResp.Valid {
//	    log.Fatal("proof rejected")
//	}
package zk
