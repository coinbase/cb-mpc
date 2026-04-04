package main

import (
	"context"
	"crypto/sha256"
	"crypto/tls"
	"fmt"
	"html/template"
	"sync"
	"time"

	"github.com/coinbase/cb-mpc-go/pkg/mpc"
)

func buildWorkflowAccess(workflow *workflowState) (*mpc.AccessStructure, error) {
	return ThresholdAccessStructure(workflow.PartyNames, workflow.Threshold)
}

func selectTransportAddrs(addrs []string, indices []int) []string {
	out := make([]string, len(indices))
	for i, idx := range indices {
		out[i] = addrs[idx]
	}
	return out
}

func (a *app) startOperation(workflowID string, operation string, runner func(context.Context, *workflowState) error) error {
	_, err := a.store.update(workflowID, func(workflow *workflowState) error {
		if workflow.ActiveOperation != "" {
			return fmt.Errorf("workflow %q already has active operation %q", workflow.ID, workflow.ActiveOperation)
		}
		workflow.ActiveOperation = operation
		workflow.ActiveSince = time.Now().UTC()
		workflow.LastError = ""
		appendEvent(workflow, "info", fmt.Sprintf("Starting %s", operation))
		return nil
	})
	if err != nil {
		return err
	}

	go a.runOperation(workflowID, operation, runner)
	return nil
}

func (a *app) startSignOperation(workflowID string, quorumRaw string) error {
	var req signRequest
	_, err := a.store.update(workflowID, func(workflow *workflowState) error {
		if workflow.ActiveOperation != "" {
			return fmt.Errorf("workflow %q already has active operation %q", workflow.ID, workflow.ActiveOperation)
		}
		parsedReq, err := parseSignRequest(workflow, a.cfg.self, quorumRaw)
		if err != nil {
			return err
		}
		req = parsedReq
		workflow.ActiveOperation = "sign"
		workflow.ActiveSince = time.Now().UTC()
		workflow.LastError = ""
		appendEvent(workflow, "info", fmt.Sprintf("Starting sign with %s (receiver party %d)", formatPartyIndicesLabel(req.QuorumIndices), req.ReceiverPartyIdx))
		return nil
	})
	if err != nil {
		return err
	}

	go a.runOperation(workflowID, "sign", func(ctx context.Context, workflow *workflowState) error {
		return a.runSign(ctx, workflow, req)
	})
	return nil
}

func (a *app) runOperation(workflowID string, operation string, runner func(context.Context, *workflowState) error) {
	ctx, cancel := context.WithTimeout(context.Background(), a.cfg.timeout)
	defer cancel()

	workflow, ok := a.store.get(workflowID)
	if !ok {
		return
	}

	a.protocolMu.Lock()
	err := runner(ctx, workflow)
	a.protocolMu.Unlock()

	if err != nil {
		_, _ = a.store.update(workflowID, func(persisted *workflowState) error {
			persisted.ActiveOperation = ""
			persisted.ActiveSince = time.Time{}
			persisted.LastError = err.Error()
			appendEvent(persisted, "error", fmt.Sprintf("%s failed: %v", operation, err))
			return nil
		})
		return
	}

	_, _ = a.store.update(workflowID, func(persisted *workflowState) error {
		updated := cloneWorkflowState(workflow)
		updated.ActiveOperation = ""
		updated.ActiveSince = time.Time{}
		updated.LastError = ""
		*persisted = *updated
		return nil
	})
}

func (a *app) runDKG(ctx context.Context, workflow *workflowState) error {
	if len(workflow.CurrentKeyBlob) != 0 {
		return fmt.Errorf("workflow %q already has an MPC key", workflow.ID)
	}
	access, err := buildWorkflowAccess(workflow)
	if err != nil {
		return err
	}
	stageAddrs, err := OffsetAddresses(a.cfg.transportAddrs, ECDSAMPACDKGStagePortOffset)
	if err != nil {
		return err
	}
	result, err := RunECDSAMPACDKG(ctx, a.cfg.self, workflow.PartyNames, stageAddrs, a.tlsConfig, mpc.CurveSecp256k1, access, workflow.QuorumPartyNames)
	if err != nil {
		return err
	}
	material, err := extractKeyMaterial(result.ACKeyBlob)
	if err != nil {
		return err
	}
	workflow.CurrentKeyBlob = append([]byte(nil), result.ACKeyBlob...)
	workflow.CurrentSID = append([]byte(nil), result.SID...)
	workflow.PublicKeyCompressed = append([]byte(nil), material.JointPublicKey...)
	workflow.PreviousPublicShare = nil
	workflow.PreviousPrivateScalar = nil
	workflow.Status = "dkg-complete"
	appendEvent(workflow, "info", fmt.Sprintf("DKG complete: public key bytes=%d sid bytes=%d local public share bytes=%d private scalar bytes=%d", len(material.JointPublicKey), len(result.SID), len(material.PublicShare), len(material.PrivateScalar)))
	return nil
}

