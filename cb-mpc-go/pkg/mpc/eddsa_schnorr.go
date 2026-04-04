package mpc

import (
	"context"

	cinterop "github.com/coinbase/cb-mpc-go/internal/cgo"
)

// EdDSA2PDKG performs two-party distributed key generation for EdDSA on the given curve.
func EdDSA2PDKG(ctx context.Context, job Job2P, curve Curve) ([]byte, error) {
	cgoJob, err := toCGOJob2P(job)
	if err != nil {
		return nil, wrapPublicError("EdDSA2PDKG", err)
	}
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("EdDSA2PDKG", err)
	}
	keyBlob, err := cinterop.EdDSA2PDKG(ctx, cgoJob, cgoCurve)
	return wrapResult1("EdDSA2PDKG", keyBlob, err)
}

// EdDSA2PRefresh refreshes a two-party EdDSA key share without changing the joint public key.
func EdDSA2PRefresh(ctx context.Context, job Job2P, keyBlob []byte) ([]byte, error) {
	cgoJob, err := toCGOJob2P(job)
	if err != nil {
		return nil, wrapPublicError("EdDSA2PRefresh", err)
	}
	refreshedKeyBlob, err := cinterop.EdDSA2PRefresh(ctx, cgoJob, keyBlob)
	return wrapResult1("EdDSA2PRefresh", refreshedKeyBlob, err)
}

// EdDSA2PSign signs msg with EdDSA-2PC.
//
// The signature is produced only on P1. When job.Self is `TwoPartyP2`,
// signature may be empty on successful completion.
func EdDSA2PSign(ctx context.Context, job Job2P, keyBlob []byte, msg []byte) (signature []byte, err error) {
	cgoJob, err := toCGOJob2P(job)
	if err != nil {
		return nil, wrapPublicError("EdDSA2PSign", err)
	}
	signature, err = cinterop.EdDSA2PSign(ctx, cgoJob, keyBlob, msg)
	return wrapResult1("EdDSA2PSign", signature, err)
}

// EdDSA2PGetPublicKeyCompressed extracts the compressed joint public key from a two-party EdDSA key blob.
func EdDSA2PGetPublicKeyCompressed(keyBlob []byte) ([]byte, error) {
	publicKey, err := cinterop.EdDSA2PGetPublicKeyCompressed(keyBlob)
	return wrapResult1("EdDSA2PGetPublicKeyCompressed", publicKey, err)
}

// EdDSA2PGetPublicShareCompressed extracts this party's compressed public share from a two-party EdDSA key blob.
func EdDSA2PGetPublicShareCompressed(keyBlob []byte) ([]byte, error) {
	publicShare, err := cinterop.EdDSA2PGetPublicShareCompressed(keyBlob)
	return wrapResult1("EdDSA2PGetPublicShareCompressed", publicShare, err)
}

// EdDSA2PDetachPrivateScalar splits a two-party EdDSA key blob into a
// public-only blob and a fixed-length private scalar.
func EdDSA2PDetachPrivateScalar(keyBlob []byte) (publicKeyBlob []byte, privateScalarFixed []byte, err error) {
	publicKeyBlob, privateScalarFixed, err = cinterop.EdDSA2PDetachPrivateScalar(keyBlob)
	return wrapResult2("EdDSA2PDetachPrivateScalar", publicKeyBlob, privateScalarFixed, err)
}

// EdDSA2PAttachPrivateScalar re-assembles a two-party EdDSA key blob from a public blob, private scalar, and public share.
func EdDSA2PAttachPrivateScalar(publicKeyBlob []byte, privateScalarFixed []byte, publicShareCompressed []byte) ([]byte, error) {
	keyBlob, err := cinterop.EdDSA2PAttachPrivateScalar(publicKeyBlob, privateScalarFixed, publicShareCompressed)
	return wrapResult1("EdDSA2PAttachPrivateScalar", keyBlob, err)
}

// EdDSAMPDKGAdditive performs multi-party distributed key generation for EdDSA
// with additive shares, returning the key blob and output session ID.
func EdDSAMPDKGAdditive(ctx context.Context, job JobMP, curve Curve) (keyBlob []byte, sidOut []byte, err error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, nil, wrapPublicError("EdDSAMPDKGAdditive", err)
	}
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, nil, wrapPublicError("EdDSAMPDKGAdditive", err)
	}
	keyBlob, sidOut, err = cinterop.EdDSAMPDKGAdditive(ctx, cgoJob, cgoCurve)
	return wrapResult2("EdDSAMPDKGAdditive", keyBlob, sidOut, err)
}

