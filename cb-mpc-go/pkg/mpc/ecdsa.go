package mpc

import (
	"context"

	cinterop "github.com/coinbase/cb-mpc-go/internal/cgo"
)

// ECDSA2PDKG performs two-party distributed key generation for ECDSA on the given curve.
func ECDSA2PDKG(ctx context.Context, job Job2P, curve Curve) ([]byte, error) {
	cgoJob, err := toCGOJob2P(job)
	if err != nil {
		return nil, wrapPublicError("ECDSA2PDKG", err)
	}
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("ECDSA2PDKG", err)
	}
	keyBlob, err := cinterop.ECDSA2PDKG(ctx, cgoJob, cgoCurve)
	return wrapResult1("ECDSA2PDKG", keyBlob, err)
}

// ECDSA2PRefresh refreshes a two-party ECDSA key share without changing the joint public key.
func ECDSA2PRefresh(ctx context.Context, job Job2P, keyBlob []byte) ([]byte, error) {
	cgoJob, err := toCGOJob2P(job)
	if err != nil {
		return nil, wrapPublicError("ECDSA2PRefresh", err)
	}
	refreshedKeyBlob, err := cinterop.ECDSA2PRefresh(ctx, cgoJob, keyBlob)
	return wrapResult1("ECDSA2PRefresh", refreshedKeyBlob, err)
}

// ECDSA2PSign signs msgHash and returns the session ID used for the run plus a
// DER-encoded signature.
//
// The signature is produced only on P1. When job.Self is `TwoPartyP2`, sigDER
// may be empty on successful completion.
func ECDSA2PSign(ctx context.Context, job Job2P, keyBlob []byte, msgHash []byte, sidIn []byte) (sidOut []byte, sigDER []byte, err error) {
	cgoJob, err := toCGOJob2P(job)
	if err != nil {
		return nil, nil, wrapPublicError("ECDSA2PSign", err)
	}
	sidOut, sigDER, err = cinterop.ECDSA2PSign(ctx, cgoJob, keyBlob, msgHash, sidIn)
	return wrapResult2("ECDSA2PSign", sidOut, sigDER, err)
}

// ECDSA2PGetPublicKeyCompressed extracts the compressed joint public key from a two-party ECDSA key blob.
func ECDSA2PGetPublicKeyCompressed(keyBlob []byte) ([]byte, error) {
	publicKey, err := cinterop.ECDSA2PGetPublicKeyCompressed(keyBlob)
	return wrapResult1("ECDSA2PGetPublicKeyCompressed", publicKey, err)
}

// ECDSA2PGetPublicShareCompressed extracts this party's compressed public share from a two-party ECDSA key blob.
func ECDSA2PGetPublicShareCompressed(keyBlob []byte) ([]byte, error) {
	publicShare, err := cinterop.ECDSA2PGetPublicShareCompressed(keyBlob)
	return wrapResult1("ECDSA2PGetPublicShareCompressed", publicShare, err)
}

// ECDSA2PDetachPrivateScalar splits a two-party ECDSA key blob into a
// public-only blob and a raw private scalar.
func ECDSA2PDetachPrivateScalar(keyBlob []byte) (publicKeyBlob []byte, privateScalar []byte, err error) {
	publicKeyBlob, privateScalar, err = cinterop.ECDSA2PDetachPrivateScalar(keyBlob)
	return wrapResult2("ECDSA2PDetachPrivateScalar", publicKeyBlob, privateScalar, err)
}

// ECDSA2PAttachPrivateScalar re-assembles a two-party ECDSA key blob from a public blob, private scalar, and public share.
func ECDSA2PAttachPrivateScalar(publicKeyBlob []byte, privateScalar []byte, publicShareCompressed []byte) ([]byte, error) {
	keyBlob, err := cinterop.ECDSA2PAttachPrivateScalar(publicKeyBlob, privateScalar, publicShareCompressed)
	return wrapResult1("ECDSA2PAttachPrivateScalar", keyBlob, err)
}

// ECDSAMPDKGAdditive performs multi-party distributed key generation for ECDSA
// with additive shares, returning the key blob and output session ID.
func ECDSAMPDKGAdditive(ctx context.Context, job JobMP, curve Curve) (keyBlob []byte, sidOut []byte, err error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, nil, wrapPublicError("ECDSAMPDKGAdditive", err)
	}
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, nil, wrapPublicError("ECDSAMPDKGAdditive", err)
	}
	keyBlob, sidOut, err = cinterop.ECDSAMPDKGAdditive(ctx, cgoJob, cgoCurve)
	return wrapResult2("ECDSAMPDKGAdditive", keyBlob, sidOut, err)
}

// ECDSAMPDKGAC performs multi-party ECDSA DKG governed by an access-structure
// quorum policy, returning the AC key blob and output session ID.
func ECDSAMPDKGAC(ctx context.Context, job JobMP, curve Curve, sidIn []byte, access *AccessStructure, quorumPartyNames []string) (acKeyBlob []byte, sidOut []byte, err error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, nil, wrapPublicError("ECDSAMPDKGAC", err)
	}
	if err := validatePartyNameSubset("quorumPartyNames", job.PartyNames, quorumPartyNames); err != nil {
		return nil, nil, wrapPublicError("ECDSAMPDKGAC", err)
	}
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, nil, wrapPublicError("ECDSAMPDKGAC", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, nil, wrapPublicError("ECDSAMPDKGAC", err)
	}
	acKeyBlob, sidOut, err = cinterop.ECDSAMPDKGAC(ctx, cgoJob, cgoCurve, sidIn, cgoAccess, quorumPartyNames)
	return wrapResult2("ECDSAMPDKGAC", acKeyBlob, sidOut, err)
}

