package mpc

import (
	"context"
	"fmt"
	"io"
	"net"
	"strings"
)

// Curve matches cbmpc_curve_id_t.
type Curve int32

const (
	// CurveP256 is NIST P-256 / secp256r1.
	CurveP256 Curve = 1
	// CurveSecp256k1 is the secp256k1 curve.
	CurveSecp256k1 Curve = 2
	// CurveEd25519 is the Edwards curve used by Ed25519.
	CurveEd25519 Curve = 3
)

func (c Curve) String() string {
	switch c {
	case CurveP256:
		return "p256"
	case CurveSecp256k1:
		return "secp256k1"
	case CurveEd25519:
		return "ed25519"
	default:
		return fmt.Sprintf("unknown(%d)", int32(c))
	}
}

// Transport is the network contract for interactive protocols.
//
// Production implementations should provide authenticated and confidential
// channels (e.g., mTLS) and enforce reasonable message size limits to avoid
// memory-exhaustion attacks from untrusted peers.
type Transport interface {
	io.Closer

	// Send transmits msg to the party identified by toParty.
	//
	// Party indices use the same numbering as the job invoking the protocol:
	// `TwoPartyP1`/`TwoPartyP2` for `Job2P`, or indices into `JobMP.PartyNames`
	// for `JobMP`.
	//
	// It blocks until the message is accepted or the context is canceled.
	Send(ctx context.Context, toParty int, msg []byte) error

	// Receive blocks until a message arrives from fromParty or the context is
	// canceled.
	//
	// fromParty uses the same party-index space as Send.
	Receive(ctx context.Context, fromParty int) ([]byte, error)

	// ReceiveAll blocks until one message has arrived from each party in
	// fromParties.
	//
	// The returned slice must have len(fromParties) entries and preserve the same
	// order as fromParties.
	ReceiveAll(ctx context.Context, fromParties []int) ([][]byte, error)
}

// TwoPartySelf identifies this caller's role in a 2PC job.
type TwoPartySelf int32

const (
	// TwoPartyP1 identifies the first party in a 2PC job.
	TwoPartyP1 TwoPartySelf = 0
	// TwoPartyP2 identifies the second party in a 2PC job.
	TwoPartyP2 TwoPartySelf = 1
)

func (s TwoPartySelf) String() string {
	switch s {
	case TwoPartyP1:
		return "p1"
	case TwoPartyP2:
		return "p2"
	default:
		return fmt.Sprintf("unknown(%d)", int32(s))
	}
}

// Job2P is the explicit cb-mpc execution context for a 2-party protocol call.
//
// P1Name and P2Name should be stable, globally unique identifiers for the two
// parties taking part in the protocol run.
type Job2P struct {
	Self      TwoPartySelf
	P1Name    string
	P2Name    string
	Transport Transport
}

// JobMP is the explicit cb-mpc execution context for a multi-party protocol call.
//
// PartyNames should contain stable, globally unique identifiers for each party.
// `Self` and any other party indices passed to the public API are interpreted
// relative to the ordering in PartyNames.
type JobMP struct {
	Self       int
	PartyNames []string
	Transport  Transport
}

// AccessStructureNodeType matches cbmpc_access_structure_node_type_t.
type AccessStructureNodeType int32

const (
	// AccessNodeLeaf identifies a leaf node that names a single party.
	AccessNodeLeaf AccessStructureNodeType = 1
	// AccessNodeAnd identifies a node that requires all children.
	AccessNodeAnd AccessStructureNodeType = 2
	// AccessNodeOr identifies a node that requires any one child.
	AccessNodeOr AccessStructureNodeType = 3
	// AccessNodeThreshold identifies a node that requires ThresholdK children.
	AccessNodeThreshold AccessStructureNodeType = 4
)

// AccessStructureNode is a Go tree node for access-structure APIs.
type AccessStructureNode struct {
	Type       AccessStructureNodeType
	LeafName   string
	ThresholdK int
	Children   []int
}

// AccessStructure is the Go representation of an access-structure tree.
type AccessStructure struct {
	Nodes []AccessStructureNode
	Root  int
}

// PartyNamesFromIndices selects party names from an explicit roster by index.
func PartyNamesFromIndices(partyNames []string, indices []int) ([]string, error) {
	names := make([]string, len(indices))
	for i, idx := range indices {
		if idx < 0 || idx >= len(partyNames) {
			return nil, invalidParameterErrorf("indices[%d]=%d out of range [0, %d)", i, idx, len(partyNames))
		}
		names[i] = partyNames[idx]
	}
	return names, nil
}

