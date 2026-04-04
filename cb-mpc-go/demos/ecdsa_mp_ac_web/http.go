package main

import (
	"crypto/sha256"
	"embed"
	"encoding/hex"
	"fmt"
	"html/template"
	"net/http"
	"net/url"
	"strings"

	"github.com/coinbase/cb-mpc-go/pkg/mpc"
)

//go:embed templates/*.html
var templateFS embed.FS

type indexPageData struct {
	Self           int
	HTTPListen     string
	TransportAddrs []string
	PartyNames     []string
	QuorumIndices  []int
	QuorumNames    []string
	Threshold      int
	SignReceiver   int
	FlashError     string
	FlashInfo      string
	AutoRefresh    bool
	Workflows      []workflowView
}

type workflowView struct {
	ID                       string
	Message                  string
	MessageHashHex           string
	Status                   string
	ActiveOperation          string
	LastError                string
	KeyMaterialError         string
	PublicKeyHex             string
	PublicKeyUncompressedHex string
	CurrentSIDHex            string
	CurrentKeyBlobBytes      int
	LocalPublicShareHex      string
	LocalPrivateScalarHex    string
	PreviousPublicShareHex   string
	PreviousPrivateScalarHex string
	PublicShareChanged       bool
	PrivateScalarChanged     bool
	RefreshCount             int
	SignCount                int
	LastSignatureHex         string
	LastSignatureStage       string
	LastSignatureQuorum      string
	LastSignatureReceiver    string
	LastSignatureVerified    bool
	ShowVerificationBundle   bool
	CanRunDKG                bool
	CanRunSign               bool
	CanRunRefresh            bool
	SignButtonLabel          string
	SignQuorumOptions        []signQuorumOption
	Events                   []workflowEvent
}

func loadTemplates() (*template.Template, error) {
	return template.ParseFS(templateFS, "templates/*.html")
}

func (a *app) routes() http.Handler {
	mux := http.NewServeMux()
	mux.HandleFunc("GET /", a.handleIndex)
	mux.HandleFunc("GET /healthz", a.handleHealthz)
	mux.HandleFunc("POST /workflows", a.handleCreateWorkflow)
	mux.HandleFunc("POST /workflows/{id}/actions/dkg", a.handleStartDKG)
	mux.HandleFunc("POST /workflows/{id}/actions/sign", a.handleStartSign)
	mux.HandleFunc("POST /workflows/{id}/actions/refresh", a.handleStartRefresh)
	return mux
}

func (a *app) handleHealthz(w http.ResponseWriter, _ *http.Request) {
	w.WriteHeader(http.StatusOK)
	_, _ = w.Write([]byte("ok"))
}

func (a *app) handleIndex(w http.ResponseWriter, r *http.Request) {
	workflows := a.store.list()
	pageData := indexPageData{
		Self:           a.cfg.self,
		HTTPListen:     a.cfg.httpListen,
		TransportAddrs: append([]string(nil), a.cfg.transportAddrs...),
		PartyNames:     append([]string(nil), a.cfg.partyNames...),
		QuorumIndices:  append([]int(nil), a.cfg.quorumIndices...),
		Threshold:      a.cfg.threshold,
		SignReceiver:   a.cfg.signReceiver,
		FlashError:     r.URL.Query().Get("error"),
		FlashInfo:      r.URL.Query().Get("info"),
		Workflows:      make([]workflowView, 0, len(workflows)),
	}
	quorumNames, err := mpcPartyNamesFromIndices(pageData.PartyNames, pageData.QuorumIndices)
	if err == nil {
		pageData.QuorumNames = quorumNames
	}
	for _, workflow := range workflows {
		view := a.makeWorkflowView(workflow)
		if view.ActiveOperation != "" {
			pageData.AutoRefresh = true
		}
		pageData.Workflows = append(pageData.Workflows, view)
	}
	if err := a.templates.ExecuteTemplate(w, "index.html", pageData); err != nil {
		http.Error(w, fmt.Sprintf("render page: %v", err), http.StatusInternalServerError)
	}
}