func (a *app) runSign(ctx context.Context, workflow *workflowState, req signRequest) error {
	if len(workflow.CurrentKeyBlob) == 0 || len(workflow.PublicKeyCompressed) == 0 {
		return fmt.Errorf("workflow %q must complete DKG before signing", workflow.ID)
	}
	selfInQuorum, ok := positionInIndices(req.QuorumIndices, a.cfg.self)
	if !ok {
		return fmt.Errorf("party %d is not in the selected signing quorum %v", a.cfg.self, req.QuorumIndices)
	}
	access, err := buildWorkflowAccess(workflow)
	if err != nil {
		return err
	}
	signAddrs, err := OffsetAddresses(selectTransportAddrs(a.cfg.transportAddrs, req.QuorumIndices), ECDSAMPACSignStagePortOffset)
	if err != nil {
		return err
	}
	messageHash := sha256.Sum256([]byte(workflow.Message))
	result, err := RunECDSAMPACSign(ctx, selfInQuorum, req.QuorumPartyNames, req.QuorumIndices, signAddrs, a.tlsConfig, access, workflow.CurrentKeyBlob, messageHash[:], workflow.PublicKeyCompressed, req.SignReceiver)
	if err != nil {
		return err
	}
	stageNumber := workflow.SignCount + 1
	stageLabel := fmt.Sprintf("sign-%d", stageNumber)
	workflow.SignCount = stageNumber
	workflow.Status = stageLabel + "-complete"
	workflow.LastSignatureStage = stageLabel
	workflow.LastSignatureDER = append([]byte(nil), result.SignatureDER...)
	workflow.LastSignatureQuorum = append([]int(nil), req.QuorumIndices...)
	workflow.LastSignatureReceiver = req.ReceiverPartyIdx
	workflow.LastSignatureVerified = result.IsReceiver
	quorumLabel := formatPartyIndicesLabel(req.QuorumIndices)
	if result.IsReceiver {
		appendEvent(workflow, "info", fmt.Sprintf("%s complete with %s: signature bytes=%d and verification succeeded at party %d", stageLabel, quorumLabel, len(result.SignatureDER), req.ReceiverPartyIdx))
		return nil
	}
	appendEvent(workflow, "info", fmt.Sprintf("%s complete with %s: party contributed to quorum signing", stageLabel, quorumLabel))
	return nil
}

func (a *app) runRefresh(ctx context.Context, workflow *workflowState) error {
	if len(workflow.CurrentKeyBlob) == 0 || len(workflow.CurrentSID) == 0 {
		return fmt.Errorf("workflow %q must complete DKG before refresh", workflow.ID)
	}
	previousMaterial, err := extractKeyMaterial(workflow.CurrentKeyBlob)
	if err != nil {
		return err
	}
	access, err := buildWorkflowAccess(workflow)
	if err != nil {
		return err
	}
	stageAddrs, err := OffsetAddresses(a.cfg.transportAddrs, ECDSAMPACRefreshStagePortOffset)
	if err != nil {
		return err
	}
	result, err := RunECDSAMPACRefresh(ctx, a.cfg.self, workflow.PartyNames, stageAddrs, a.tlsConfig, access, workflow.QuorumPartyNames, workflow.CurrentSID, workflow.CurrentKeyBlob)
	if err != nil {
		return err
	}
	newMaterial, err := extractKeyMaterial(result.ACKeyBlob)
	if err != nil {
		return err
	}
	if len(workflow.PublicKeyCompressed) != 0 && !equalBytes(workflow.PublicKeyCompressed, newMaterial.JointPublicKey) {
		return fmt.Errorf("refresh changed the joint public key")
	}
	workflow.CurrentKeyBlob = append([]byte(nil), result.ACKeyBlob...)
	workflow.CurrentSID = append([]byte(nil), result.SID...)
	workflow.PublicKeyCompressed = append([]byte(nil), newMaterial.JointPublicKey...)
	workflow.PreviousPublicShare = append([]byte(nil), previousMaterial.PublicShare...)
	workflow.PreviousPrivateScalar = append([]byte(nil), previousMaterial.PrivateScalar...)
	workflow.RefreshCount++
	workflow.Status = "refresh-complete"
	appendEvent(workflow, "info", fmt.Sprintf("Refresh complete: sid bytes=%d refresh count=%d local public share changed=%t private scalar changed=%t", len(result.SID), workflow.RefreshCount, !equalBytes(previousMaterial.PublicShare, newMaterial.PublicShare), !equalBytes(previousMaterial.PrivateScalar, newMaterial.PrivateScalar)))
	return nil
}

func positionInIndices(indices []int, target int) (int, bool) {
	for i, idx := range indices {
		if idx == target {
			return i, true
		}
	}
	return 0, false
}

func equalBytes(a, b []byte) bool {
	if len(a) != len(b) {
		return false
	}
	for i := range a {
		if a[i] != b[i] {
			return false
		}
	}
	return true
}

type app struct {
	cfg        config
	tlsConfig  *tls.Config
	store      *workflowStore
	templates  *template.Template
	protocolMu sync.Mutex
}