func validateTransport(transport Transport) error {
	if transport == nil {
		return invalidParameterErrorf("transport cannot be nil")
	}
	return nil
}

func validateTwoPartySelf(self TwoPartySelf) error {
	switch self {
	case TwoPartyP1, TwoPartyP2:
		return nil
	default:
		return invalidParameterErrorf("invalid two-party self %d", int32(self))
	}
}

func validatePartyName(name string, value string) error {
	if value == "" {
		return invalidParameterErrorf("%s cannot be empty", name)
	}
	if strings.IndexByte(value, 0) != -1 {
		return invalidParameterErrorf("%s cannot contain NUL bytes", name)
	}
	if isSequentialPartyLabel(value) {
		return invalidParameterErrorf("%s=%q uses a placeholder label; use a stable globally unique identifier such as a public key fingerprint or UUID", name, value)
	}
	if net.ParseIP(value) != nil {
		return invalidParameterErrorf("%s=%q must not be a bare IP address; use a stable globally unique identifier such as a public key fingerprint or UUID", name, value)
	}
	if host, port, err := net.SplitHostPort(value); err == nil && host != "" && port != "" {
		return invalidParameterErrorf("%s=%q must not be a host:port endpoint; use a stable globally unique identifier such as a public key fingerprint or UUID", name, value)
	}
	return nil
}

func isSequentialPartyLabel(value string) bool {
	lower := strings.ToLower(value)
	if !strings.HasPrefix(lower, "party") {
		return false
	}
	if len(lower) == len("party") {
		return true
	}
	for _, ch := range lower[len("party"):] {
		if ch < '0' || ch > '9' {
			return false
		}
	}
	return true
}

func validatePartyNames(name string, partyNames []string) error {
	return validateDistinctPartyNames(name, partyNames)
}

func validateAccessStructureLeafNames(access *AccessStructure) error {
	if access == nil {
		return nil
	}
	for i, node := range access.Nodes {
		if node.Type != AccessNodeLeaf {
			continue
		}
		if err := validatePartyName(fmt.Sprintf("access.Nodes[%d].LeafName", i), node.LeafName); err != nil {
			return err
		}
	}
	return nil
}

func validateDistinctPartyNames(name string, partyNames []string) error {
	seen := make(map[string]struct{}, len(partyNames))
	for i, partyName := range partyNames {
		if err := validatePartyName(fmt.Sprintf("%s[%d]", name, i), partyName); err != nil {
			return err
		}
		if _, ok := seen[partyName]; ok {
			return invalidParameterErrorf("%s[%d]=%q duplicates another party name", name, i, partyName)
		}
		seen[partyName] = struct{}{}
	}
	return nil
}

func validatePartyNameSubset(name string, roster []string, subset []string) error {
	if err := validateDistinctPartyNames(name, subset); err != nil {
		return err
	}
	rosterSet := make(map[string]struct{}, len(roster))
	for _, partyName := range roster {
		rosterSet[partyName] = struct{}{}
	}
	for i, partyName := range subset {
		if _, ok := rosterSet[partyName]; !ok {
			return invalidParameterErrorf("%s[%d]=%q is not present in the job party roster", name, i, partyName)
		}
	}
	return nil
}

func validateJob2P(job Job2P) error {
	if err := validateTransport(job.Transport); err != nil {
		return err
	}
	if err := validateTwoPartySelf(job.Self); err != nil {
		return err
	}
	if err := validatePartyName("job.P1Name", job.P1Name); err != nil {
		return err
	}
	if err := validatePartyName("job.P2Name", job.P2Name); err != nil {
		return err
	}
	if job.P1Name == job.P2Name {
		return invalidParameterErrorf("job.P1Name and job.P2Name must be distinct")
	}
	return nil
}

func validateJobMP(job JobMP) error {
	if err := validateTransport(job.Transport); err != nil {
		return err
	}
	if len(job.PartyNames) < 2 {
		return invalidParameterErrorf("job.PartyNames must contain at least 2 entries, got %d", len(job.PartyNames))
	}
	if len(job.PartyNames) > 64 {
		return invalidParameterErrorf("job.PartyNames exceeds maximum size 64: got %d", len(job.PartyNames))
	}
	if job.Self < 0 || job.Self >= len(job.PartyNames) {
		return invalidParameterErrorf("job.Self %d out of range [0, %d)", job.Self, len(job.PartyNames))
	}
	return validatePartyNames("job.PartyNames", job.PartyNames)
}

func partyNamesFromIndices(partyNames []string, indices []int) []string {
	names, err := PartyNamesFromIndices(partyNames, indices)
	if err != nil {
		return nil
	}
	return names
}
