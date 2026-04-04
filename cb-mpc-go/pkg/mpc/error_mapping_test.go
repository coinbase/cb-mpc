package mpc

import (
	"context"
	"errors"
	"testing"

	cinterop "github.com/coinbase/cb-mpc-go/internal/cgo"
	"github.com/coinbase/cb-mpc-go/pkg/mpctest"
)

func TestWrapPublicError_InvalidCurveIsInvalidParameter(t *testing.T) {
	sessions := mpctest.NewMockNetwork(2)
	jobs := test2PJobs("errors/invalid-curve", sessions)
	t.Cleanup(func() {
		for _, session := range sessions {
			_ = session.Close()
		}
	})

	_, err := ECDSA2PDKG(context.Background(), jobs[0], Curve(99))
	if err == nil {
		t.Fatal("expected error for unsupported curve")
	}
	if !errors.Is(err, ErrInvalidParameter) {
		t.Fatalf("expected ErrInvalidParameter, got %v", err)
	}
	var wrapped *Error
	if !errors.As(err, &wrapped) {
		t.Fatalf("expected public Error wrapper, got %T", err)
	}
	if wrapped.Op != "ECDSA2PDKG" {
		t.Fatalf("unexpected Error.Op: got %q want %q", wrapped.Op, "ECDSA2PDKG")
	}
}

func TestWrapPublicError_MapsCGOStatusCodes(t *testing.T) {
	tests := []struct {
		name     string
		code     cinterop.ErrorCode
		expected error
	}{
		{
			name:     "network failure",
			code:     cinterop.ErrorCodeNetworkGeneral,
			expected: ErrNetworkFailure,
		},
		{
			name:     "invalid data",
			code:     cinterop.ErrorCodeFormat,
			expected: ErrInvalidData,
		},
		{
			name:     "protocol failure",
			code:     cinterop.ErrorCodeCrypto,
			expected: ErrProtocolFailure,
		},
	}

	for _, tc := range tests {
		t.Run(tc.name, func(t *testing.T) {
			err := wrapPublicError("demoOp", &cinterop.StatusError{
				Op:      "cbmpc_demo",
				Code:    tc.code,
				Message: "demo failure",
			})
			if err == nil {
				t.Fatal("expected mapped error")
			}
			if !errors.Is(err, tc.expected) {
				t.Fatalf("expected errors.Is(..., %v) for %v", tc.expected, err)
			}
			var wrapped *Error
			if !errors.As(err, &wrapped) {
				t.Fatalf("expected public Error wrapper, got %T", err)
			}
			if wrapped.Op != "demoOp" {
				t.Fatalf("unexpected Error.Op: got %q want %q", wrapped.Op, "demoOp")
			}
			var statusErr *cinterop.StatusError
			if !errors.As(err, &statusErr) {
				t.Fatalf("expected original StatusError to be preserved, got %v", err)
			}
		})
	}
}

func TestWrapPublicError_MapsKeyShareOperationsSeparately(t *testing.T) {
	err := wrapPublicError("EdDSAMPAttachPrivateScalar", &cinterop.StatusError{
		Op:      "cbmpc_eddsa_mp_attach_private_scalar",
		Code:    cinterop.ErrorCodeFormat,
		Message: "invalid format",
	})
	if err == nil {
		t.Fatal("expected mapped error")
	}
	if !errors.Is(err, ErrInvalidKeyShare) {
		t.Fatalf("expected ErrInvalidKeyShare, got %v", err)
	}
	if errors.Is(err, ErrInvalidData) {
		t.Fatalf("did not expect generic ErrInvalidData for key-share-specific error: %v", err)
	}
}

func TestWrapPublicError_MapsTransportClosed(t *testing.T) {
	err := wrapPublicError("demoOp", &cinterop.TransportError{
		Status: &cinterop.StatusError{
			Op:      "cbmpc_demo",
			Code:    cinterop.ErrorCodeNetworkGeneral,
			Message: "network error",
		},
		Cause: cinterop.ErrTransportClosed,
	})
	if err == nil {
		t.Fatal("expected mapped error")
	}
	if !errors.Is(err, ErrTransportClosed) {
		t.Fatalf("expected ErrTransportClosed, got %v", err)
	}
	if errors.Is(err, ErrNetworkFailure) {
		t.Fatalf("did not expect ErrNetworkFailure to win over ErrTransportClosed: %v", err)
	}
}
