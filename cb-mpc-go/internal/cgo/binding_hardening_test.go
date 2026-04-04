package cgo

import (
	"bytes"
	"context"
	"errors"
	"math"
	"strings"
	"testing"
	"unsafe"
)

type panicTransport struct {
	sendPanic       bool
	receivePanic    bool
	receiveAllPanic bool
}

func (s panicTransport) Send(_ context.Context, _ int, _ []byte) error {
	if s.sendPanic {
		panic("send panic")
	}
	return nil
}

func (s panicTransport) Receive(_ context.Context, _ int) ([]byte, error) {
	if s.receivePanic {
		panic("receive panic")
	}
	return nil, nil
}

func (s panicTransport) ReceiveAll(_ context.Context, _ []int) ([][]byte, error) {
	if s.receiveAllPanic {
		panic("receive-all panic")
	}
	return nil, nil
}

type validationTransport struct{}

func (s validationTransport) Send(_ context.Context, _ int, _ []byte) error {
	return errors.New("unexpected Send")
}

func (s validationTransport) Receive(_ context.Context, _ int) ([]byte, error) {
	return nil, errors.New("unexpected Receive")
}

func (s validationTransport) ReceiveAll(_ context.Context, _ []int) ([][]byte, error) {
	return nil, errors.New("unexpected ReceiveAll")
}

type messageTransport struct {
	receiveMsg     []byte
	receiveAllMsgs [][]byte
}

func (s messageTransport) Send(_ context.Context, _ int, _ []byte) error {
	return nil
}

func (s messageTransport) Receive(_ context.Context, _ int) ([]byte, error) {
	return append([]byte(nil), s.receiveMsg...), nil
}

func (s messageTransport) ReceiveAll(_ context.Context, _ []int) ([][]byte, error) {
	out := make([][]byte, len(s.receiveAllMsgs))
	for i := range s.receiveAllMsgs {
		out[i] = append([]byte(nil), s.receiveAllMsgs[i]...)
	}
	return out, nil
}

func hardeningPartyNames() []string {
	return []string{
		"urn:cb-mpc-go:test:cgo-hardening:alice",
		"urn:cb-mpc-go:test:cgo-hardening:bob",
		"urn:cb-mpc-go:test:cgo-hardening:carol",
	}
}

func validThresholdAccess() *AccessStructure {
	partyNames := hardeningPartyNames()
	return &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeLeaf, LeafName: partyNames[0]},
			{Type: AccessNodeLeaf, LeafName: partyNames[1]},
			{Type: AccessNodeLeaf, LeafName: partyNames[2]},
			{Type: AccessNodeThreshold, ThresholdK: 2, Children: []int32{0, 1, 2}},
		},
		Root: 3,
	}
}

var oversizedCallbackBacking byte

func oversizedCallbackBytes(t *testing.T) []byte {
	t.Helper()
	oversizedLen := int64(math.MaxInt32) + 1
	if unsafe.Sizeof(int(0)) < 8 {
		t.Skip("oversized callback output test requires 64-bit ints")
	}
	return unsafe.Slice(&oversizedCallbackBacking, int(oversizedLen))
}

func TestByteSlicesToOwnedCmems_RoundTrip(t *testing.T) {
	in := [][]byte{
		[]byte("a"),
		{},
		[]byte("bc"),
	}

	mems, cleanup, err := byteSlicesToOwnedCmems(in)
	if err != nil {
		t.Fatalf("byteSlicesToOwnedCmems: %v", err)
	}
	defer cleanup()

	got, err := cmemsToByteSlices(mems)
	if err != nil {
		t.Fatalf("cmemsToByteSlices: %v", err)
	}
	if len(got) != len(in) {
		t.Fatalf("got %d slices, want %d", len(got), len(in))
	}
	for i := range in {
		if !bytes.Equal(got[i], in[i]) {
			t.Fatalf("slice %d mismatch: got=%x want=%x", i, got[i], in[i])
		}
	}
}

