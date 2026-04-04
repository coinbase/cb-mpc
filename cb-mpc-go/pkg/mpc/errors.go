package mpc

import (
	"errors"
	"fmt"

	cinterop "github.com/coinbase/cb-mpc-go/internal/cgo"
)

var (
	// ErrInvalidParameter indicates an invalid parameter was provided
	ErrInvalidParameter = errors.New("mpc: invalid parameter")

	// ErrInvalidData indicates malformed or unsupported serialized MPC data.
	ErrInvalidData = errors.New("mpc: invalid data")

	// ErrNetworkFailure indicates a network communication error
	ErrNetworkFailure = errors.New("mpc: network failure")

	// ErrProtocolFailure indicates the MPC protocol failed
	ErrProtocolFailure = errors.New("mpc: protocol failure")

	// ErrInvalidSignature is reserved for caller-side signature verification
	// helpers. The current public wrapper APIs do not return it automatically.
	ErrInvalidSignature = errors.New("mpc: invalid signature")

	// ErrInvalidKeyShare indicates the key share is invalid or corrupted
	ErrInvalidKeyShare = errors.New("mpc: invalid key share")

	// ErrTransportClosed indicates the transport has been closed.
	ErrTransportClosed = errors.New("mpc: transport closed")

	// ErrSessionClosed is kept as an alias for backward recognition of the
	// previous transport abstraction name.
	ErrSessionClosed = ErrTransportClosed
)

// Error wraps an underlying error with context
type Error struct {
	Op  string // Operation that failed
	Err error  // Underlying error
}

func (e *Error) Error() string {
	return fmt.Sprintf("mpc.%s: %v", e.Op, e.Err)
}

func (e *Error) Unwrap() error {
	return e.Err
}

type categorizedError struct {
	category error
	detail   error
}

func (e *categorizedError) Error() string {
	if e.category == nil {
		if e.detail == nil {
			return ""
		}
		return e.detail.Error()
	}
	if e.detail == nil {
		return e.category.Error()
	}
	return fmt.Sprintf("%s: %v", e.category, e.detail)
}

func (e *categorizedError) Unwrap() []error {
	errs := make([]error, 0, 2)
	if e.category != nil {
		errs = append(errs, e.category)
	}
	if e.detail != nil {
		errs = append(errs, e.detail)
	}
	return errs
}

func categorizeError(category error, detail error) error {
	if detail == nil {
		return nil
	}
	return &categorizedError{
		category: category,
		detail:   detail,
	}
}

func invalidParameterErrorf(format string, args ...interface{}) error {
	return categorizeError(ErrInvalidParameter, fmt.Errorf(format, args...))
}

func wrapPublicError(op string, err error) error {
	if err == nil {
		return nil
	}

	var existing *Error
	if errors.As(err, &existing) {
		return err
	}

	if category := categoryForError(op, err); category != nil && !errors.Is(err, category) {
		err = categorizeError(category, err)
	}

	return &Error{
		Op:  op,
		Err: err,
	}
}

func categoryForError(op string, err error) error {
	if errors.Is(err, cinterop.ErrTransportClosed) {
		return ErrTransportClosed
	}

	var statusErr *cinterop.StatusError
	if errors.As(err, &statusErr) {
		code := statusErr.Code
		if code == cinterop.ErrorCodeBadArgument || code == cinterop.ErrorCodeRange || code == cinterop.ErrorCodeNotSupported {
			return ErrInvalidParameter
		}
		if code == cinterop.ErrorCodeFormat || code == cinterop.ErrorCodeNotFound {
			if isKeyShareOperation(op) {
				return ErrInvalidKeyShare
			}
			return ErrInvalidData
		}
		if code == cinterop.ErrorCodeNetworkGeneral {
			return ErrNetworkFailure
		}
		if code == cinterop.ErrorCodeCrypto ||
			code == cinterop.ErrorCodeGeneral ||
			code == cinterop.ErrorCodeUninitialized ||
			code == cinterop.ErrorCodeInsufficient ||
			code == cinterop.ErrorCodeECDSA2PBitLeak {
			return ErrProtocolFailure
		}
	}

	switch {
	case errors.Is(err, ErrInvalidParameter):
		return ErrInvalidParameter
	case errors.Is(err, ErrInvalidData):
		return ErrInvalidData
	case errors.Is(err, ErrInvalidKeyShare):
		return ErrInvalidKeyShare
	case errors.Is(err, ErrNetworkFailure):
		return ErrNetworkFailure
	case errors.Is(err, ErrProtocolFailure):
		return ErrProtocolFailure
	case errors.Is(err, ErrInvalidSignature):
		return ErrInvalidSignature
	case errors.Is(err, ErrTransportClosed):
		return ErrTransportClosed
	default:
		return nil
	}
}

