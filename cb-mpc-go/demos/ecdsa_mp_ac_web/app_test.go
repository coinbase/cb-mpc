package main

import (
	"encoding/hex"
	"net/http"
	"net/http/httptest"
	"net/url"
	"strings"
	"testing"
	"time"
)

func testWebDemoConfig(dataDir string) config {
	return config{
		self:           0,
		httpListen:     "127.0.0.1:7080",
		transportAddrs: []string{"127.0.0.1:9680", "127.0.0.1:9681", "127.0.0.1:9682"},
		caFile:         "ca.pem",
		certFile:       "cert.pem",
		keyFile:        "key.pem",
		dataDir:        dataDir,
		timeout:        10 * time.Second,
		partyNames:     DefaultECDSAMPACPartyNames(),
		quorumIndices:  []int{0, 1},
		threshold:      2,
		signReceiver:   0,
	}
}

func newTestApp(t *testing.T) *app {
	t.Helper()
	store, err := newWorkflowStore(t.TempDir())
	if err != nil {
		t.Fatalf("newWorkflowStore: %v", err)
	}
	templates, err := loadTemplates()
	if err != nil {
		t.Fatalf("loadTemplates: %v", err)
	}
	return &app{
		cfg:       testWebDemoConfig(store.rootDir),
		store:     store,
		templates: templates,
	}
}

func TestParseConfigFromArgs(t *testing.T) {
	cfg, err := parseConfigFromArgs([]string{
		"--self", "1",
		"--http-listen", "127.0.0.1:7081",
		"--transport-addrs", "127.0.0.1:9680,127.0.0.1:9681,127.0.0.1:9682",
		"--ca", "ca.pem",
		"--cert", "cert.pem",
		"--key", "key.pem",
	})
	if err != nil {
		t.Fatalf("parseConfigFromArgs: %v", err)
	}
	if got, want := cfg.self, 1; got != want {
		t.Fatalf("self mismatch: got %d want %d", got, want)
	}
	if got, want := len(cfg.partyNames), 3; got != want {
		t.Fatalf("party name count mismatch: got %d want %d", got, want)
	}
	if got, want := cfg.threshold, 2; got != want {
		t.Fatalf("threshold mismatch: got %d want %d", got, want)
	}
	if got, want := cfg.signReceiver, 0; got != want {
		t.Fatalf("sign receiver mismatch: got %d want %d", got, want)
	}
}

func TestParseConfigFromArgsRejectsWeakQuorum(t *testing.T) {
	_, err := parseConfigFromArgs([]string{
		"--self", "0",
		"--http-listen", "127.0.0.1:7080",
		"--transport-addrs", "127.0.0.1:9680,127.0.0.1:9681,127.0.0.1:9682",
		"--ca", "ca.pem",
		"--cert", "cert.pem",
		"--key", "key.pem",
		"--threshold", "2",
		"--quorum-indices", "0",
	})
	if err == nil || !strings.Contains(err.Error(), "at least threshold") {
		t.Fatalf("expected threshold/quorum validation error, got %v", err)
	}
}

func TestParseConfigFromArgsRejectsSignReceiverOutsideThreshold(t *testing.T) {
	_, err := parseConfigFromArgs([]string{
		"--self", "0",
		"--http-listen", "127.0.0.1:7080",
		"--transport-addrs", "127.0.0.1:9680,127.0.0.1:9681,127.0.0.1:9682",
		"--ca", "ca.pem",
		"--cert", "cert.pem",
		"--key", "key.pem",
		"--threshold", "2",
		"--quorum-indices", "0,1,2",
		"--sign-receiver", "2",
	})
	if err == nil || !strings.Contains(err.Error(), "--sign-receiver 2 out of range [0, 2)") {
		t.Fatalf("expected sign receiver validation error, got %v", err)
	}
}

func TestWorkflowStoreCreateAndReload(t *testing.T) {
	cfg := testWebDemoConfig(t.TempDir())
	store, err := newWorkflowStore(cfg.dataDir)
	if err != nil {
		t.Fatalf("newWorkflowStore: %v", err)
	}
	created, err := store.create("wallet-demo-001", "Pay demo merchant", cfg)
	if err != nil {
		t.Fatalf("create workflow: %v", err)
	}
	if got, want := created.Status, "created"; got != want {
		t.Fatalf("status mismatch: got %q want %q", got, want)
	}

	reloaded, err := newWorkflowStore(cfg.dataDir)
	if err != nil {
		t.Fatalf("reload workflow store: %v", err)
	}
	workflow, ok := reloaded.get("wallet-demo-001")
	if !ok {
		t.Fatal("expected workflow to exist after reload")
	}
	if got, want := workflow.Message, "Pay demo merchant"; got != want {
		t.Fatalf("message mismatch: got %q want %q", got, want)
	}
	if got, want := len(workflow.QuorumPartyNames), 2; got != want {
		t.Fatalf("quorum size mismatch: got %d want %d", got, want)
	}
}