func TestCreateCStringArray_RejectsNUL(t *testing.T) {
	if _, _, err := createCStringArray([]string{"urn:cb-mpc-go:test:cgo-hardening:alice\x00evil"}); err == nil {
		t.Fatalf("expected error for NUL-containing string")
	}
}

func TestCompileAccessStructure_ValidThreshold(t *testing.T) {
	partyNames := hardeningPartyNames()
	access := &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeLeaf, LeafName: partyNames[0]},
			{Type: AccessNodeLeaf, LeafName: partyNames[1]},
			{Type: AccessNodeLeaf, LeafName: partyNames[2]},
			{Type: AccessNodeThreshold, ThresholdK: 2, Children: []int32{0, 1, 2}},
		},
		Root: 3,
	}

	compiled, err := compileAccessStructure(access)
	if err != nil {
		t.Fatalf("compileAccessStructure: %v", err)
	}
	defer compiled.free()
}

func TestCompileAccessStructure_RejectsLeafRoot(t *testing.T) {
	access := &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeLeaf, LeafName: "urn:cb-mpc-go:test:cgo-hardening:leaf-root"},
		},
		Root: 0,
	}
	if _, err := compileAccessStructure(access); err == nil {
		t.Fatalf("expected error for leaf root")
	}
}

func TestCompileAccessStructure_RejectsEmptyLeafName(t *testing.T) {
	access := &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeLeaf, LeafName: ""},
			{Type: AccessNodeAnd, Children: []int32{0}},
		},
		Root: 1,
	}
	if _, err := compileAccessStructure(access); err == nil {
		t.Fatalf("expected error for empty leaf name")
	}
}

func TestCompileAccessStructure_RejectsNULLeafName(t *testing.T) {
	access := &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeLeaf, LeafName: "urn:cb-mpc-go:test:cgo-hardening:nul\x00evil"},
			{Type: AccessNodeAnd, Children: []int32{0}},
		},
		Root: 1,
	}
	if _, err := compileAccessStructure(access); err == nil {
		t.Fatalf("expected error for NUL leaf name")
	}
}

func TestCompileAccessStructure_RejectsLeafWithChildren(t *testing.T) {
	access := &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeLeaf, LeafName: "urn:cb-mpc-go:test:cgo-hardening:leaf-with-children", Children: []int32{0}},
			{Type: AccessNodeAnd, Children: []int32{0}},
		},
		Root: 1,
	}
	if _, err := compileAccessStructure(access); err == nil {
		t.Fatalf("expected error for leaf with children")
	}
}

func TestCompileAccessStructure_RejectsThresholdKOutOfRange(t *testing.T) {
	access := &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeLeaf, LeafName: "urn:cb-mpc-go:test:cgo-hardening:threshold-alice"},
			{Type: AccessNodeLeaf, LeafName: "urn:cb-mpc-go:test:cgo-hardening:threshold-bob"},
			{Type: AccessNodeThreshold, ThresholdK: 3, Children: []int32{0, 1}},
		},
		Root: 2,
	}
	if _, err := compileAccessStructure(access); err == nil {
		t.Fatalf("expected error for threshold_k out of range")
	}
}

func TestCompileAccessStructure_RejectsOutOfRangeChildIndex(t *testing.T) {
	access := &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeLeaf, LeafName: "urn:cb-mpc-go:test:cgo-hardening:range-check"},
			{Type: AccessNodeAnd, Children: []int32{99}},
		},
		Root: 1,
	}
	if _, err := compileAccessStructure(access); err == nil {
		t.Fatalf("expected error for out-of-range child index")
	}
}

func TestCompileAccessStructure_RejectsCycle(t *testing.T) {
	access := &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeAnd, Children: []int32{1}},
			{Type: AccessNodeAnd, Children: []int32{0}},
		},
		Root: 0,
	}
	if _, err := compileAccessStructure(access); err == nil {
		t.Fatalf("expected error for cycle")
	}
}

func TestCompileAccessStructure_RejectsNodeReuse(t *testing.T) {
	access := &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeAnd, Children: []int32{1, 2}},
			{Type: AccessNodeAnd, Children: []int32{2}},
			{Type: AccessNodeLeaf, LeafName: "urn:cb-mpc-go:test:cgo-hardening:node-reuse"},
		},
		Root: 0,
	}
	if _, err := compileAccessStructure(access); err == nil {
		t.Fatalf("expected error for node reuse")
	}
}