// EdDSAMPDKGAC performs multi-party EdDSA DKG governed by an access-structure
// quorum policy, returning the AC key blob and output session ID.
func EdDSAMPDKGAC(ctx context.Context, job JobMP, curve Curve, sidIn []byte, access *AccessStructure, quorumPartyNames []string) (acKeyBlob []byte, sidOut []byte, err error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, nil, wrapPublicError("EdDSAMPDKGAC", err)
	}
	if err := validatePartyNameSubset("quorumPartyNames", job.PartyNames, quorumPartyNames); err != nil {
		return nil, nil, wrapPublicError("EdDSAMPDKGAC", err)
	}
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, nil, wrapPublicError("EdDSAMPDKGAC", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, nil, wrapPublicError("EdDSAMPDKGAC", err)
	}
	acKeyBlob, sidOut, err = cinterop.EdDSAMPDKGAC(ctx, cgoJob, cgoCurve, sidIn, cgoAccess, quorumPartyNames)
	return wrapResult2("EdDSAMPDKGAC", acKeyBlob, sidOut, err)
}

// EdDSAMPRefreshAdditive refreshes multi-party EdDSA additive key shares
// without changing the joint public key, returning the new session ID and
// refreshed key blob.
func EdDSAMPRefreshAdditive(ctx context.Context, job JobMP, sidIn []byte, keyBlob []byte) (sidOut []byte, refreshedKeyBlob []byte, err error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, nil, wrapPublicError("EdDSAMPRefreshAdditive", err)
	}
	sidOut, refreshedKeyBlob, err = cinterop.EdDSAMPRefreshAdditive(ctx, cgoJob, sidIn, keyBlob)
	return wrapResult2("EdDSAMPRefreshAdditive", sidOut, refreshedKeyBlob, err)
}

// EdDSAMPRefreshAC refreshes multi-party EdDSA key shares under an
// access-structure quorum policy, returning the new session ID and refreshed AC
// key blob.
func EdDSAMPRefreshAC(ctx context.Context, job JobMP, sidIn []byte, acKeyBlob []byte, access *AccessStructure, quorumPartyNames []string) (sidOut []byte, refreshedKeyBlob []byte, err error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, nil, wrapPublicError("EdDSAMPRefreshAC", err)
	}
	if err := validatePartyNameSubset("quorumPartyNames", job.PartyNames, quorumPartyNames); err != nil {
		return nil, nil, wrapPublicError("EdDSAMPRefreshAC", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, nil, wrapPublicError("EdDSAMPRefreshAC", err)
	}
	sidOut, refreshedKeyBlob, err = cinterop.EdDSAMPRefreshAC(ctx, cgoJob, sidIn, acKeyBlob, cgoAccess, quorumPartyNames)
	return wrapResult2("EdDSAMPRefreshAC", sidOut, refreshedKeyBlob, err)
}

// EdDSAMPSignAdditive produces a multi-party EdDSA signature with additive shares; only sigReceiver gets the result.
//
// sigReceiver is interpreted as an index into job.PartyNames.
func EdDSAMPSignAdditive(ctx context.Context, job JobMP, keyBlob []byte, msg []byte, sigReceiver int) ([]byte, error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, wrapPublicError("EdDSAMPSignAdditive", err)
	}
	signature, err := cinterop.EdDSAMPSignAdditive(ctx, cgoJob, keyBlob, msg, sigReceiver)
	return wrapResult1("EdDSAMPSignAdditive", signature, err)
}

// EdDSAMPSignAC produces a multi-party EdDSA signature under an access-structure quorum policy.
//
// sigReceiver is interpreted as an index into job.PartyNames.
func EdDSAMPSignAC(ctx context.Context, job JobMP, acKeyBlob []byte, access *AccessStructure, msg []byte, sigReceiver int) ([]byte, error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, wrapPublicError("EdDSAMPSignAC", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, wrapPublicError("EdDSAMPSignAC", err)
	}
	signature, err := cinterop.EdDSAMPSignAC(ctx, cgoJob, acKeyBlob, cgoAccess, msg, sigReceiver)
	return wrapResult1("EdDSAMPSignAC", signature, err)
}

