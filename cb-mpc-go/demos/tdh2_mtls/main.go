package main

import (
	"context"
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"strings"
	"time"

	democommon "github.com/coinbase/cb-mpc-go/demos/common"
	"github.com/coinbase/cb-mpc-go/pkg/mpc"
)

const (
	tdh2PartyCount = 3
)

var demoPartyNames = []string{
	"urn:uuid:2f4b567e-7d58-52c5-9c74-0d0c8b6a3e10",
	"urn:uuid:7a1f5e4a-b8b4-5c67-a4cb-5777f284d64e",
	"urn:uuid:3f5d3b64-51d7-58d6-9d8d-3a7e6b1f4032",
}

type config struct {
	self       int
	listenAddr string
	partyAddrs []string
	caFile     string
	certFile   string
	keyFile    string
	timeout    time.Duration
}

type tdh2Message struct {
	Type       string `json:"type"`
	Label      []byte `json:"label,omitempty"`
	Ciphertext []byte `json:"ciphertext,omitempty"`
	Partial    []byte `json:"partial,omitempty"`
}

func main() {
	cfg := parseConfig()
	ctx, cancel := context.WithTimeout(context.Background(), cfg.timeout)
	defer cancel()

	tlsConfig, err := democommon.LoadTLSConfig(cfg.caFile, cfg.certFile, cfg.keyFile, false)
	if err != nil {
		log.Fatalf("load TLS config: %v", err)
	}

	transport, err := democommon.NewMTLSTransport(cfg.self, len(cfg.partyAddrs), cfg.listenAddr, cfg.partyAddrs, tlsConfig)
	if err != nil {
		log.Fatalf("create mTLS transport: %v", err)
	}
	defer func() { _ = transport.Close() }()

	peers := otherPartyIndices(cfg.self, len(cfg.partyAddrs))
	if err := transport.WaitForPeers(ctx, peers); err != nil {
		log.Fatalf("wait for peers: %v", err)
	}
	log.Printf("party %d connected to peers %v", cfg.self, peers)

	job := mpc.JobMP{
		Self:       cfg.self,
		PartyNames: append([]string(nil), demoPartyNames...),
		Transport:  transport,
	}
	publicKey, publicShares, privateShare, sidOut, err := mpc.TDH2DKGAdditive(ctx, job, mpc.CurveP256)
	if err != nil {
		log.Fatalf("tdh2 dkg additive: %v", err)
	}
	log.Printf("party %d DKG complete: public key bytes=%d sid bytes=%d", cfg.self, len(publicKey), len(sidOut))

	if cfg.self == 0 {
		if err := runCoordinatorTDH2Flow(ctx, transport, publicKey, publicShares, privateShare); err != nil {
			log.Fatalf("coordinator flow: %v", err)
		}
	} else {
		if err := runParticipantTDH2Flow(ctx, transport, cfg.self, publicKey, privateShare); err != nil {
			log.Fatalf("participant flow: %v", err)
		}
	}

	log.Printf("party %d TDH2 showcase complete", cfg.self)
}

func parseConfig() config {
	cfg := config{}
	var partyAddrs string
	flag.IntVar(&cfg.self, "self", -1, "party index (0, 1, or 2)")
	flag.StringVar(&cfg.listenAddr, "listen", "", "listen address for this party, e.g. 127.0.0.1:9740")
	flag.StringVar(&partyAddrs, "party-addrs", "", "comma-separated listen addresses for all parties in index order")
	flag.StringVar(&cfg.caFile, "ca", "", "path to CA certificate PEM")
	flag.StringVar(&cfg.certFile, "cert", "", "path to party certificate PEM")
	flag.StringVar(&cfg.keyFile, "key", "", "path to party private key PEM")
	flag.DurationVar(&cfg.timeout, "timeout", 90*time.Second, "overall protocol timeout")
	flag.Parse()

	if cfg.self < 0 || cfg.self >= tdh2PartyCount {
		log.Fatalf("--self must be in [0,%d)", tdh2PartyCount)
	}
	cfg.partyAddrs = splitAddresses(partyAddrs)
	if len(cfg.partyAddrs) != tdh2PartyCount {
		log.Fatalf("--party-addrs must list exactly %d addresses, got %d", tdh2PartyCount, len(cfg.partyAddrs))
	}
	if cfg.listenAddr == "" {
		log.Fatal("--listen is required")
	}
	if cfg.partyAddrs[cfg.self] != cfg.listenAddr {
		log.Fatalf("--listen must match the address at --party-addrs[%d]", cfg.self)
	}
	for flagName, value := range map[string]string{
		"--ca":   cfg.caFile,
		"--cert": cfg.certFile,
		"--key":  cfg.keyFile,
	} {
		if value == "" {
			log.Fatalf("%s is required", flagName)
		}
	}
	if flag.NArg() != 0 {
		log.Fatalf("unexpected positional arguments: %v", flag.Args())
	}
	log.Printf("starting party %d with listen=%s", cfg.self, cfg.listenAddr)
	return cfg
}