func TestCompileAccessStructure_RejectsUnreachableNode(t *testing.T) {
	access := &AccessStructure{
		Nodes: []AccessStructureNode{
			{Type: AccessNodeAnd, Children: []int32{1}},
			{Type: AccessNodeLeaf, LeafName: "urn:cb-mpc-go:test:cgo-hardening:reachable"},
			{Type: AccessNodeLeaf, LeafName: "urn:cb-mpc-go:test:cgo-hardening:unreachable"},
		},
		Root: 0,
	}
	if _, err := compileAccessStructure(access); err == nil {
		t.Fatalf("expected error for unreachable node")
	}
}

func TestPVEACPartyDecryptRow_RejectsEmptyLeafName(t *testing.T) {
	if _, err := PVEACPartyDecryptRow(nil, CurveP256, nil, nil, 0, "", nil, nil); err == nil {
		t.Fatalf("expected error for empty leafName")
	}
}

func TestPVEACPartyDecryptRow_RejectsNULLeafName(t *testing.T) {
	if _, err := PVEACPartyDecryptRow(nil, CurveP256, nil, nil, 0, "urn:cb-mpc-go:test:cgo-hardening:pve\x00evil", nil, nil); err == nil {
		t.Fatalf("expected error for NUL-containing leafName")
	}
}

func TestPVEACPartyDecryptRow_RejectsNegativeRowIndex(t *testing.T) {
	if _, err := PVEACPartyDecryptRow(nil, CurveP256, nil, nil, -1, "urn:cb-mpc-go:test:cgo-hardening:pve-row", nil, nil); err == nil {
		t.Fatalf("expected error for negative rowIndex")
	}
}

func TestPVEACAggregateToRestoreRow_RejectsNegativeRowIndex(t *testing.T) {
	if _, err := PVEACAggregateToRestoreRow(nil, CurveP256, nil, []string{"urn:cb-mpc-go:test:cgo-hardening:aggregate-row"}, [][]byte{{0x01}}, nil, -1, nil); err == nil {
		t.Fatalf("expected error for negative rowIndex")
	}
}

func TestPVEACPartyDecryptRowRSAOAEPHSM_RejectsNULLeafName(t *testing.T) {
	callbacks := &PVERSAOAEPHSMDecapCallbacks{
		Decap: func(dkHandle, kemCT []byte) ([]byte, error) { return nil, nil },
	}
	if _, err := PVEACPartyDecryptRowRSAOAEPHSM(CurveP256, nil, nil, 0, "urn:cb-mpc-go:test:cgo-hardening:rsa\x00evil", nil, nil, nil, callbacks); err == nil {
		t.Fatalf("expected error for NUL-containing leafName")
	}
}

func TestPVEACPartyDecryptRowECIESP256HSM_RejectsNULLeafName(t *testing.T) {
	callbacks := &PVEECIESP256HSMECDHCallbacks{
		ECDH: func(dkHandle, kemCT []byte) ([]byte, error) { return nil, nil },
	}
	if _, err := PVEACPartyDecryptRowECIESP256HSM(CurveP256, nil, nil, 0, "urn:cb-mpc-go:test:cgo-hardening:ecies\x00evil", nil, nil, nil, callbacks); err == nil {
		t.Fatalf("expected error for NUL-containing leafName")
	}
}

