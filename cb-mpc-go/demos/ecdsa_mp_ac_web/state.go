package main

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"regexp"
	"sort"
	"sync"
	"time"

	"github.com/coinbase/cb-mpc-go/pkg/mpc"
)

const maxWorkflowEvents = 40

var workflowIDPattern = regexp.MustCompile(`^[a-zA-Z0-9][a-zA-Z0-9._-]{0,63}$`)

type workflowEvent struct {
	Time    time.Time `json:"time"`
	Level   string    `json:"level"`
	Message string    `json:"message"`
}

type workflowState struct {
	ID                    string          `json:"id"`
	Message               string          `json:"message"`
	PartyNames            []string        `json:"partyNames"`
	QuorumIndices         []int           `json:"quorumIndices"`
	QuorumPartyNames      []string        `json:"quorumPartyNames"`
	Threshold             int             `json:"threshold"`
	SignReceiver          int             `json:"signReceiver"`
	Status                string          `json:"status"`
	ActiveOperation       string          `json:"activeOperation,omitempty"`
	ActiveSince           time.Time       `json:"activeSince,omitempty"`
	LastError             string          `json:"lastError,omitempty"`
	PublicKeyCompressed   []byte          `json:"publicKeyCompressed,omitempty"`
	CurrentSID            []byte          `json:"currentSID,omitempty"`
	CurrentKeyBlob        []byte          `json:"currentKeyBlob,omitempty"`
	PreviousPublicShare   []byte          `json:"previousPublicShare,omitempty"`
	PreviousPrivateScalar []byte          `json:"previousPrivateScalar,omitempty"`
	RefreshCount          int             `json:"refreshCount"`
	SignCount             int             `json:"signCount"`
	LastSignatureDER      []byte          `json:"lastSignatureDER,omitempty"`
	LastSignatureVerified bool            `json:"lastSignatureVerified"`
	LastSignatureStage    string          `json:"lastSignatureStage,omitempty"`
	LastSignatureQuorum   []int           `json:"lastSignatureQuorum,omitempty"`
	LastSignatureReceiver int             `json:"lastSignatureReceiver,omitempty"`
	Events                []workflowEvent `json:"events,omitempty"`
	CreatedAt             time.Time       `json:"createdAt"`
	UpdatedAt             time.Time       `json:"updatedAt"`
}

type workflowStore struct {
	rootDir   string
	mu        sync.RWMutex
	workflows map[string]*workflowState
}

func newWorkflowStore(rootDir string) (*workflowStore, error) {
	if err := os.MkdirAll(rootDir, 0o700); err != nil {
		return nil, fmt.Errorf("create data dir: %w", err)
	}
	store := &workflowStore{
		rootDir:   rootDir,
		workflows: make(map[string]*workflowState),
	}
	if err := store.load(); err != nil {
		return nil, err
	}
	return store, nil
}

func (s *workflowStore) load() error {
	entries, err := os.ReadDir(s.rootDir)
	if err != nil {
		return fmt.Errorf("read data dir: %w", err)
	}
	s.mu.Lock()
	defer s.mu.Unlock()
	for _, entry := range entries {
		if entry.IsDir() || filepath.Ext(entry.Name()) != ".json" {
			continue
		}
		path := filepath.Join(s.rootDir, entry.Name())
		data, err := os.ReadFile(path)
		if err != nil {
			return fmt.Errorf("read workflow %q: %w", entry.Name(), err)
		}
		var state workflowState
		if err := json.Unmarshal(data, &state); err != nil {
			return fmt.Errorf("decode workflow %q: %w", entry.Name(), err)
		}
		if state.ActiveOperation != "" {
			appendEvent(&state, "warn", fmt.Sprintf("Recovered after restart while %s was in progress; marked operation as interrupted", state.ActiveOperation))
			state.LastError = fmt.Sprintf("%s interrupted by process restart", state.ActiveOperation)
			state.ActiveOperation = ""
			state.ActiveSince = time.Time{}
		}
		s.workflows[state.ID] = cloneWorkflowState(&state)
	}
	return nil
}

func validateWorkflowID(id string) error {
	if !workflowIDPattern.MatchString(id) {
		return fmt.Errorf("workflow id %q must match %s", id, workflowIDPattern.String())
	}
	return nil
}

func validateWorkflowMessage(message string) error {
	if message == "" {
		return fmt.Errorf("message cannot be empty")
	}
	if len(message) > 4096 {
		return fmt.Errorf("message exceeds maximum length 4096")
	}
	return nil
}

