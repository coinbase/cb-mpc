package common

import (
	"fmt"
	"net"
	"strconv"
)

// SplitHostPort parses host:port strings and returns the numeric port.
func SplitHostPort(addr string) (string, int, error) {
	host, portStr, err := net.SplitHostPort(addr)
	if err != nil {
		return "", 0, err
	}
	port, err := strconv.Atoi(portStr)
	if err != nil {
		return "", 0, fmt.Errorf("invalid port %q: %w", portStr, err)
	}
	return host, port, nil
}

// JoinHostPort formats a host and numeric port.
func JoinHostPort(host string, port int) string {
	return net.JoinHostPort(host, strconv.Itoa(port))
}

// OffsetAddresses applies a fixed port offset to each host:port address.
func OffsetAddresses(addrs []string, offset int) ([]string, error) {
	out := make([]string, len(addrs))
	for i, addr := range addrs {
		host, port, err := SplitHostPort(addr)
		if err != nil {
			return nil, fmt.Errorf("address %q: %w", addr, err)
		}
		out[i] = JoinHostPort(host, port+offset)
	}
	return out, nil
}

// OtherPartyIndices returns all party indices except self.
func OtherPartyIndices(self, partyCount int) []int {
	indices := make([]int, 0, partyCount-1)
	for i := 0; i < partyCount; i++ {
		if i != self {
			indices = append(indices, i)
		}
	}
	return indices
}