func TestGoTransportCallbacks_RecoverTransportPanics(t *testing.T) {
	t.Run("send", func(t *testing.T) {
		ctxPtr, _, err := registerTransport(context.Background(), panicTransport{sendPanic: true})
		if err != nil {
			t.Fatalf("registerTransport: %v", err)
		}
		defer unregisterTransport(ctxPtr)

		code := testGoTransportSendCode(ctxPtr)
		if code != testCBMPCENetGeneral {
			t.Fatalf("goTransportSend returned %d, want %d", code, testCBMPCENetGeneral)
		}
	})

	t.Run("receive", func(t *testing.T) {
		ctxPtr, _, err := registerTransport(context.Background(), panicTransport{receivePanic: true})
		if err != nil {
			t.Fatalf("registerTransport: %v", err)
		}
		defer unregisterTransport(ctxPtr)

		code, zeroed := testGoTransportReceiveResult(ctxPtr)
		if code != testCBMPCENetGeneral {
			t.Fatalf("goTransportReceive returned %d, want %d", code, testCBMPCENetGeneral)
		}
		if !zeroed {
			t.Fatalf("goTransportReceive should leave outputs zeroed after panic")
		}
	})

	t.Run("receive all", func(t *testing.T) {
		ctxPtr, _, err := registerTransport(context.Background(), panicTransport{receiveAllPanic: true})
		if err != nil {
			t.Fatalf("registerTransport: %v", err)
		}
		defer unregisterTransport(ctxPtr)

		code, zeroed := testGoTransportReceiveAllResult(ctxPtr)
		if code != testCBMPCENetGeneral {
			t.Fatalf("goTransportReceiveAll returned %d, want %d", code, testCBMPCENetGeneral)
		}
		if !zeroed {
			t.Fatalf("goTransportReceiveAll should leave outputs zeroed after panic")
		}
	})

	t.Run("receive after allocation", func(t *testing.T) {
		ctxPtr, _, err := registerTransport(context.Background(), messageTransport{receiveMsg: []byte("allocated")})
		if err != nil {
			t.Fatalf("registerTransport: %v", err)
		}
		defer unregisterTransport(ctxPtr)

		called := false
		testHookAfterTransportReceiveAlloc = func() {
			called = true
			panic("receive panic after allocation")
		}
		t.Cleanup(func() { testHookAfterTransportReceiveAlloc = nil })

		code, zeroed := testGoTransportReceiveResult(ctxPtr)
		if !called {
			t.Fatalf("expected receive allocation hook to run")
		}
		if code != testCBMPCENetGeneral {
			t.Fatalf("goTransportReceive returned %d, want %d", code, testCBMPCENetGeneral)
		}
		if !zeroed {
			t.Fatalf("goTransportReceive should free and zero outputs after post-allocation panic")
		}
	})

	t.Run("receive all after allocation", func(t *testing.T) {
		ctxPtr, _, err := registerTransport(context.Background(), messageTransport{receiveAllMsgs: [][]byte{[]byte("allocated")}})
		if err != nil {
			t.Fatalf("registerTransport: %v", err)
		}
		defer unregisterTransport(ctxPtr)

		called := false
		testHookAfterTransportReceiveAllAlloc = func() {
			called = true
			panic("receive-all panic after allocation")
		}
		t.Cleanup(func() { testHookAfterTransportReceiveAllAlloc = nil })

		code, zeroed := testGoTransportReceiveAllWithSendersResult(ctxPtr, []int32{1})
		if !called {
			t.Fatalf("expected receive-all allocation hook to run")
		}
		if code != testCBMPCENetGeneral {
			t.Fatalf("goTransportReceiveAll returned %d, want %d", code, testCBMPCENetGeneral)
		}
		if !zeroed {
			t.Fatalf("goTransportReceiveAll should free and zero outputs after post-allocation panic")
		}
	})
}