func TestBuildLocalSignQuorumOptions(t *testing.T) {
	workflow := &workflowState{
		PartyNames:   DefaultECDSAMPACPartyNames(),
		Threshold:    2,
		SignReceiver: 0,
	}
	options, err := buildLocalSignQuorumOptions(workflow, 1)
	if err != nil {
		t.Fatalf("buildLocalSignQuorumOptions: %v", err)
	}
	if got, want := len(options), 2; got != want {
		t.Fatalf("option count mismatch: got %d want %d", got, want)
	}
	if got, want := options[0].Value, "0,1"; got != want {
		t.Fatalf("first option mismatch: got %q want %q", got, want)
	}
	if got, want := options[1].Value, "1,2"; got != want {
		t.Fatalf("second option mismatch: got %q want %q", got, want)
	}
	if got, want := options[1].Label, "Parties 1 + 2 (receiver party 1)"; got != want {
		t.Fatalf("second option label mismatch: got %q want %q", got, want)
	}
}

func TestParseSignRequestRejectsQuorumWithoutSelf(t *testing.T) {
	workflow := &workflowState{
		PartyNames:   DefaultECDSAMPACPartyNames(),
		Threshold:    2,
		SignReceiver: 0,
	}
	_, err := parseSignRequest(workflow, 0, "1,2")
	if err == nil || !strings.Contains(err.Error(), "party 0 is not in selected sign quorum") {
		t.Fatalf("expected self-in-quorum validation error, got %v", err)
	}
}

func TestParseSignRequestCanonicalizesQuorumOrder(t *testing.T) {
	workflow := &workflowState{
		PartyNames:   DefaultECDSAMPACPartyNames(),
		Threshold:    2,
		SignReceiver: 0,
	}
	req, err := parseSignRequest(workflow, 2, "2,0")
	if err != nil {
		t.Fatalf("parseSignRequest: %v", err)
	}
	if got, want := joinIndicesCSV(req.QuorumIndices), "0,2"; got != want {
		t.Fatalf("quorum order mismatch: got %q want %q", got, want)
	}
	if got, want := req.ReceiverPartyIdx, 0; got != want {
		t.Fatalf("receiver mismatch: got %d want %d", got, want)
	}
}

func TestExpandSecp256k1PublicKey(t *testing.T) {
	compressed, err := hex.DecodeString("026d17e27e22aa500b8c417363f3caf5b08de5d1fcc9bf11ab1a40ac6109d12e98")
	if err != nil {
		t.Fatalf("decode compressed key: %v", err)
	}
	uncompressed, err := expandSecp256k1PublicKey(compressed)
	if err != nil {
		t.Fatalf("expandSecp256k1PublicKey: %v", err)
	}
	if got, want := hex.EncodeToString(uncompressed), "046d17e27e22aa500b8c417363f3caf5b08de5d1fcc9bf11ab1a40ac6109d12e98d0825c102f31a7e9ba14c7c3dc1169a20daee838dbc403c0b50c1e0d797df6b2"; got != want {
		t.Fatalf("uncompressed key mismatch: got %q want %q", got, want)
	}
}

func TestCreateWorkflowHandlerAndDashboard(t *testing.T) {
	app := newTestApp(t)
	form := url.Values{
		"id":      {"wallet-demo-001"},
		"message": {"Pay demo merchant"},
	}
	req := httptest.NewRequest(http.MethodPost, "/workflows", strings.NewReader(form.Encode()))
	req.Header.Set("Content-Type", "application/x-www-form-urlencoded")
	rec := httptest.NewRecorder()

	app.routes().ServeHTTP(rec, req)
	if got, want := rec.Code, http.StatusSeeOther; got != want {
		t.Fatalf("unexpected status: got %d want %d", got, want)
	}

	indexReq := httptest.NewRequest(http.MethodGet, "/", nil)
	indexRec := httptest.NewRecorder()
	app.routes().ServeHTTP(indexRec, indexReq)
	if got, want := indexRec.Code, http.StatusOK; got != want {
		t.Fatalf("unexpected index status: got %d want %d", got, want)
	}
	body := indexRec.Body.String()
	if !strings.Contains(body, "wallet-demo-001") {
		t.Fatalf("expected workflow ID in dashboard body")
	}
	if !strings.Contains(body, "Run DKG") {
		t.Fatalf("expected DKG button in dashboard body")
	}
	if !strings.Contains(body, "name=\"quorum-indices\"") {
		t.Fatalf("expected sign quorum selector in dashboard body")
	}
	if !strings.Contains(body, "value=\"0,1\"") || !strings.Contains(body, "value=\"0,2\"") {
		t.Fatalf("expected local sign quorum options for parties 0+1 and 0+2")
	}
}