func (a *app) makeWorkflowView(workflow *workflowState) workflowView {
	messageHash := sha256.Sum256([]byte(workflow.Message))
	signLabel := "Sign"
	if workflow.SignCount > 0 {
		signLabel = "Sign Again"
	}
	signOptions, err := buildLocalSignQuorumOptions(workflow, a.cfg.self)
	if err != nil {
		signOptions = nil
	}
	lastSignatureQuorum := ""
	lastSignatureReceiver := ""
	if workflow.LastSignatureStage != "" && len(workflow.LastSignatureQuorum) != 0 {
		lastSignatureQuorum = formatPartyIndicesLabel(workflow.LastSignatureQuorum)
		lastSignatureReceiver = fmt.Sprintf("party %d", workflow.LastSignatureReceiver)
	}
	view := workflowView{
		ID:                       workflow.ID,
		Message:                  workflow.Message,
		MessageHashHex:           hex.EncodeToString(messageHash[:]),
		Status:                   workflow.Status,
		ActiveOperation:          workflow.ActiveOperation,
		LastError:                workflow.LastError,
		PublicKeyHex:             fullHex(workflow.PublicKeyCompressed),
		CurrentSIDHex:            fullHex(workflow.CurrentSID),
		CurrentKeyBlobBytes:      len(workflow.CurrentKeyBlob),
		PreviousPublicShareHex:   fullHex(workflow.PreviousPublicShare),
		PreviousPrivateScalarHex: fullHex(workflow.PreviousPrivateScalar),
		RefreshCount:             workflow.RefreshCount,
		SignCount:                workflow.SignCount,
		LastSignatureHex:         fullHex(workflow.LastSignatureDER),
		LastSignatureStage:       workflow.LastSignatureStage,
		LastSignatureQuorum:      lastSignatureQuorum,
		LastSignatureReceiver:    lastSignatureReceiver,
		LastSignatureVerified:    workflow.LastSignatureVerified,
		ShowVerificationBundle:   len(workflow.LastSignatureDER) != 0 && len(workflow.PublicKeyCompressed) != 0,
		CanRunDKG:                workflow.ActiveOperation == "" && len(workflow.CurrentKeyBlob) == 0,
		CanRunSign:               workflow.ActiveOperation == "" && len(workflow.CurrentKeyBlob) != 0 && len(signOptions) != 0,
		CanRunRefresh:            workflow.ActiveOperation == "" && len(workflow.CurrentKeyBlob) != 0 && len(workflow.CurrentSID) != 0,
		SignButtonLabel:          signLabel,
		SignQuorumOptions:        signOptions,
		Events:                   append([]workflowEvent(nil), workflow.Events...),
	}
	material, err := extractKeyMaterial(workflow.CurrentKeyBlob)
	if err != nil {
		view.KeyMaterialError = err.Error()
		return view
	}
	if material != nil {
		view.PublicKeyHex = fullHex(material.JointPublicKey)
		view.PublicKeyUncompressedHex = fullHex(material.JointPublicKeyUncompressed)
		view.LocalPublicShareHex = fullHex(material.PublicShare)
		view.LocalPrivateScalarHex = fullHex(material.PrivateScalar)
		view.PublicShareChanged = len(workflow.PreviousPublicShare) != 0 && !equalBytes(workflow.PreviousPublicShare, material.PublicShare)
		view.PrivateScalarChanged = len(workflow.PreviousPrivateScalar) != 0 && !equalBytes(workflow.PreviousPrivateScalar, material.PrivateScalar)
	}
	return view
}

func shortHex(data []byte) string {
	if len(data) == 0 {
		return ""
	}
	encoded := hex.EncodeToString(data)
	if len(encoded) <= 32 {
		return encoded
	}
	return encoded[:16] + "..." + encoded[len(encoded)-16:]
}

func (a *app) handleCreateWorkflow(w http.ResponseWriter, r *http.Request) {
	if err := r.ParseForm(); err != nil {
		redirectWithError(w, r, fmt.Errorf("parse form: %w", err))
		return
	}
	id := strings.TrimSpace(r.FormValue("id"))
	message := strings.TrimSpace(r.FormValue("message"))
	if _, err := a.store.create(id, message, a.cfg); err != nil {
		redirectWithError(w, r, err)
		return
	}
	redirectWithInfo(w, r, fmt.Sprintf("Created workflow %q", id))
}

func (a *app) handleStartDKG(w http.ResponseWriter, r *http.Request) {
	id := r.PathValue("id")
	if err := a.startOperation(id, "dkg", a.runDKG); err != nil {
		redirectWithError(w, r, err)
		return
	}
	redirectWithInfo(w, r, fmt.Sprintf("Started DKG for workflow %q", id))
}

func (a *app) handleStartSign(w http.ResponseWriter, r *http.Request) {
	if err := r.ParseForm(); err != nil {
		redirectWithError(w, r, fmt.Errorf("parse form: %w", err))
		return
	}
	id := r.PathValue("id")
	quorumRaw := strings.TrimSpace(r.FormValue("quorum-indices"))
	if err := a.startSignOperation(id, quorumRaw); err != nil {
		redirectWithError(w, r, err)
		return
	}
	redirectWithInfo(w, r, fmt.Sprintf("Started sign for workflow %q", id))
}

func (a *app) handleStartRefresh(w http.ResponseWriter, r *http.Request) {
	id := r.PathValue("id")
	if err := a.startOperation(id, "refresh", a.runRefresh); err != nil {
		redirectWithError(w, r, err)
		return
	}
	redirectWithInfo(w, r, fmt.Sprintf("Started refresh for workflow %q", id))
}

func redirectWithError(w http.ResponseWriter, r *http.Request, err error) {
	values := url.Values{}
	values.Set("error", err.Error())
	http.Redirect(w, r, "/?"+values.Encode(), http.StatusSeeOther)
}

func redirectWithInfo(w http.ResponseWriter, r *http.Request, info string) {
	values := url.Values{}
	values.Set("info", info)
	http.Redirect(w, r, "/?"+values.Encode(), http.StatusSeeOther)
}

func mpcPartyNamesFromIndices(partyNames []string, indices []int) ([]string, error) {
	return mpc.PartyNamesFromIndices(partyNames, indices)
}