func TestGoPVECallbacks_RecoverPanics(t *testing.T) {
	t.Run("base pke encrypt", func(t *testing.T) {
		ctxPtr, err := registerPVEBasePKECallbacks(&PVEBasePKECallbacks{
			Encrypt: func(ek, label, plain, rho []byte) ([]byte, error) {
				panic("encrypt panic")
			},
			Decrypt: func(dk, label, ct []byte) ([]byte, error) {
				return nil, nil
			},
		})
		if err != nil {
			t.Fatalf("registerPVEBasePKECallbacks: %v", err)
		}
		defer unregisterPVEBasePKECallbacks(ctxPtr)

		code, zeroed := testGoPVEBasePKEEncryptResult(ctxPtr)
		if code != testCBMPCEGeneral {
			t.Fatalf("goPVEBasePKEEncrypt returned %d, want %d", code, testCBMPCEGeneral)
		}
		if !zeroed {
			t.Fatalf("goPVEBasePKEEncrypt should leave outputs zeroed after panic")
		}
	})

	t.Run("base pke decrypt", func(t *testing.T) {
		ctxPtr, err := registerPVEBasePKECallbacks(&PVEBasePKECallbacks{
			Encrypt: func(ek, label, plain, rho []byte) ([]byte, error) {
				return nil, nil
			},
			Decrypt: func(dk, label, ct []byte) ([]byte, error) {
				panic("decrypt panic")
			},
		})
		if err != nil {
			t.Fatalf("registerPVEBasePKECallbacks: %v", err)
		}
		defer unregisterPVEBasePKECallbacks(ctxPtr)

		code, zeroed := testGoPVEBasePKEDecryptResult(ctxPtr)
		if code != testCBMPCEGeneral {
			t.Fatalf("goPVEBasePKEDecrypt returned %d, want %d", code, testCBMPCEGeneral)
		}
		if !zeroed {
			t.Fatalf("goPVEBasePKEDecrypt should leave outputs zeroed after panic")
		}
	})

	t.Run("rsa oaep hsm decap", func(t *testing.T) {
		ctxPtr, err := registerPVERSAOAEPHSMDecapCallbacks(&PVERSAOAEPHSMDecapCallbacks{
			Decap: func(dkHandle, kemCT []byte) ([]byte, error) {
				panic("decap panic")
			},
		})
		if err != nil {
			t.Fatalf("registerPVERSAOAEPHSMDecapCallbacks: %v", err)
		}
		defer unregisterPVERSAOAEPHSMDecapCallbacks(ctxPtr)

		code, zeroed := testGoPVERSAOAEPHSMDecapResult(ctxPtr)
		if code != testCBMPCEGeneral {
			t.Fatalf("goPVERSAOAEPHSMDecap returned %d, want %d", code, testCBMPCEGeneral)
		}
		if !zeroed {
			t.Fatalf("goPVERSAOAEPHSMDecap should leave outputs zeroed after panic")
		}
	})

	t.Run("ecies p256 hsm ecdh", func(t *testing.T) {
		ctxPtr, err := registerPVEECIESP256HSMECDHCallbacks(&PVEECIESP256HSMECDHCallbacks{
			ECDH: func(dkHandle, kemCT []byte) ([]byte, error) {
				panic("ecdh panic")
			},
		})
		if err != nil {
			t.Fatalf("registerPVEECIESP256HSMECDHCallbacks: %v", err)
		}
		defer unregisterPVEECIESP256HSMECDHCallbacks(ctxPtr)

		code, zeroed := testGoPVEECIESP256HSMECDHResult(ctxPtr)
		if code != testCBMPCEGeneral {
			t.Fatalf("goPVEECIESP256HSMECDH returned %d, want %d", code, testCBMPCEGeneral)
		}
		if !zeroed {
			t.Fatalf("goPVEECIESP256HSMECDH should leave outputs zeroed after panic")
		}
	})

	t.Run("kem encap", func(t *testing.T) {
		ctxPtr, err := registerPVEBaseKEMCallbacks(&PVEBaseKEMCallbacks{
			Encap: func(ek, rho32 []byte) ([]byte, []byte, error) {
				panic("encap panic")
			},
			Decap: func(dk, kemCT []byte) ([]byte, error) {
				return nil, nil
			},
		})
		if err != nil {
			t.Fatalf("registerPVEBaseKEMCallbacks: %v", err)
		}
		defer unregisterPVEBaseKEMCallbacks(ctxPtr)

		code, zeroCT, zeroSS := testGoPVEKEMEncapResult(ctxPtr)
		if code != testCBMPCEGeneral {
			t.Fatalf("goPVEKEMEncap returned %d, want %d", code, testCBMPCEGeneral)
		}
		if !zeroCT || !zeroSS {
			t.Fatalf("goPVEKEMEncap should leave both outputs zeroed after panic")
		}
	})

	t.Run("kem decap", func(t *testing.T) {
		ctxPtr, err := registerPVEBaseKEMCallbacks(&PVEBaseKEMCallbacks{
			Encap: func(ek, rho32 []byte) ([]byte, []byte, error) {
				return nil, nil, nil
			},
			Decap: func(dk, kemCT []byte) ([]byte, error) {
				panic("decap panic")
			},
		})
		if err != nil {
			t.Fatalf("registerPVEBaseKEMCallbacks: %v", err)
		}
		defer unregisterPVEBaseKEMCallbacks(ctxPtr)

		code, zeroed := testGoPVEKEMDecapResult(ctxPtr)
		if code != testCBMPCEGeneral {
			t.Fatalf("goPVEKEMDecap returned %d, want %d", code, testCBMPCEGeneral)
		}
		if !zeroed {
			t.Fatalf("goPVEKEMDecap should leave outputs zeroed after panic")
		}
	})
}

