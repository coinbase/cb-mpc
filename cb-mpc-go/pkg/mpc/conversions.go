package mpc

import (
	"math"

	cinterop "github.com/coinbase/cb-mpc-go/internal/cgo"
)

func toCGOTwoPartySelf(self TwoPartySelf) (cinterop.TwoPartySelf, error) {
	switch self {
	case TwoPartyP1:
		return cinterop.TwoPartyP1, nil
	case TwoPartyP2:
		return cinterop.TwoPartyP2, nil
	default:
		return 0, invalidParameterErrorf("invalid two-party self %d", int32(self))
	}
}

func toCGOJob2P(job Job2P) (cinterop.Job2P, error) {
	if err := validateJob2P(job); err != nil {
		return cinterop.Job2P{}, err
	}
	self, err := toCGOTwoPartySelf(job.Self)
	if err != nil {
		return cinterop.Job2P{}, err
	}
	return cinterop.Job2P{
		Self:      self,
		P1Name:    job.P1Name,
		P2Name:    job.P2Name,
		Transport: job.Transport,
	}, nil
}

func toCGOJobMP(job JobMP) (cinterop.JobMP, error) {
	if err := validateJobMP(job); err != nil {
		return cinterop.JobMP{}, err
	}
	return cinterop.JobMP{
		Self:       job.Self,
		PartyNames: append([]string(nil), job.PartyNames...),
		Transport:  job.Transport,
	}, nil
}

func toCGOCurve(curve Curve) (cinterop.CurveID, error) {
	switch curve {
	case CurveP256:
		return cinterop.CurveP256, nil
	case CurveSecp256k1:
		return cinterop.CurveSecp256k1, nil
	case CurveEd25519:
		return cinterop.CurveEd25519, nil
	default:
		return 0, invalidParameterErrorf("unsupported curve %d", int32(curve))
	}
}

func toCGOAccessStructure(access *AccessStructure) (*cinterop.AccessStructure, error) {
	if access == nil {
		return nil, invalidParameterErrorf("access structure cannot be nil")
	}
	if len(access.Nodes) == 0 {
		return nil, invalidParameterErrorf("access structure must contain at least one node")
	}
	if len(access.Nodes) > math.MaxInt32 {
		return nil, invalidParameterErrorf("access structure has too many nodes: %d", len(access.Nodes))
	}
	if access.Root > math.MaxInt32 {
		return nil, invalidParameterErrorf("access structure root index %d exceeds int32 range", access.Root)
	}
	if access.Root < 0 || access.Root >= len(access.Nodes) {
		return nil, invalidParameterErrorf("access structure root index %d out of range [0, %d)", access.Root, len(access.Nodes))
	}
	if err := validateAccessStructureLeafNames(access); err != nil {
		return nil, err
	}
	nodes := make([]cinterop.AccessStructureNode, len(access.Nodes))
	for i, node := range access.Nodes {
		if node.ThresholdK < math.MinInt32 || node.ThresholdK > math.MaxInt32 {
			return nil, invalidParameterErrorf("access structure node %d has threshold_k out of int32 range: %d", i, node.ThresholdK)
		}
		children := make([]int32, len(node.Children))
		for j, child := range node.Children {
			if child > math.MaxInt32 {
				return nil, invalidParameterErrorf("access structure node %d child[%d]=%d exceeds int32 range", i, j, child)
			}
			if child < 0 || child >= len(access.Nodes) {
				return nil, invalidParameterErrorf("access structure node %d child[%d]=%d out of range [0, %d)", i, j, child, len(access.Nodes))
			}
			children[j] = int32(child)
		}
		nodes[i] = cinterop.AccessStructureNode{
			Type:       cinterop.AccessStructureNodeType(node.Type),
			LeafName:   node.LeafName,
			ThresholdK: int32(node.ThresholdK),
			Children:   children,
		}
	}
	return &cinterop.AccessStructure{
		Nodes: nodes,
		Root:  int32(access.Root),
	}, nil
}
