package cgobinding

/*
#cgo CFLAGS: -Werror
#include "eckeymp.h"
*/
import "C"

import "fmt"

type Mpc_eckey_mp_ref C.mpc_eckey_mp_ref

// SerializeKeyShare converts an mpc_eckey_mp_ref into a slice of byte buffers
// that fully represent the secret-share. The data is suitable for short-term
// transport or caching. It should NOT be relied upon for long-term storage
// across library versions.
func SerializeKeyShare(key Mpc_eckey_mp_ref) ([][]byte, error) {
	var ser CMEMS
	err := C.serialize_mpc_eckey_mp((*C.mpc_eckey_mp_ref)(&key), &ser)
	if err != 0 {
		return nil, fmt.Errorf("serialize_mpc_eckey_mp failed: %v", err)
	}
	return CMEMSGet(ser), nil
}

// DeserializeKeyShare allocates a fresh key-share object from the byte buffers
// produced by SerializeKeyShare and returns a reference to it.
func DeserializeKeyShare(ser [][]byte) (Mpc_eckey_mp_ref, error) {
	var key Mpc_eckey_mp_ref
	err := C.deserialize_mpc_eckey_mp(cmems(ser), (*C.mpc_eckey_mp_ref)(&key))
	if err != 0 {
		return Mpc_eckey_mp_ref{}, fmt.Errorf("deserialize_mpc_eckey_mp failed: %v", err)
	}
	return key, nil
}

// -----------------------------------------------------------------------------
// Backwards-compatibility thin wrappers (deprecated)
// -----------------------------------------------------------------------------

// SerializeECDSAShare is kept for historical reasons. New code should migrate
// to SerializeKeyShare.
func SerializeECDSAShare(key Mpc_eckey_mp_ref) ([][]byte, error) { return SerializeKeyShare(key) }

// DeserializeECDSAShare is kept for historical reasons. New code should migrate
// to DeserializeKeyShare.
func DeserializeECDSAShare(ser [][]byte) (Mpc_eckey_mp_ref, error) {
	return DeserializeKeyShare(ser)
}

// KeyShareDKG performs distributed key generation and returns a key-share
// reference. It is algorithm agnostic – it only depends on the underlying
// Schnorr-style key-share representation.
func KeyShareDKG(job JobMP, curveRef ECurveRef) (Mpc_eckey_mp_ref, error) {
	var key Mpc_eckey_mp_ref
	cErr := C.mpc_eckey_mp_dkg(job.GetCJob(), (*C.ecurve_ref)(&curveRef), (*C.mpc_eckey_mp_ref)(&key))
	if cErr != 0 {
		return key, fmt.Errorf("key-share DKG failed, %v", cErr)
	}
	return key, nil
}

// KeyShareDKGCode is a convenience wrapper that takes a curve code instead of a native curve ref.
func KeyShareDKGCode(job JobMP, curveCode int) (Mpc_eckey_mp_ref, error) {
	ref, err := ECurveFind(curveCode)
	if err != nil {
		return Mpc_eckey_mp_ref{}, err
	}
	return KeyShareDKG(job, ref)
}

// KeyShareRefresh rerandomises the secret shares while keeping the aggregated
// public key unchanged.
func KeyShareRefresh(job JobMP, sid []byte, key Mpc_eckey_mp_ref) (Mpc_eckey_mp_ref, error) {
	if sid == nil {
		sid = make([]byte, 0)
	}
	var newKey Mpc_eckey_mp_ref
	cErr := C.mpc_eckey_mp_refresh(job.GetCJob(), cmem(sid), (*C.mpc_eckey_mp_ref)(&key), (*C.mpc_eckey_mp_ref)(&newKey))
	if cErr != 0 {
		return newKey, fmt.Errorf("key-share refresh failed, %v", cErr)
	}
	return newKey, nil
}

// ThresholdDKG runs a threshold Distributed Key Generation for Schnorr-style
// keys (used by both ECDSA-MPC and EdDSA-MPC). It returns a fresh key share
// owned by the calling party.
func ThresholdDKG(job JobMP, curveRef ECurveRef, sid []byte, ac C_AcPtr, roleIndices []int) (Mpc_eckey_mp_ref, error) {
	if sid == nil {
		sid = make([]byte, 0)
	}

	quorum := NewPartySet()
	defer quorum.Free()
	for _, idx := range roleIndices {
		quorum.Add(idx)
	}

	var key Mpc_eckey_mp_ref
	cErr := C.eckey_dkg_mp_threshold_dkg(
		job.GetCJob(),
		(*C.ecurve_ref)(&curveRef),
		cmem(sid),
		(*C.crypto_ss_ac_ref)(&ac),
		(*C.mpc_party_set_ref)(&quorum),
		(*C.mpc_eckey_mp_ref)(&key))
	if cErr != 0 {
		return key, fmt.Errorf("threshold DKG failed, %v", cErr)
	}
	return key, nil
}

// ThresholdDKGCode mirrors ThresholdDKG but accepts a curve code.
func ThresholdDKGCode(job JobMP, curveCode int, sid []byte, ac C_AcPtr, roleIndices []int) (Mpc_eckey_mp_ref, error) {
	ref, err := ECurveFind(curveCode)
	if err != nil {
		return Mpc_eckey_mp_ref{}, err
	}
	return ThresholdDKG(job, ref, sid, ac, roleIndices)
}