func TestGoPVECallbacks_RejectOversizedOutputs(t *testing.T) {
	oversized := oversizedCallbackBytes(t)

	t.Run("base pke encrypt", func(t *testing.T) {
		ctxPtr, err := registerPVEBasePKECallbacks(&PVEBasePKECallbacks{
			Encrypt: func(ek, label, plain, rho []byte) ([]byte, error) { return oversized, nil },
			Decrypt: func(dk, label, ct []byte) ([]byte, error) { return nil, nil },
		})
		if err != nil {
			t.Fatalf("registerPVEBasePKECallbacks: %v", err)
		}
		defer unregisterPVEBasePKECallbacks(ctxPtr)

		code, zeroed := testGoPVEBasePKEEncryptResult(ctxPtr)
		if code != testCBMPCERange {
			t.Fatalf("goPVEBasePKEEncrypt returned %d, want %d", code, testCBMPCERange)
		}
		if !zeroed {
			t.Fatalf("goPVEBasePKEEncrypt should leave outputs zeroed on oversized callback output")
		}
	})

	t.Run("base pke decrypt", func(t *testing.T) {
		ctxPtr, err := registerPVEBasePKECallbacks(&PVEBasePKECallbacks{
			Encrypt: func(ek, label, plain, rho []byte) ([]byte, error) { return nil, nil },
			Decrypt: func(dk, label, ct []byte) ([]byte, error) { return oversized, nil },
		})
		if err != nil {
			t.Fatalf("registerPVEBasePKECallbacks: %v", err)
		}
		defer unregisterPVEBasePKECallbacks(ctxPtr)

		code, zeroed := testGoPVEBasePKEDecryptResult(ctxPtr)
		if code != testCBMPCERange {
			t.Fatalf("goPVEBasePKEDecrypt returned %d, want %d", code, testCBMPCERange)
		}
		if !zeroed {
			t.Fatalf("goPVEBasePKEDecrypt should leave outputs zeroed on oversized callback output")
		}
	})

	t.Run("rsa oaep hsm decap", func(t *testing.T) {
		ctxPtr, err := registerPVERSAOAEPHSMDecapCallbacks(&PVERSAOAEPHSMDecapCallbacks{
			Decap: func(dkHandle, kemCT []byte) ([]byte, error) { return oversized, nil },
		})
		if err != nil {
			t.Fatalf("registerPVERSAOAEPHSMDecapCallbacks: %v", err)
		}
		defer unregisterPVERSAOAEPHSMDecapCallbacks(ctxPtr)

		code, zeroed := testGoPVERSAOAEPHSMDecapResult(ctxPtr)
		if code != testCBMPCERange {
			t.Fatalf("goPVERSAOAEPHSMDecap returned %d, want %d", code, testCBMPCERange)
		}
		if !zeroed {
			t.Fatalf("goPVERSAOAEPHSMDecap should leave outputs zeroed on oversized callback output")
		}
	})

	t.Run("ecies p256 hsm ecdh", func(t *testing.T) {
		ctxPtr, err := registerPVEECIESP256HSMECDHCallbacks(&PVEECIESP256HSMECDHCallbacks{
			ECDH: func(dkHandle, kemCT []byte) ([]byte, error) { return oversized, nil },
		})
		if err != nil {
			t.Fatalf("registerPVEECIESP256HSMECDHCallbacks: %v", err)
		}
		defer unregisterPVEECIESP256HSMECDHCallbacks(ctxPtr)

		code, zeroed := testGoPVEECIESP256HSMECDHResult(ctxPtr)
		if code != testCBMPCERange {
			t.Fatalf("goPVEECIESP256HSMECDH returned %d, want %d", code, testCBMPCERange)
		}
		if !zeroed {
			t.Fatalf("goPVEECIESP256HSMECDH should leave outputs zeroed on oversized callback output")
		}
	})

	t.Run("kem encap oversized ct", func(t *testing.T) {
		ctxPtr, err := registerPVEBaseKEMCallbacks(&PVEBaseKEMCallbacks{
			Encap: func(ek, rho32 []byte) ([]byte, []byte, error) { return oversized, nil, nil },
			Decap: func(dk, kemCT []byte) ([]byte, error) { return nil, nil },
		})
		if err != nil {
			t.Fatalf("registerPVEBaseKEMCallbacks: %v", err)
		}
		defer unregisterPVEBaseKEMCallbacks(ctxPtr)

		code, zeroCT, zeroSS := testGoPVEKEMEncapResult(ctxPtr)
		if code != testCBMPCERange {
			t.Fatalf("goPVEKEMEncap returned %d, want %d", code, testCBMPCERange)
		}
		if !zeroCT || !zeroSS {
			t.Fatalf("goPVEKEMEncap should leave both outputs zeroed on oversized ct output")
		}
	})

	t.Run("kem encap oversized ss", func(t *testing.T) {
		ctxPtr, err := registerPVEBaseKEMCallbacks(&PVEBaseKEMCallbacks{
			Encap: func(ek, rho32 []byte) ([]byte, []byte, error) { return []byte{0x01}, oversized, nil },
			Decap: func(dk, kemCT []byte) ([]byte, error) { return nil, nil },
		})
		if err != nil {
			t.Fatalf("registerPVEBaseKEMCallbacks: %v", err)
		}
		defer unregisterPVEBaseKEMCallbacks(ctxPtr)

		code, zeroCT, zeroSS := testGoPVEKEMEncapResult(ctxPtr)
		if code != testCBMPCERange {
			t.Fatalf("goPVEKEMEncap returned %d, want %d", code, testCBMPCERange)
		}
		if !zeroCT || !zeroSS {
			t.Fatalf("goPVEKEMEncap should free and zero both outputs on oversized ss output")
		}
	})

	t.Run("kem decap", func(t *testing.T) {
		ctxPtr, err := registerPVEBaseKEMCallbacks(&PVEBaseKEMCallbacks{
			Encap: func(ek, rho32 []byte) ([]byte, []byte, error) { return nil, nil, nil },
			Decap: func(dk, kemCT []byte) ([]byte, error) { return oversized, nil },
		})
		if err != nil {
			t.Fatalf("registerPVEBaseKEMCallbacks: %v", err)
		}
		defer unregisterPVEBaseKEMCallbacks(ctxPtr)

		code, zeroed := testGoPVEKEMDecapResult(ctxPtr)
		if code != testCBMPCERange {
			t.Fatalf("goPVEKEMDecap returned %d, want %d", code, testCBMPCERange)
		}
		if !zeroed {
			t.Fatalf("goPVEKEMDecap should leave outputs zeroed on oversized callback output")
		}
	})
}

