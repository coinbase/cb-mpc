package main

import (
	"log"
	"net/http"
	"os"
	"time"
)

func main() {
	cfg, err := parseConfigFromArgs(os.Args[1:])
	if err != nil {
		log.Fatalf("parse config: %v", err)
	}

	tlsConfig, err := LoadTLSConfig(cfg.caFile, cfg.certFile, cfg.keyFile, false)
	if err != nil {
		log.Fatalf("load TLS config: %v", err)
	}

	store, err := newWorkflowStore(cfg.dataDir)
	if err != nil {
		log.Fatalf("open workflow store: %v", err)
	}

	templates, err := loadTemplates()
	if err != nil {
		log.Fatalf("load templates: %v", err)
	}

	app := &app{
		cfg:       cfg,
		tlsConfig: tlsConfig,
		store:     store,
		templates: templates,
	}

	server := &http.Server{
		Addr:              cfg.httpListen,
		Handler:           app.routes(),
		ReadHeaderTimeout: 5 * time.Second,
	}

	log.Printf("ECDSA MP AC web demo party %d listening on http://%s", cfg.self, cfg.httpListen)
	log.Printf("transport base addresses: %v", cfg.transportAddrs)
	log.Printf("party names: %v", cfg.partyNames)
	log.Printf("quorum indices: %v (sign receiver within quorum=%d)", cfg.quorumIndices, cfg.signReceiver)
	log.Printf("persisting party state under %s", cfg.dataDir)

	if err := server.ListenAndServe(); err != nil && err != http.ErrServerClosed {
		log.Fatalf("serve: %v", err)
	}
}