// ECDSAMPRefreshAdditive refreshes multi-party ECDSA additive key shares
// without changing the joint public key, returning the new session ID and
// refreshed key blob.
func ECDSAMPRefreshAdditive(ctx context.Context, job JobMP, sidIn []byte, keyBlob []byte) (sidOut []byte, refreshedKeyBlob []byte, err error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, nil, wrapPublicError("ECDSAMPRefreshAdditive", err)
	}
	sidOut, refreshedKeyBlob, err = cinterop.ECDSAMPRefreshAdditive(ctx, cgoJob, sidIn, keyBlob)
	return wrapResult2("ECDSAMPRefreshAdditive", sidOut, refreshedKeyBlob, err)
}

// ECDSAMPRefreshAC refreshes multi-party ECDSA key shares under an
// access-structure quorum policy, returning the new session ID and refreshed AC
// key blob.
func ECDSAMPRefreshAC(ctx context.Context, job JobMP, sidIn []byte, acKeyBlob []byte, access *AccessStructure, quorumPartyNames []string) (sidOut []byte, refreshedKeyBlob []byte, err error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, nil, wrapPublicError("ECDSAMPRefreshAC", err)
	}
	if err := validatePartyNameSubset("quorumPartyNames", job.PartyNames, quorumPartyNames); err != nil {
		return nil, nil, wrapPublicError("ECDSAMPRefreshAC", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, nil, wrapPublicError("ECDSAMPRefreshAC", err)
	}
	sidOut, refreshedKeyBlob, err = cinterop.ECDSAMPRefreshAC(ctx, cgoJob, sidIn, acKeyBlob, cgoAccess, quorumPartyNames)
	return wrapResult2("ECDSAMPRefreshAC", sidOut, refreshedKeyBlob, err)
}

// ECDSAMPSignAdditive produces a multi-party ECDSA signature with additive shares; only sigReceiver gets the result.
//
// sigReceiver is interpreted as an index into job.PartyNames.
func ECDSAMPSignAdditive(ctx context.Context, job JobMP, keyBlob []byte, msgHash []byte, sigReceiver int) ([]byte, error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, wrapPublicError("ECDSAMPSignAdditive", err)
	}
	sigDER, err := cinterop.ECDSAMPSignAdditive(ctx, cgoJob, keyBlob, msgHash, sigReceiver)
	return wrapResult1("ECDSAMPSignAdditive", sigDER, err)
}

// ECDSAMPSignAC produces a multi-party ECDSA signature under an access-structure quorum policy.
//
// sigReceiver is interpreted as an index into job.PartyNames.
func ECDSAMPSignAC(ctx context.Context, job JobMP, acKeyBlob []byte, access *AccessStructure, msgHash []byte, sigReceiver int) ([]byte, error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, wrapPublicError("ECDSAMPSignAC", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, wrapPublicError("ECDSAMPSignAC", err)
	}
	sigDER, err := cinterop.ECDSAMPSignAC(ctx, cgoJob, acKeyBlob, cgoAccess, msgHash, sigReceiver)
	return wrapResult1("ECDSAMPSignAC", sigDER, err)
}

// ECDSAMPGetPublicKeyCompressed extracts the compressed joint public key from a multi-party ECDSA key blob.
func ECDSAMPGetPublicKeyCompressed(keyBlob []byte) ([]byte, error) {
	publicKey, err := cinterop.ECDSAMPGetPublicKeyCompressed(keyBlob)
	return wrapResult1("ECDSAMPGetPublicKeyCompressed", publicKey, err)
}

// ECDSAMPGetPublicShareCompressed extracts this party's compressed public share from a multi-party ECDSA key blob.
func ECDSAMPGetPublicShareCompressed(keyBlob []byte) ([]byte, error) {
	publicShare, err := cinterop.ECDSAMPGetPublicShareCompressed(keyBlob)
	return wrapResult1("ECDSAMPGetPublicShareCompressed", publicShare, err)
}

// ECDSAMPDetachPrivateScalar splits a multi-party ECDSA key blob into a
// public-only blob and a raw private scalar.
func ECDSAMPDetachPrivateScalar(keyBlob []byte) (publicKeyBlob []byte, privateScalar []byte, err error) {
	publicKeyBlob, privateScalar, err = cinterop.ECDSAMPDetachPrivateScalar(keyBlob)
	return wrapResult2("ECDSAMPDetachPrivateScalar", publicKeyBlob, privateScalar, err)
}

// ECDSAMPAttachPrivateScalar re-assembles a multi-party ECDSA key blob from a public blob, private scalar, and public share.
func ECDSAMPAttachPrivateScalar(publicKeyBlob []byte, privateScalarFixed []byte, publicShareCompressed []byte) ([]byte, error) {
	keyBlob, err := cinterop.ECDSAMPAttachPrivateScalar(publicKeyBlob, privateScalarFixed, publicShareCompressed)
	return wrapResult1("ECDSAMPAttachPrivateScalar", keyBlob, err)
}