// Back-compat synonym.
func KeyShareThresholdDKG(job JobMP, curveRef ECurveRef, sid []byte, ac C_AcPtr, roleIndices []int) (Mpc_eckey_mp_ref, error) {
	return ThresholdDKG(job, curveRef, sid, ac, roleIndices)
}

// ToAdditiveShare converts a multiplicative share into an additive one under the given access structure and quorum names.
func (key *Mpc_eckey_mp_ref) ToAdditiveShare(ac C_AcPtr, quorumPartyNames []string) (Mpc_eckey_mp_ref, error) {
	var additiveKey Mpc_eckey_mp_ref

	nameBytes := make([][]byte, len(quorumPartyNames))
	for i, name := range quorumPartyNames {
		nameBytes[i] = []byte(name)
	}

	cErr := C.eckey_key_share_mp_to_additive_share(
		(*C.mpc_eckey_mp_ref)(key),
		(*C.crypto_ss_ac_ref)(&ac),
		cmems(nameBytes),
		(*C.mpc_eckey_mp_ref)(&additiveKey))
	if cErr != 0 {
		return additiveKey, fmt.Errorf("to_additive_share failed, %v", cErr)
	}
	return additiveKey, nil
}

// -----------------------------------------------------------------------------
// Accessors (shared between ECDSA-MPC and EdDSA-MPC)
// -----------------------------------------------------------------------------

// KeySharePartyName returns the party identifier associated with the key share.
func KeySharePartyName(key Mpc_eckey_mp_ref) (string, error) {
	var nameMem CMEM
	err := C.mpc_eckey_mp_get_party_name((*C.mpc_eckey_mp_ref)(&key), &nameMem)
	if err != 0 {
		return "", fmt.Errorf("getting party name failed, %v", err)
	}
	return string(CMEMGet(nameMem)), nil
}

// KeyShareXShare returns the secret scalar held by this party.
func KeyShareXShare(key Mpc_eckey_mp_ref) ([]byte, error) {
	var xShareMem CMEM
	err := C.mpc_eckey_mp_get_x_share((*C.mpc_eckey_mp_ref)(&key), &xShareMem)
	if err != 0 {
		return nil, fmt.Errorf("getting x_share failed, %v", err)
	}
	return CMEMGet(xShareMem), nil
}

// KeyShareQ returns a reference to the aggregated public key point Q.
func KeyShareQ(key Mpc_eckey_mp_ref) (ECCPointRef, error) {
	cPoint := C.mpc_eckey_mp_get_Q((*C.mpc_eckey_mp_ref)(&key))
	if cPoint.opaque == nil {
		return ECCPointRef{}, fmt.Errorf("failed to retrieve Q from key")
	}
	return ECCPointRef(cPoint), nil
}

// KeyShareCurve returns the curve associated with the key share.
func KeyShareCurve(key Mpc_eckey_mp_ref) (ECurveRef, error) {
	cRef := C.mpc_eckey_mp_get_curve((*C.mpc_eckey_mp_ref)(&key))
	if cRef.opaque == nil {
		return ECurveRef{}, fmt.Errorf("failed to get curve from key")
	}
	return ECurveRef(cRef), nil
}

// KeyShareQis returns per-party public key shares.
func KeyShareQis(key Mpc_eckey_mp_ref) ([][]byte, [][]byte, error) {
	var nameMems CMEMS
	var pointMems CMEMS
	cErr := C.mpc_eckey_mp_get_Qis((*C.mpc_eckey_mp_ref)(&key), &nameMems, &pointMems)
	if cErr != 0 {
		return nil, nil, fmt.Errorf("getting Qis failed, %v", cErr)
	}
	names := CMEMSGet(nameMems)
	points := CMEMSGet(pointMems)
	if len(names) != len(points) {
		return nil, nil, fmt.Errorf("inconsistent Qis arrays: %d names vs %d points", len(names), len(points))
	}
	return names, points, nil
}

// KeyShareCurveCode returns the numeric curve code associated with the key share.
func KeyShareCurveCode(key Mpc_eckey_mp_ref) (int, error) {
	ref, err := KeyShareCurve(key)
	if err != nil {
		return 0, err
	}
	return ECurveGetCurveCode(ref), nil
}

// KeyShareQBytes returns the Q point as bytes.
func KeyShareQBytes(key Mpc_eckey_mp_ref) ([]byte, error) {
	ref, err := KeyShareQ(key)
	if err != nil {
		return nil, err
	}
	bytes := ECCPointToBytes(ref)
	(&ref).Free()
	return bytes, nil
}

// Free releases the underlying native key-share object.
func (k *Mpc_eckey_mp_ref) Free() {
	C.free_mpc_eckey_mp(C.mpc_eckey_mp_ref(*k))
}

// -----------------------------------------------------------------------------
// Back-compat wrappers with legacy names
// -----------------------------------------------------------------------------

func MPC_mpc_eckey_mp_get_party_name(key Mpc_eckey_mp_ref) (string, error) {
	return KeySharePartyName(key)
}
func MPC_mpc_eckey_mp_get_x_share(key Mpc_eckey_mp_ref) ([]byte, error) {
	return KeyShareXShare(key)
}
func MPC_mpc_eckey_mp_Q(key Mpc_eckey_mp_ref) (ECCPointRef, error)   { return KeyShareQ(key) }
func MPC_mpc_eckey_mp_curve(key Mpc_eckey_mp_ref) (ECurveRef, error) { return KeyShareCurve(key) }
func MPC_mpc_eckey_mp_Qis(key Mpc_eckey_mp_ref) ([][]byte, [][]byte, error) {
	return KeyShareQis(key)
}
