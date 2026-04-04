package main

import (
	"fmt"
	"slices"
	"sort"
	"strconv"
	"strings"
)

type signRequest struct {
	QuorumIndices    []int
	QuorumPartyNames []string
	SignReceiver     int
	ReceiverPartyIdx int
}

type signQuorumOption struct {
	Value string
	Label string
}

func buildLocalSignQuorumOptions(workflow *workflowState, self int) ([]signQuorumOption, error) {
	quorums, err := enumerateThresholdQuorums(len(workflow.PartyNames), workflow.Threshold)
	if err != nil {
		return nil, err
	}
	if workflow.SignReceiver < 0 || workflow.SignReceiver >= workflow.Threshold {
		return nil, fmt.Errorf("sign receiver %d out of range [0, %d)", workflow.SignReceiver, workflow.Threshold)
	}

	options := make([]signQuorumOption, 0, len(quorums))
	for _, quorum := range quorums {
		if !slices.Contains(quorum, self) {
			continue
		}
		receiverPartyIdx := quorum[workflow.SignReceiver]
		options = append(options, signQuorumOption{
			Value: joinIndicesCSV(quorum),
			Label: fmt.Sprintf("%s (receiver party %d)", formatPartyIndicesLabel(quorum), receiverPartyIdx),
		})
	}
	return options, nil
}

func parseSignRequest(workflow *workflowState, self int, rawQuorum string) (signRequest, error) {
	indices, err := parseIndicesCSV(rawQuorum)
	if err != nil {
		return signRequest{}, fmt.Errorf("parse sign quorum: %w", err)
	}
	sort.Ints(indices)
	if err := validateIndexSet("sign quorum", indices, len(workflow.PartyNames)); err != nil {
		return signRequest{}, err
	}
	if len(indices) != workflow.Threshold {
		return signRequest{}, fmt.Errorf("sign quorum count %d must equal threshold %d", len(indices), workflow.Threshold)
	}
	if workflow.SignReceiver < 0 || workflow.SignReceiver >= len(indices) {
		return signRequest{}, fmt.Errorf("sign receiver %d out of range [0, %d)", workflow.SignReceiver, len(indices))
	}
	if !slices.Contains(indices, self) {
		return signRequest{}, fmt.Errorf("party %d is not in selected sign quorum %s", self, formatPartyIndicesLabel(indices))
	}
	partyNames, err := mpcPartyNamesFromIndices(workflow.PartyNames, indices)
	if err != nil {
		return signRequest{}, err
	}
	return signRequest{
		QuorumIndices:    append([]int(nil), indices...),
		QuorumPartyNames: partyNames,
		SignReceiver:     workflow.SignReceiver,
		ReceiverPartyIdx: indices[workflow.SignReceiver],
	}, nil
}

func enumerateThresholdQuorums(partyCount int, threshold int) ([][]int, error) {
	if partyCount < 2 {
		return nil, fmt.Errorf("party count must be at least 2, got %d", partyCount)
	}
	if threshold <= 0 || threshold > partyCount {
		return nil, fmt.Errorf("threshold %d out of range [1, %d]", threshold, partyCount)
	}

	var out [][]int
	buildThresholdQuorums(0, partyCount, threshold, nil, &out)
	return out, nil
}

func buildThresholdQuorums(start int, partyCount int, remaining int, current []int, out *[][]int) {
	if remaining == 0 {
		quorum := append([]int(nil), current...)
		*out = append(*out, quorum)
		return
	}
	maxStart := partyCount - remaining
	for idx := start; idx <= maxStart; idx++ {
		next := append(append([]int(nil), current...), idx)
		buildThresholdQuorums(idx+1, partyCount, remaining-1, next, out)
	}
}

func joinIndicesCSV(indices []int) string {
	parts := make([]string, len(indices))
	for i, idx := range indices {
		parts[i] = strconv.Itoa(idx)
	}
	return strings.Join(parts, ",")
}

func formatPartyIndicesLabel(indices []int) string {
	parts := make([]string, len(indices))
	for i, idx := range indices {
		parts[i] = strconv.Itoa(idx)
	}
	return "Parties " + strings.Join(parts, " + ")
}
