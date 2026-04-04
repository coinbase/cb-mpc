package main

import (
	"flag"
	"fmt"
	"io"
	"path/filepath"
	"strconv"
	"strings"
	"time"
)

const (
	defaultThreshold    = 2
	defaultSignReceiver = 0
)

type config struct {
	self           int
	httpListen     string
	transportAddrs []string
	caFile         string
	certFile       string
	keyFile        string
	dataDir        string
	timeout        time.Duration
	partyNames     []string
	quorumIndices  []int
	threshold      int
	signReceiver   int
}

func parseConfigFromArgs(args []string) (config, error) {
	defaultPartyNames := strings.Join(DefaultECDSAMPACPartyNames(), ",")

	var cfg config
	var rawTransportAddrs string
	var rawPartyNames string
	var rawQuorumIndices string

	fs := flag.NewFlagSet("ecdsa_mp_ac_web", flag.ContinueOnError)
	fs.SetOutput(io.Discard)
	fs.IntVar(&cfg.self, "self", -1, "party index for this web server")
	fs.StringVar(&cfg.httpListen, "http-listen", "", "HTTP listen address, e.g. 127.0.0.1:7080")
	fs.StringVar(&rawTransportAddrs, "transport-addrs", "", "comma-separated base transport listen addresses for all parties")
	fs.StringVar(&rawPartyNames, "party-names", defaultPartyNames, "comma-separated globally unique MPC party names")
	fs.StringVar(&rawQuorumIndices, "quorum-indices", "0,1", "comma-separated quorum indices used for DKG and refresh within the full party roster")
	fs.IntVar(&cfg.threshold, "threshold", defaultThreshold, "access-structure threshold across the full roster")
	fs.IntVar(&cfg.signReceiver, "sign-receiver", defaultSignReceiver, "index within the selected signing quorum that should receive the DER signature")
	fs.StringVar(&cfg.caFile, "ca", "", "path to CA certificate PEM")
	fs.StringVar(&cfg.certFile, "cert", "", "path to local party certificate PEM")
	fs.StringVar(&cfg.keyFile, "key", "", "path to local party private key PEM")
	fs.StringVar(&cfg.dataDir, "data-dir", "", "directory used for persisted demo state (defaults to tmp/state/party-N relative to the demo module)")
	fs.DurationVar(&cfg.timeout, "timeout", 120*time.Second, "per-operation timeout")
	if err := fs.Parse(args); err != nil {
		return config{}, err
	}
	if fs.NArg() != 0 {
		return config{}, fmt.Errorf("unexpected positional arguments: %v", fs.Args())
	}

	if cfg.self < 0 {
		return config{}, fmt.Errorf("--self is required")
	}
	if cfg.httpListen == "" {
		return config{}, fmt.Errorf("--http-listen is required")
	}
	for flagName, value := range map[string]string{
		"--transport-addrs": rawTransportAddrs,
		"--ca":              cfg.caFile,
		"--cert":            cfg.certFile,
		"--key":             cfg.keyFile,
	} {
		if value == "" {
			return config{}, fmt.Errorf("%s is required", flagName)
		}
	}
	if cfg.timeout <= 0 {
		return config{}, fmt.Errorf("--timeout must be positive")
	}

	cfg.partyNames = splitCSV(rawPartyNames)
	if len(cfg.partyNames) < 2 {
		return config{}, fmt.Errorf("--party-names must list at least 2 entries, got %d", len(cfg.partyNames))
	}
	cfg.transportAddrs = splitCSV(rawTransportAddrs)
	if len(cfg.transportAddrs) != len(cfg.partyNames) {
		return config{}, fmt.Errorf("--transport-addrs count %d must match --party-names count %d", len(cfg.transportAddrs), len(cfg.partyNames))
	}
	if cfg.self >= len(cfg.partyNames) {
		return config{}, fmt.Errorf("--self=%d out of range for %d party names", cfg.self, len(cfg.partyNames))
	}

	quorumIndices, err := parseIndicesCSV(rawQuorumIndices)
	if err != nil {
		return config{}, fmt.Errorf("--quorum-indices: %w", err)
	}
	if err := validateIndexSet("quorum", quorumIndices, len(cfg.partyNames)); err != nil {
		return config{}, err
	}
	if cfg.threshold <= 0 || cfg.threshold > len(cfg.partyNames) {
		return config{}, fmt.Errorf("--threshold %d out of range [1, %d]", cfg.threshold, len(cfg.partyNames))
	}
	if len(quorumIndices) < cfg.threshold {
		return config{}, fmt.Errorf("--quorum-indices count %d must be at least threshold %d", len(quorumIndices), cfg.threshold)
	}
	if cfg.signReceiver < 0 || cfg.signReceiver >= cfg.threshold {
		return config{}, fmt.Errorf("--sign-receiver %d out of range [0, %d)", cfg.signReceiver, cfg.threshold)
	}
	cfg.quorumIndices = quorumIndices

	if cfg.dataDir == "" {
		cfg.dataDir = filepath.Join("tmp", "state", fmt.Sprintf("party-%d", cfg.self))
	}

	return cfg, nil
}

func splitCSV(raw string) []string {
	if raw == "" {
		return nil
	}
	parts := strings.Split(raw, ",")
	out := make([]string, 0, len(parts))
	for _, part := range parts {
		trimmed := strings.TrimSpace(part)
		if trimmed == "" {
			continue
		}
		out = append(out, trimmed)
	}
	return out
}

func parseIndicesCSV(raw string) ([]int, error) {
	parts := splitCSV(raw)
	if len(parts) == 0 {
		return nil, fmt.Errorf("must list at least one index")
	}
	out := make([]int, len(parts))
	for i, part := range parts {
		value, err := strconv.Atoi(part)
		if err != nil {
			return nil, fmt.Errorf("invalid integer %q", part)
		}
		out[i] = value
	}
	return out, nil
}

func validateIndexSet(name string, indices []int, limit int) error {
	seen := make(map[int]struct{}, len(indices))
	for i, idx := range indices {
		if idx < 0 || idx >= limit {
			return fmt.Errorf("%s[%d]=%d out of range [0, %d)", name, i, idx, limit)
		}
		if _, ok := seen[idx]; ok {
			return fmt.Errorf("%s[%d]=%d duplicates another index", name, i, idx)
		}
		seen[idx] = struct{}{}
	}
	return nil
}