// EdDSAMPGetPublicKeyCompressed extracts the compressed joint public key from a multi-party EdDSA key blob.
func EdDSAMPGetPublicKeyCompressed(keyBlob []byte) ([]byte, error) {
	publicKey, err := cinterop.EdDSAMPGetPublicKeyCompressed(keyBlob)
	return wrapResult1("EdDSAMPGetPublicKeyCompressed", publicKey, err)
}

// EdDSAMPGetPublicShareCompressed extracts this party's compressed public share from a multi-party EdDSA key blob.
func EdDSAMPGetPublicShareCompressed(keyBlob []byte) ([]byte, error) {
	publicShare, err := cinterop.EdDSAMPGetPublicShareCompressed(keyBlob)
	return wrapResult1("EdDSAMPGetPublicShareCompressed", publicShare, err)
}

// EdDSAMPDetachPrivateScalar splits a multi-party EdDSA key blob into a
// public-only blob and a fixed-length private scalar.
func EdDSAMPDetachPrivateScalar(keyBlob []byte) (publicKeyBlob []byte, privateScalarFixed []byte, err error) {
	publicKeyBlob, privateScalarFixed, err = cinterop.EdDSAMPDetachPrivateScalar(keyBlob)
	return wrapResult2("EdDSAMPDetachPrivateScalar", publicKeyBlob, privateScalarFixed, err)
}

// EdDSAMPAttachPrivateScalar re-assembles a multi-party EdDSA key blob from a public blob, private scalar, and public share.
func EdDSAMPAttachPrivateScalar(publicKeyBlob []byte, privateScalarFixed []byte, publicShareCompressed []byte) ([]byte, error) {
	keyBlob, err := cinterop.EdDSAMPAttachPrivateScalar(publicKeyBlob, privateScalarFixed, publicShareCompressed)
	return wrapResult1("EdDSAMPAttachPrivateScalar", keyBlob, err)
}

// Schnorr2PDKG performs two-party distributed key generation for Schnorr signatures on the given curve.
func Schnorr2PDKG(ctx context.Context, job Job2P, curve Curve) ([]byte, error) {
	cgoJob, err := toCGOJob2P(job)
	if err != nil {
		return nil, wrapPublicError("Schnorr2PDKG", err)
	}
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("Schnorr2PDKG", err)
	}
	keyBlob, err := cinterop.Schnorr2PDKG(ctx, cgoJob, cgoCurve)
	return wrapResult1("Schnorr2PDKG", keyBlob, err)
}

// Schnorr2PRefresh refreshes a two-party Schnorr key share without changing the joint public key.
func Schnorr2PRefresh(ctx context.Context, job Job2P, keyBlob []byte) ([]byte, error) {
	cgoJob, err := toCGOJob2P(job)
	if err != nil {
		return nil, wrapPublicError("Schnorr2PRefresh", err)
	}
	refreshedKeyBlob, err := cinterop.Schnorr2PRefresh(ctx, cgoJob, keyBlob)
	return wrapResult1("Schnorr2PRefresh", refreshedKeyBlob, err)
}

// Schnorr2PSign signs msg with Schnorr-2PC.
//
// The signature is produced only on P1. When job.Self is `TwoPartyP2`,
// signature may be empty on successful completion.
func Schnorr2PSign(ctx context.Context, job Job2P, keyBlob []byte, msg []byte) (signature []byte, err error) {
	cgoJob, err := toCGOJob2P(job)
	if err != nil {
		return nil, wrapPublicError("Schnorr2PSign", err)
	}
	signature, err = cinterop.Schnorr2PSign(ctx, cgoJob, keyBlob, msg)
	return wrapResult1("Schnorr2PSign", signature, err)
}

// Schnorr2PGetPublicKeyCompressed extracts the compressed joint public key from a two-party Schnorr key blob.
func Schnorr2PGetPublicKeyCompressed(keyBlob []byte) ([]byte, error) {
	publicKey, err := cinterop.Schnorr2PGetPublicKeyCompressed(keyBlob)
	return wrapResult1("Schnorr2PGetPublicKeyCompressed", publicKey, err)
}

// Schnorr2PExtractPublicKeyXOnly extracts the x-only public key from a two-party Schnorr key blob.
func Schnorr2PExtractPublicKeyXOnly(keyBlob []byte) ([]byte, error) {
	publicKey, err := cinterop.Schnorr2PExtractPublicKeyXOnly(keyBlob)
	return wrapResult1("Schnorr2PExtractPublicKeyXOnly", publicKey, err)
}