func TestMPHelpers_RejectInvalidSigReceiver(t *testing.T) {
	job := JobMP{
		Self:       0,
		PartyNames: hardeningPartyNames(),
		Transport:  validationTransport{},
	}
	access := validThresholdAccess()
	ctx := context.Background()

	tests := []struct {
		name string
		call func() error
	}{
		{
			name: "ecdsa additive",
			call: func() error {
				_, err := ECDSAMPSignAdditive(ctx, job, nil, nil, -1)
				return err
			},
		},
		{
			name: "ecdsa ac",
			call: func() error {
				_, err := ECDSAMPSignAC(ctx, job, nil, access, nil, 3)
				return err
			},
		},
		{
			name: "eddsa additive",
			call: func() error {
				_, err := EdDSAMPSignAdditive(ctx, job, nil, nil, -1)
				return err
			},
		},
		{
			name: "eddsa ac",
			call: func() error {
				_, err := EdDSAMPSignAC(ctx, job, nil, access, nil, 3)
				return err
			},
		},
		{
			name: "schnorr additive",
			call: func() error {
				_, err := SchnorrMPSignAdditive(ctx, job, nil, nil, -1)
				return err
			},
		},
		{
			name: "schnorr ac",
			call: func() error {
				_, err := SchnorrMPSignAC(ctx, job, nil, access, nil, 3)
				return err
			},
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			err := tc.call()
			if err == nil {
				t.Fatalf("expected invalid sigReceiver error")
			}
			if !strings.Contains(err.Error(), "sigReceiver") {
				t.Fatalf("expected sigReceiver validation error, got %v", err)
			}
		})
	}
}