func isKeyShareOperation(op string) bool {
	switch op {
	case "ECDSA2PGetPublicKeyCompressed",
		"ECDSA2PGetPublicShareCompressed",
		"ECDSA2PDetachPrivateScalar",
		"ECDSA2PAttachPrivateScalar",
		"ECDSA2PRefresh",
		"ECDSA2PSign",
		"ECDSAMPGetPublicKeyCompressed",
		"ECDSAMPGetPublicShareCompressed",
		"ECDSAMPDetachPrivateScalar",
		"ECDSAMPAttachPrivateScalar",
		"ECDSAMPRefreshAdditive",
		"ECDSAMPRefreshAC",
		"ECDSAMPSignAdditive",
		"ECDSAMPSignAC",
		"EdDSA2PGetPublicKeyCompressed",
		"EdDSA2PGetPublicShareCompressed",
		"EdDSA2PDetachPrivateScalar",
		"EdDSA2PAttachPrivateScalar",
		"EdDSA2PRefresh",
		"EdDSA2PSign",
		"EdDSAMPGetPublicKeyCompressed",
		"EdDSAMPGetPublicShareCompressed",
		"EdDSAMPDetachPrivateScalar",
		"EdDSAMPAttachPrivateScalar",
		"EdDSAMPRefreshAdditive",
		"EdDSAMPRefreshAC",
		"EdDSAMPSignAdditive",
		"EdDSAMPSignAC",
		"Schnorr2PGetPublicKeyCompressed",
		"Schnorr2PExtractPublicKeyXOnly",
		"Schnorr2PGetPublicShareCompressed",
		"Schnorr2PDetachPrivateScalar",
		"Schnorr2PAttachPrivateScalar",
		"Schnorr2PRefresh",
		"Schnorr2PSign",
		"SchnorrMPGetPublicKeyCompressed",
		"SchnorrMPExtractPublicKeyXOnly",
		"SchnorrMPGetPublicShareCompressed",
		"SchnorrMPDetachPrivateScalar",
		"SchnorrMPAttachPrivateScalar",
		"SchnorrMPRefreshAdditive",
		"SchnorrMPRefreshAC",
		"SchnorrMPSignAdditive",
		"SchnorrMPSignAC",
		"TDH2PartialDecrypt":
		return true
	default:
		return false
	}
}

func wrapResult1[T any](op string, value T, err error) (T, error) {
	if err != nil {
		var zero T
		return zero, wrapPublicError(op, err)
	}
	return value, nil
}

func wrapResult2[T1, T2 any](op string, value1 T1, value2 T2, err error) (T1, T2, error) {
	if err != nil {
		var zero1 T1
		var zero2 T2
		return zero1, zero2, wrapPublicError(op, err)
	}
	return value1, value2, nil
}

func wrapResult4[T1, T2, T3, T4 any](op string, value1 T1, value2 T2, value3 T3, value4 T4, err error) (T1, T2, T3, T4, error) {
	if err != nil {
		var zero1 T1
		var zero2 T2
		var zero3 T3
		var zero4 T4
		return zero1, zero2, zero3, zero4, wrapPublicError(op, err)
	}
	return value1, value2, value3, value4, nil
}