// Schnorr2PGetPublicShareCompressed extracts this party's compressed public share from a two-party Schnorr key blob.
func Schnorr2PGetPublicShareCompressed(keyBlob []byte) ([]byte, error) {
	publicShare, err := cinterop.Schnorr2PGetPublicShareCompressed(keyBlob)
	return wrapResult1("Schnorr2PGetPublicShareCompressed", publicShare, err)
}

// Schnorr2PDetachPrivateScalar splits a two-party Schnorr key blob into a
// public-only blob and a fixed-length private scalar.
func Schnorr2PDetachPrivateScalar(keyBlob []byte) (publicKeyBlob []byte, privateScalarFixed []byte, err error) {
	publicKeyBlob, privateScalarFixed, err = cinterop.Schnorr2PDetachPrivateScalar(keyBlob)
	return wrapResult2("Schnorr2PDetachPrivateScalar", publicKeyBlob, privateScalarFixed, err)
}

// Schnorr2PAttachPrivateScalar re-assembles a two-party Schnorr key blob from a public blob, private scalar, and public share.
func Schnorr2PAttachPrivateScalar(publicKeyBlob []byte, privateScalarFixed []byte, publicShareCompressed []byte) ([]byte, error) {
	keyBlob, err := cinterop.Schnorr2PAttachPrivateScalar(publicKeyBlob, privateScalarFixed, publicShareCompressed)
	return wrapResult1("Schnorr2PAttachPrivateScalar", keyBlob, err)
}

// SchnorrMPDKGAdditive performs multi-party distributed key generation for
// Schnorr with additive shares, returning the key blob and output session ID.
func SchnorrMPDKGAdditive(ctx context.Context, job JobMP, curve Curve) (keyBlob []byte, sidOut []byte, err error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, nil, wrapPublicError("SchnorrMPDKGAdditive", err)
	}
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, nil, wrapPublicError("SchnorrMPDKGAdditive", err)
	}
	keyBlob, sidOut, err = cinterop.SchnorrMPDKGAdditive(ctx, cgoJob, cgoCurve)
	return wrapResult2("SchnorrMPDKGAdditive", keyBlob, sidOut, err)
}

// SchnorrMPDKGAC performs multi-party Schnorr DKG governed by an
// access-structure quorum policy, returning the AC key blob and output session
// ID.
func SchnorrMPDKGAC(ctx context.Context, job JobMP, curve Curve, sidIn []byte, access *AccessStructure, quorumPartyNames []string) (acKeyBlob []byte, sidOut []byte, err error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, nil, wrapPublicError("SchnorrMPDKGAC", err)
	}
	if err := validatePartyNameSubset("quorumPartyNames", job.PartyNames, quorumPartyNames); err != nil {
		return nil, nil, wrapPublicError("SchnorrMPDKGAC", err)
	}
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, nil, wrapPublicError("SchnorrMPDKGAC", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, nil, wrapPublicError("SchnorrMPDKGAC", err)
	}
	acKeyBlob, sidOut, err = cinterop.SchnorrMPDKGAC(ctx, cgoJob, cgoCurve, sidIn, cgoAccess, quorumPartyNames)
	return wrapResult2("SchnorrMPDKGAC", acKeyBlob, sidOut, err)
}

// SchnorrMPRefreshAdditive refreshes multi-party Schnorr additive key shares
// without changing the joint public key, returning the new session ID and
// refreshed key blob.
func SchnorrMPRefreshAdditive(ctx context.Context, job JobMP, sidIn []byte, keyBlob []byte) (sidOut []byte, refreshedKeyBlob []byte, err error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, nil, wrapPublicError("SchnorrMPRefreshAdditive", err)
	}
	sidOut, refreshedKeyBlob, err = cinterop.SchnorrMPRefreshAdditive(ctx, cgoJob, sidIn, keyBlob)
	return wrapResult2("SchnorrMPRefreshAdditive", sidOut, refreshedKeyBlob, err)
}

