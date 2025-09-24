// Package mpc provides data structures used by multiple MPC protocols.
// This file defines AccessNode – a minimal, logic-free representation
// of attribute-based access structures that will later be processed by
// native C++ code.
package mpc

import (
	"fmt"
	"strings"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// NodeKind tells which logical operator a node represents.
//
// The zero value corresponds to KindLeaf so that freshly allocated structs
// default to the most restrictive (leaf) kind.
type NodeKind uint8

const (
	// KindLeaf marks a terminal node with no children.
	KindLeaf NodeKind = iota
	// KindAnd represents logical conjunction – all children must be satisfied.
	KindAnd
	// KindOr represents logical disjunction – any child suffices.
	KindOr
	// KindThreshold represents an out-of-n condition where at least K of
	// the children must be satisfied.
	KindThreshold
)

// AccessNode is a purely-data representation of a node in an
// attribute-based access structure.
//
// Invariants (not enforced by code):
//   - The unique root has Name == "" and Parent == nil.
//   - Non-leaf nodes have at least one child.
//   - KindThreshold nodes use field K (0 < K ≤ len(Children)).
//   - KindLeaf nodes ignore field K and must have len(Children) == 0.
//
// All fields are exported so that other packages (or the C++ bridge) can walk
// and mutate the tree freely.
//
// NOTE: This struct purposefully embeds _no_ validation logic. Consumers are
// expected to perform their own checks or rely on downstream C++ code.
type AccessNode struct {
	Name     string        // human-readable identifier (root == "")
	Kind     NodeKind      // AND / OR / THRESHOLD / LEAF
	Parent   *AccessNode   // nil for root
	Children []*AccessNode // nil or empty slice for leaf
	K        int           // threshold value if Kind == KindThreshold
}

// ================= AccessStructure wrapper =========================

// AccessStructure bundles an access-tree (Root) together with the elliptic
// curve on which the underlying cryptographic secret-sharing operates.
//
// The type is a thin container with helper utilities to bridge to the native
// C++ implementation.
type AccessStructure struct {
	Root  *AccessNode // Root of the access structure tree (must not be nil)
	Curve curve.Curve // Elliptic curve used for commitments / shares
}

// String returns a multi-line representation that starts with the curve name
// and then embeds the pretty-printed access-tree.
func (as *AccessStructure) String() string {
	if as == nil {
		return "<nil>"
	}
	var sb strings.Builder
	if as.Curve != nil {
		sb.WriteString(fmt.Sprintf("Curve: %s\n", as.Curve))
	} else {
		sb.WriteString("Curve: <nil>\n")
	}
	if as.Root != nil {
		sb.WriteString(as.Root.String())
	} else {
		sb.WriteString("<nil root>\n")
	}
	return sb.String()
}

// toCryptoAC converts the AccessStructure into the native secret-sharing
// representation expected by the MPC engine and returns an opaque handle.
//
// The method panics if the AccessStructure is malformed (nil fields, unknown
// node kinds, …). Such errors typically indicate a misuse by calling code.
func (as *AccessStructure) toCryptoAC() cgobinding.C_AcPtr {
	if as == nil {
		panic("AccessStructure.toCryptoAC: receiver is nil")
	}
	if as.Root == nil {
		panic("AccessStructure.toCryptoAC: Root is nil")
	}
	if as.Curve == nil {
		panic("AccessStructure.toCryptoAC: Curve is nil")
	}

	// Local helper mapping Go enum to C enum (identical to the previous
	// implementation that lived on AccessNode).
	kindToC := func(k NodeKind) cgobinding.NodeType {
		switch k {
		case KindLeaf:
			return cgobinding.NodeType_LEAF
		case KindAnd:
			return cgobinding.NodeType_AND
		case KindOr:
			return cgobinding.NodeType_OR
		case KindThreshold:
			return cgobinding.NodeType_THRESHOLD
		default:
			panic(fmt.Sprintf("AccessStructure.toCryptoAC: unknown NodeKind %d", k))
		}
	}

	// Recursively clone the Go tree into the C representation.
	var build func(n *AccessNode) cgobinding.C_NodePtr
	build = func(n *AccessNode) cgobinding.C_NodePtr {
		cNode := cgobinding.NewNode(kindToC(n.Kind), n.Name, n.K)
		for _, child := range n.Children {
			if child == nil {
				continue
			}
			childPtr := build(child)
			cgobinding.AddChild(cNode, childPtr)
		}
		return cNode
	}

	rootPtr := build(as.Root)

	code := curve.Code(as.Curve)
	curveRef, err := cgobinding.ECurveFind(code)
	if err != nil {
		panic(fmt.Sprintf("AccessStructure.toCryptoAC: unsupported curve code %d", code))
	}

	ac := cgobinding.NewAccessStructure(rootPtr, curveRef)
	return ac
}

// Leaf returns a pointer to a leaf node.
func Leaf(name string) *AccessNode {
	return &AccessNode{Name: name, Kind: KindLeaf}
}

// And creates a logical AND node and wires the Parent pointers of its children.
func And(name string, kids ...*AccessNode) *AccessNode {
	n := &AccessNode{Name: name, Kind: KindAnd, Children: kids}
	for _, c := range kids {
		if c != nil {
			c.Parent = n
		}
	}
	return n
}

// Or creates a logical OR node and wires the Parent pointers of its children.
func Or(name string, kids ...*AccessNode) *AccessNode {
	n := &AccessNode{Name: name, Kind: KindOr, Children: kids}
	for _, c := range kids {
		if c != nil {
			c.Parent = n
		}
	}
	return n
}

// Threshold creates a threshold node (at least k of n) and wires the Parent pointers.
// It performs no validation of k against the number of children – that is deferred
// to the consuming C++ logic.
func Threshold(name string, k int, kids ...*AccessNode) *AccessNode {
	n := &AccessNode{Name: name, Kind: KindThreshold, K: k, Children: kids}
	for _, c := range kids {
		if c != nil {
			c.Parent = n
		}
	}
	return n
}

/*
Example construction (root name must be ""):

    root := And("",
        Or("role",
            Leaf("role:Admin"),
            Leaf("dept:HR"),
        ),
        Threshold("sig", 2,
            Leaf("sig:A"),
            Leaf("sig:B"),
            Leaf("sig:C"),
        ),
    )

`root` is now ready to be translated to C++.
*/

// String returns a human-readable, multi-line representation of the subtree
// rooted at the receiver. It recursively walks the tree and formats each node
// with two-space indentation per level.
//
// Example (matching the construction snippet below):
//
//	AND
//	  OR role
//	    LEAF role:Admin
//	    LEAF dept:HR
//	  THRESHOLD sig (2/3)
//	    LEAF sig:A
//	    LEAF sig:B
//	    LEAF sig:C
//
// The function never returns an error – malformed trees are printed as-is.
func (n *AccessNode) String() string {
	if n == nil {
		return "<nil>"
	}
	var sb strings.Builder
	n.format(&sb, 0)
	return sb.String()
}

// format writes a single node (with indentation) followed by all children.
func (n *AccessNode) format(sb *strings.Builder, level int) {
	indent := strings.Repeat("  ", level)
	sb.WriteString(indent)
	sb.WriteString(n.Kind.String())
	if n.Name != "" {
		sb.WriteByte(' ')
		sb.WriteString(n.Name)
	}
	if n.Kind == KindThreshold {
		sb.WriteString(fmt.Sprintf(" (%d/%d)", n.K, len(n.Children)))
	}
	sb.WriteByte('\n')
	for _, child := range n.Children {
		if child != nil {
			child.format(sb, level+1)
		} else {
			sb.WriteString(strings.Repeat("  ", level+1))
			sb.WriteString("<nil>\n")
		}
	}
}

// String returns the symbolic name of the NodeKind (LEAF, AND, OR, THRESHOLD).
func (k NodeKind) String() string {
	switch k {
	case KindLeaf:
		return "LEAF"
	case KindAnd:
		return "AND"
	case KindOr:
		return "OR"
	case KindThreshold:
		return "THRESHOLD"
	default:
		return fmt.Sprintf("NodeKind(%d)", k)
	}
}
