package cgobinding

/*
#cgo CFLAGS:    -I${SRCDIR}
#cgo CXXFLAGS:  -I${SRCDIR}
#include "ac.h"
*/
import "C"

type C_NodePtr C.crypto_ss_node_ref
type C_AcPtr C.crypto_ss_ac_ref

// NodeType represents the type of a node in an access-structure tree.
type NodeType int

const (
	NodeType_NONE NodeType = iota
	NodeType_LEAF
	NodeType_AND
	NodeType_OR
	NodeType_THRESHOLD
)

// NewNode constructs a new access-structure node by delegating to the native helper.
// The returned C_NodePtr owns the underlying C++ ss::node_t* pointer.
func NewNode(nodeType NodeType, nodeName string, threshold int) C_NodePtr {
	node := C.new_node(C.int(nodeType), cmem([]byte(nodeName)), C.int(threshold))
	return C_NodePtr(node)
}

// AddChild links |child| as a direct child of |parent| in the access structure tree.
func AddChild(parent, child C_NodePtr) {
	C.add_child((*C.crypto_ss_node_ref)(&parent), (*C.crypto_ss_node_ref)(&child))
}

// NewAccessStructure constructs a new native access-structure object and
// returns an opaque handle managed by the caller. The returned handle must be
// released exactly once via FreeAccessStructure.
func NewAccessStructure(root C_NodePtr, curve ECurveRef) C_AcPtr {
	ac := C.new_access_structure((*C.crypto_ss_node_ref)(&root), (*C.ecurve_ref)(&curve))
	return C_AcPtr(ac)
}

// FreeAccessStructure releases the resources associated with the native
// access-structure object previously created via NewAccessStructure.
func FreeAccessStructure(ac C_AcPtr) {
	C.free_crypto_ss_ac(C.crypto_ss_ac_ref(ac))
}