// SchnorrMPRefreshAC refreshes multi-party Schnorr key shares under an
// access-structure quorum policy, returning the new session ID and refreshed AC
// key blob.
func SchnorrMPRefreshAC(ctx context.Context, job JobMP, sidIn []byte, acKeyBlob []byte, access *AccessStructure, quorumPartyNames []string) (sidOut []byte, refreshedKeyBlob []byte, err error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, nil, wrapPublicError("SchnorrMPRefreshAC", err)
	}
	if err := validatePartyNameSubset("quorumPartyNames", job.PartyNames, quorumPartyNames); err != nil {
		return nil, nil, wrapPublicError("SchnorrMPRefreshAC", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, nil, wrapPublicError("SchnorrMPRefreshAC", err)
	}
	sidOut, refreshedKeyBlob, err = cinterop.SchnorrMPRefreshAC(ctx, cgoJob, sidIn, acKeyBlob, cgoAccess, quorumPartyNames)
	return wrapResult2("SchnorrMPRefreshAC", sidOut, refreshedKeyBlob, err)
}

// SchnorrMPSignAdditive produces a multi-party Schnorr signature with additive shares; only sigReceiver gets the result.
//
// sigReceiver is interpreted as an index into job.PartyNames.
func SchnorrMPSignAdditive(ctx context.Context, job JobMP, keyBlob []byte, msg []byte, sigReceiver int) ([]byte, error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, wrapPublicError("SchnorrMPSignAdditive", err)
	}
	signature, err := cinterop.SchnorrMPSignAdditive(ctx, cgoJob, keyBlob, msg, sigReceiver)
	return wrapResult1("SchnorrMPSignAdditive", signature, err)
}

// SchnorrMPSignAC produces a multi-party Schnorr signature under an access-structure quorum policy.
//
// sigReceiver is interpreted as an index into job.PartyNames.
func SchnorrMPSignAC(ctx context.Context, job JobMP, acKeyBlob []byte, access *AccessStructure, msg []byte, sigReceiver int) ([]byte, error) {
	cgoJob, err := toCGOJobMP(job)
	if err != nil {
		return nil, wrapPublicError("SchnorrMPSignAC", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, wrapPublicError("SchnorrMPSignAC", err)
	}
	signature, err := cinterop.SchnorrMPSignAC(ctx, cgoJob, acKeyBlob, cgoAccess, msg, sigReceiver)
	return wrapResult1("SchnorrMPSignAC", signature, err)
}

// SchnorrMPGetPublicKeyCompressed extracts the compressed joint public key from a multi-party Schnorr key blob.
func SchnorrMPGetPublicKeyCompressed(keyBlob []byte) ([]byte, error) {
	publicKey, err := cinterop.SchnorrMPGetPublicKeyCompressed(keyBlob)
	return wrapResult1("SchnorrMPGetPublicKeyCompressed", publicKey, err)
}

// SchnorrMPExtractPublicKeyXOnly extracts the x-only public key from a multi-party Schnorr key blob.
func SchnorrMPExtractPublicKeyXOnly(keyBlob []byte) ([]byte, error) {
	publicKey, err := cinterop.SchnorrMPExtractPublicKeyXOnly(keyBlob)
	return wrapResult1("SchnorrMPExtractPublicKeyXOnly", publicKey, err)
}

// SchnorrMPGetPublicShareCompressed extracts this party's compressed public share from a multi-party Schnorr key blob.
func SchnorrMPGetPublicShareCompressed(keyBlob []byte) ([]byte, error) {
	publicShare, err := cinterop.SchnorrMPGetPublicShareCompressed(keyBlob)
	return wrapResult1("SchnorrMPGetPublicShareCompressed", publicShare, err)
}

// SchnorrMPDetachPrivateScalar splits a multi-party Schnorr key blob into a
// public-only blob and a fixed-length private scalar.
func SchnorrMPDetachPrivateScalar(keyBlob []byte) (publicKeyBlob []byte, privateScalarFixed []byte, err error) {
	publicKeyBlob, privateScalarFixed, err = cinterop.SchnorrMPDetachPrivateScalar(keyBlob)
	return wrapResult2("SchnorrMPDetachPrivateScalar", publicKeyBlob, privateScalarFixed, err)
}

// SchnorrMPAttachPrivateScalar re-assembles a multi-party Schnorr key blob from a public blob, private scalar, and public share.
func SchnorrMPAttachPrivateScalar(publicKeyBlob []byte, privateScalarFixed []byte, publicShareCompressed []byte) ([]byte, error) {
	keyBlob, err := cinterop.SchnorrMPAttachPrivateScalar(publicKeyBlob, privateScalarFixed, publicShareCompressed)
	return wrapResult1("SchnorrMPAttachPrivateScalar", keyBlob, err)
}