func TestMPACHelpers_RejectInvalidQuorumPartyNames(t *testing.T) {
	job := JobMP{
		Self:       0,
		PartyNames: hardeningPartyNames(),
		Transport:  validationTransport{},
	}
	access := validThresholdAccess()
	ctx := context.Background()

	tests := []struct {
		name string
		call func() error
	}{
		{
			name: "ecdsa dkg ac",
			call: func() error {
				_, _, err := ECDSAMPDKGAC(ctx, job, CurveSecp256k1, nil, access, []string{"missing"})
				return err
			},
		},
		{
			name: "ecdsa refresh ac",
			call: func() error {
				_, _, err := ECDSAMPRefreshAC(ctx, job, nil, nil, access, []string{"missing"})
				return err
			},
		},
		{
			name: "eddsa dkg ac",
			call: func() error {
				_, _, err := EdDSAMPDKGAC(ctx, job, CurveEd25519, nil, access, []string{"missing"})
				return err
			},
		},
		{
			name: "eddsa refresh ac",
			call: func() error {
				_, _, err := EdDSAMPRefreshAC(ctx, job, nil, nil, access, []string{"missing"})
				return err
			},
		},
		{
			name: "schnorr dkg ac",
			call: func() error {
				_, _, err := SchnorrMPDKGAC(ctx, job, CurveSecp256k1, nil, access, []string{"missing"})
				return err
			},
		},
		{
			name: "schnorr refresh ac",
			call: func() error {
				_, _, err := SchnorrMPRefreshAC(ctx, job, nil, nil, access, []string{"missing"})
				return err
			},
		},
		{
			name: "tdh2 dkg ac",
			call: func() error {
				_, _, _, _, err := TDH2DKGAC(ctx, job, CurveP256, nil, access, []string{"missing"})
				return err
			},
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			err := tc.call()
			if err == nil {
				t.Fatalf("expected invalid quorumPartyNames error")
			}
			if !strings.Contains(err.Error(), "quorumPartyNames") {
				t.Fatalf("expected quorumPartyNames validation error, got %v", err)
			}
		})
	}
}

func TestTDH2CombineAC_RejectsMismatchedInputLengths(t *testing.T) {
	access := validThresholdAccess()
	partyNames := hardeningPartyNames()

	if _, err := TDH2CombineAC(access, nil, []string{partyNames[0]}, nil, nil, nil, nil, nil); err == nil {
		t.Fatalf("expected error for mismatched partyNames/publicShares lengths")
	} else if !strings.Contains(err.Error(), "partyNames") {
		t.Fatalf("expected partyNames/publicShares validation error, got %v", err)
	}

	if _, err := TDH2CombineAC(access, nil, []string{partyNames[0]}, [][]byte{nil}, nil, []string{partyNames[0]}, nil, nil); err == nil {
		t.Fatalf("expected error for mismatched partialPartyNames/partialDecryptions lengths")
	} else if !strings.Contains(err.Error(), "partialPartyNames") {
		t.Fatalf("expected partialPartyNames/partialDecryptions validation error, got %v", err)
	}
}