func runCoordinatorTDH2Flow(ctx context.Context, transport *democommon.MTLSTransport, publicKey []byte, publicShares [][]byte, privateShare []byte) error {
	plaintext := []byte("cb-mpc-go tdh2 mTLS demo payload")
	label := []byte("cb-mpc-go:tdh2-mtls-demo")

	ciphertext, err := mpc.TDH2Encrypt(publicKey, plaintext, label)
	if err != nil {
		return fmt.Errorf("encrypt: %w", err)
	}
	if err := mpc.TDH2Verify(publicKey, ciphertext, label); err != nil {
		return fmt.Errorf("verify ciphertext: %w", err)
	}
	ownPartial, err := mpc.TDH2PartialDecrypt(privateShare, ciphertext, label)
	if err != nil {
		return fmt.Errorf("compute own partial: %w", err)
	}

	request, err := json.Marshal(tdh2Message{
		Type:       "ciphertext",
		Label:      label,
		Ciphertext: ciphertext,
	})
	if err != nil {
		return fmt.Errorf("marshal request: %w", err)
	}
	for _, peer := range []int{1, 2} {
		if err := transport.Send(ctx, peer, request); err != nil {
			return fmt.Errorf("send ciphertext to party %d: %w", peer, err)
		}
	}

	partials := make([][]byte, len(publicShares))
	partials[0] = ownPartial
	for _, peer := range []int{1, 2} {
		msg, err := transport.Receive(ctx, peer)
		if err != nil {
			return fmt.Errorf("receive partial from party %d: %w", peer, err)
		}
		var response tdh2Message
		if err := json.Unmarshal(msg, &response); err != nil {
			return fmt.Errorf("unmarshal partial from party %d: %w", peer, err)
		}
		if response.Type != "partial" {
			return fmt.Errorf("unexpected message type from party %d: %s", peer, response.Type)
		}
		partials[peer] = response.Partial
	}

	recovered, err := mpc.TDH2CombineAdditive(publicKey, publicShares, label, partials, ciphertext)
	if err != nil {
		return fmt.Errorf("combine additive: %w", err)
	}
	if string(recovered) != string(plaintext) {
		return fmt.Errorf("recovered plaintext mismatch")
	}
	log.Printf("party 0 recovered plaintext bytes=%d ciphertext bytes=%d", len(recovered), len(ciphertext))
	return nil
}

func runParticipantTDH2Flow(ctx context.Context, transport *democommon.MTLSTransport, self int, publicKey []byte, privateShare []byte) error {
	msg, err := transport.Receive(ctx, 0)
	if err != nil {
		return fmt.Errorf("receive ciphertext from party 0: %w", err)
	}

	var request tdh2Message
	if err := json.Unmarshal(msg, &request); err != nil {
		return fmt.Errorf("unmarshal request: %w", err)
	}
	if request.Type != "ciphertext" {
		return fmt.Errorf("unexpected message type %q", request.Type)
	}
	if err := mpc.TDH2Verify(publicKey, request.Ciphertext, request.Label); err != nil {
		return fmt.Errorf("verify ciphertext: %w", err)
	}
	partial, err := mpc.TDH2PartialDecrypt(privateShare, request.Ciphertext, request.Label)
	if err != nil {
		return fmt.Errorf("partial decrypt: %w", err)
	}
	response, err := json.Marshal(tdh2Message{
		Type:    "partial",
		Partial: partial,
	})
	if err != nil {
		return fmt.Errorf("marshal partial: %w", err)
	}
	if err := transport.Send(ctx, 0, response); err != nil {
		return fmt.Errorf("send partial to party 0: %w", err)
	}
	log.Printf("party %d sent partial bytes=%d", self, len(partial))
	return nil
}

func splitAddresses(raw string) []string {
	if raw == "" {
		return nil
	}
	parts := strings.Split(raw, ",")
	for i := range parts {
		parts[i] = strings.TrimSpace(parts[i])
	}
	return parts
}

func otherPartyIndices(self, partyCount int) []int {
	indices := make([]int, 0, partyCount-1)
	for i := 0; i < partyCount; i++ {
		if i != self {
			indices = append(indices, i)
		}
	}
	return indices
}