func (s *workflowStore) list() []*workflowState {
	s.mu.RLock()
	defer s.mu.RUnlock()
	out := make([]*workflowState, 0, len(s.workflows))
	for _, workflow := range s.workflows {
		out = append(out, cloneWorkflowState(workflow))
	}
	sort.Slice(out, func(i, j int) bool {
		if out[i].CreatedAt.Equal(out[j].CreatedAt) {
			return out[i].ID < out[j].ID
		}
		return out[i].CreatedAt.Before(out[j].CreatedAt)
	})
	return out
}

func (s *workflowStore) get(id string) (*workflowState, bool) {
	s.mu.RLock()
	defer s.mu.RUnlock()
	workflow, ok := s.workflows[id]
	if !ok {
		return nil, false
	}
	return cloneWorkflowState(workflow), true
}

func (s *workflowStore) create(id string, message string, cfg config) (*workflowState, error) {
	if err := validateWorkflowID(id); err != nil {
		return nil, err
	}
	if err := validateWorkflowMessage(message); err != nil {
		return nil, err
	}
	quorumPartyNames, err := mpc.PartyNamesFromIndices(cfg.partyNames, cfg.quorumIndices)
	if err != nil {
		return nil, err
	}

	now := time.Now().UTC()
	workflow := &workflowState{
		ID:               id,
		Message:          message,
		PartyNames:       append([]string(nil), cfg.partyNames...),
		QuorumIndices:    append([]int(nil), cfg.quorumIndices...),
		QuorumPartyNames: quorumPartyNames,
		Threshold:        cfg.threshold,
		SignReceiver:     cfg.signReceiver,
		Status:           "created",
		CreatedAt:        now,
		UpdatedAt:        now,
	}
	appendEvent(workflow, "info", "Workflow created")

	s.mu.Lock()
	defer s.mu.Unlock()
	if _, exists := s.workflows[id]; exists {
		return nil, fmt.Errorf("workflow %q already exists", id)
	}
	if err := s.persistLocked(workflow); err != nil {
		return nil, err
	}
	s.workflows[id] = cloneWorkflowState(workflow)
	return cloneWorkflowState(workflow), nil
}

func (s *workflowStore) update(id string, fn func(*workflowState) error) (*workflowState, error) {
	s.mu.Lock()
	defer s.mu.Unlock()
	workflow, ok := s.workflows[id]
	if !ok {
		return nil, fmt.Errorf("workflow %q not found", id)
	}
	next := cloneWorkflowState(workflow)
	if err := fn(next); err != nil {
		return nil, err
	}
	next.UpdatedAt = time.Now().UTC()
	if err := s.persistLocked(next); err != nil {
		return nil, err
	}
	s.workflows[id] = cloneWorkflowState(next)
	return cloneWorkflowState(next), nil
}

func (s *workflowStore) persistLocked(state *workflowState) error {
	data, err := json.MarshalIndent(state, "", "  ")
	if err != nil {
		return fmt.Errorf("encode workflow %q: %w", state.ID, err)
	}
	path := filepath.Join(s.rootDir, state.ID+".json")
	tempPath := path + ".tmp"
	if err := os.WriteFile(tempPath, data, 0o600); err != nil {
		return fmt.Errorf("write workflow %q: %w", state.ID, err)
	}
	if err := os.Rename(tempPath, path); err != nil {
		return fmt.Errorf("commit workflow %q: %w", state.ID, err)
	}
	return nil
}

func appendEvent(state *workflowState, level string, message string) {
	state.Events = append(state.Events, workflowEvent{
		Time:    time.Now().UTC(),
		Level:   level,
		Message: message,
	})
	if len(state.Events) > maxWorkflowEvents {
		state.Events = append([]workflowEvent(nil), state.Events[len(state.Events)-maxWorkflowEvents:]...)
	}
}

func cloneWorkflowState(in *workflowState) *workflowState {
	if in == nil {
		return nil
	}
	out := *in
	out.PartyNames = append([]string(nil), in.PartyNames...)
	out.QuorumIndices = append([]int(nil), in.QuorumIndices...)
	out.QuorumPartyNames = append([]string(nil), in.QuorumPartyNames...)
	out.PublicKeyCompressed = append([]byte(nil), in.PublicKeyCompressed...)
	out.CurrentSID = append([]byte(nil), in.CurrentSID...)
	out.CurrentKeyBlob = append([]byte(nil), in.CurrentKeyBlob...)
	out.PreviousPublicShare = append([]byte(nil), in.PreviousPublicShare...)
	out.PreviousPrivateScalar = append([]byte(nil), in.PreviousPrivateScalar...)
	out.LastSignatureDER = append([]byte(nil), in.LastSignatureDER...)
	out.LastSignatureQuorum = append([]int(nil), in.LastSignatureQuorum...)
	out.Events = append([]workflowEvent(nil), in.Events...)
	return &out
}
