package main

import (
	"bytes"
	"context"
	"crypto/sha256"
	"crypto/tls"
	"flag"
	"fmt"
	"log"
	"strings"
	"time"

	democommon "github.com/coinbase/cb-mpc-go/demos/common"
	"github.com/coinbase/cb-mpc-go/pkg/mpc"
)

const (
	dkgStagePortOffset     = democommon.ECDSAMPACDKGStagePortOffset
	signStagePortOffset    = democommon.ECDSAMPACSignStagePortOffset
	refreshStagePortOffset = democommon.ECDSAMPACRefreshStagePortOffset
	resignStagePortOffset  = democommon.ECDSAMPACResignStagePortOffset
	ecdsaPartyCount        = 3
	onlineQuorumCount      = 2
	signReceiver           = 0
)

var demoPartyNames = democommon.DefaultECDSAMPACPartyNames()

type config struct {
	self       int
	listenAddr string
	partyAddrs []string
	caFile     string
	certFile   string
	keyFile    string
	timeout    time.Duration
}

func main() {
	cfg := parseConfig()
	ctx, cancel := context.WithTimeout(context.Background(), cfg.timeout)
	defer cancel()

	tlsConfig, err := democommon.LoadTLSConfig(cfg.caFile, cfg.certFile, cfg.keyFile, false)
	if err != nil {
		log.Fatalf("load TLS config: %v", err)
	}

	partyNames := append([]string(nil), demoPartyNames...)
	access, err := democommon.ThresholdAccessStructure(partyNames, 2)
	if err != nil {
		log.Fatalf("build access structure: %v", err)
	}
	quorumPartyIndices := []int{0, 1}
	quorumPartyNames, err := mpc.PartyNamesFromIndices(partyNames, quorumPartyIndices)
	if err != nil {
		log.Fatalf("quorum party names: %v", err)
	}
	stage0Addrs, err := democommon.OffsetAddresses(cfg.partyAddrs, dkgStagePortOffset)
	if err != nil {
		log.Fatalf("stage0 addresses: %v", err)
	}

	dkgResult, err := democommon.RunECDSAMPACDKG(ctx, cfg.self, partyNames, stage0Addrs, tlsConfig, mpc.CurveSecp256k1, access, quorumPartyNames)
	if err != nil {
		log.Fatalf("dkg ac: %v", err)
	}
	acKeyBlob := dkgResult.ACKeyBlob
	sidOut := dkgResult.SID
	publicKeyCompressed := dkgResult.PublicKeyCompressed
	log.Printf("party %d DKG AC complete: public key bytes=%d sid bytes=%d", cfg.self, len(publicKeyCompressed), len(sidOut))

	msgHash := sha256.Sum256([]byte("cb-mpc-go ecdsa mp ac mTLS demo"))
	signStageAddrs, err := democommon.OffsetAddresses(cfg.partyAddrs[:onlineQuorumCount], signStagePortOffset)
	if err != nil {
		log.Fatalf("sign stage addresses: %v", err)
	}
	sigDER, err := maybeRunSignStage(ctx, cfg, tlsConfig, signStageAddrs, access, quorumPartyNames, acKeyBlob, msgHash[:], publicKeyCompressed, "initial")
	if err != nil {
		log.Fatalf("initial sign stage: %v", err)
	}
	if cfg.self == signReceiver {
		log.Printf("party %d initial sign verified, signature bytes=%d", cfg.self, len(sigDER))
	}

	refreshStageAddrs, err := democommon.OffsetAddresses(cfg.partyAddrs, refreshStagePortOffset)
	if err != nil {
		log.Fatalf("refresh stage addresses: %v", err)
	}
	refreshResult, err := democommon.RunECDSAMPACRefresh(ctx, cfg.self, partyNames, refreshStageAddrs, tlsConfig, access, quorumPartyNames, sidOut, acKeyBlob)
	if err != nil {
		log.Fatalf("refresh ac: %v", err)
	}
	refreshedSIDOut := refreshResult.SID
	refreshedKeyBlob := refreshResult.ACKeyBlob
	refreshedPublicKeyCompressed := refreshResult.PublicKeyCompressed
	if !bytes.Equal(publicKeyCompressed, refreshedPublicKeyCompressed) {
		log.Fatalf("refreshed public key mismatch")
	}
	log.Printf("party %d refresh AC complete: sid bytes=%d", cfg.self, len(refreshedSIDOut))

	resignStageAddrs, err := democommon.OffsetAddresses(cfg.partyAddrs[:onlineQuorumCount], resignStagePortOffset)
	if err != nil {
		log.Fatalf("resign stage addresses: %v", err)
	}
	sigDER, err = maybeRunSignStage(ctx, cfg, tlsConfig, resignStageAddrs, access, quorumPartyNames, refreshedKeyBlob, msgHash[:], refreshedPublicKeyCompressed, "refreshed")
	if err != nil {
		log.Fatalf("refreshed sign stage: %v", err)
	}
	if cfg.self == signReceiver {
		log.Printf("party %d refreshed sign verified, signature bytes=%d", cfg.self, len(sigDER))
	} else if cfg.self >= onlineQuorumCount {
		log.Printf("party %d was offline for signing quorum %v and skipped sign stages", cfg.self, quorumPartyIndices)
	}

	log.Printf("party %d ECDSA MP AC showcase complete", cfg.self)
}

