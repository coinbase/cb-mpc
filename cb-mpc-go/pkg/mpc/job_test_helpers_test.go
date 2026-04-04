package mpc

import (
	"crypto/sha1"
	"fmt"
	"testing"
)

var defaultTestPartyLabels = []string{
	"alice",
	"bob",
	"carol",
	"dave",
	"erin",
	"frank",
	"grace",
	"heidi",
}

func scopedNames(scope string, labels ...string) []string {
	out := make([]string, len(labels))
	for i, label := range labels {
		out[i] = scopedUUIDName(scope, label)
	}
	return out
}

func scopedUUIDName(scope string, label string) string {
	sum := sha1.Sum([]byte("cb-mpc-go:" + scope + ":" + label))
	uuid := sum[:16]
	uuid[6] = (uuid[6] & 0x0f) | 0x50
	uuid[8] = (uuid[8] & 0x3f) | 0x80
	return fmt.Sprintf(
		"urn:uuid:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		uuid[0], uuid[1], uuid[2], uuid[3],
		uuid[4], uuid[5],
		uuid[6], uuid[7],
		uuid[8], uuid[9],
		uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15],
	)
}

func testPartyNames(namespace string, count int) []string {
	if count < 0 {
		panic("testPartyNames requires non-negative count")
	}
	if count > len(defaultTestPartyLabels) {
		panic("testPartyNames count exceeds default label pool")
	}
	return scopedNames("test:"+namespace, defaultTestPartyLabels[:count]...)
}

func test2PJobs[T interface{ Transport }](namespace string, transports []T) []Job2P {
	if len(transports) != 2 {
		panic("test2PJobs requires exactly 2 transports")
	}
	partyNames := testPartyNames(namespace, 2)
	return []Job2P{
		{
			Self:      TwoPartyP1,
			P1Name:    partyNames[0],
			P2Name:    partyNames[1],
			Transport: transports[0],
		},
		{
			Self:      TwoPartyP2,
			P1Name:    partyNames[0],
			P2Name:    partyNames[1],
			Transport: transports[1],
		},
	}
}

func jobsMP[T interface{ Transport }](transports []T, partyNames []string) []JobMP {
	jobs := make([]JobMP, len(transports))
	for i := range transports {
		jobs[i] = JobMP{
			Self:       i,
			PartyNames: append([]string(nil), partyNames...),
			Transport:  transports[i],
		}
	}
	return jobs
}

func mustPartyNamesFromIndices(t *testing.T, partyNames []string, indices []int) []string {
	t.Helper()
	names, err := PartyNamesFromIndices(partyNames, indices)
	if err != nil {
		t.Fatalf("PartyNamesFromIndices: %v", err)
	}
	return names
}