func parseConfig() config {
	cfg := config{}
	var partyAddrs string
	flag.IntVar(&cfg.self, "self", -1, "party index (0, 1, or 2)")
	flag.StringVar(&cfg.listenAddr, "listen", "", "base listen address for this party, e.g. 127.0.0.1:9640")
	flag.StringVar(&partyAddrs, "party-addrs", "", "comma-separated base listen addresses for all parties in index order")
	flag.StringVar(&cfg.caFile, "ca", "", "path to CA certificate PEM")
	flag.StringVar(&cfg.certFile, "cert", "", "path to party certificate PEM")
	flag.StringVar(&cfg.keyFile, "key", "", "path to party private key PEM")
	flag.DurationVar(&cfg.timeout, "timeout", 90*time.Second, "overall protocol timeout")
	flag.Parse()

	if cfg.self < 0 || cfg.self >= ecdsaPartyCount {
		log.Fatalf("--self must be in [0,%d)", ecdsaPartyCount)
	}
	cfg.partyAddrs = splitAddresses(partyAddrs)
	if len(cfg.partyAddrs) != ecdsaPartyCount {
		log.Fatalf("--party-addrs must list exactly %d addresses, got %d", ecdsaPartyCount, len(cfg.partyAddrs))
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
	log.Printf("starting party %d with base listen=%s", cfg.self, cfg.listenAddr)
	return cfg
}

func maybeRunSignStage(ctx context.Context, cfg config, tlsConfig *tls.Config, signAddrs []string, access *mpc.AccessStructure, quorumPartyNames []string, acKeyBlob []byte, msgHash []byte, publicKeyCompressed []byte, stage string) ([]byte, error) {
	if cfg.self >= onlineQuorumCount {
		log.Printf("party %d skipping %s sign stage; not in online quorum [0 1]", cfg.self, stage)
		return nil, nil
	}

	signResult, err := democommon.RunECDSAMPACSign(ctx, cfg.self, quorumPartyNames, signAddrs, tlsConfig, access, acKeyBlob, msgHash, publicKeyCompressed, signReceiver)
	if err != nil {
		return nil, fmt.Errorf("%s sign ac: %w", stage, err)
	}
	sigDER := signResult.SignatureDER

	if cfg.self == signReceiver {
		return sigDER, nil
	}

	if len(sigDER) != 0 {
		return nil, fmt.Errorf("party %d unexpectedly received %s signature bytes", cfg.self, stage)
	}
	return nil, nil
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
