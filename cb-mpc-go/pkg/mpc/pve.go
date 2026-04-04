package mpc

import cinterop "github.com/coinbase/cb-mpc-go/internal/cgo"

// PVEBasePKECallbacks provides encrypt/decrypt hooks for the base public-key encryption scheme used by PVE.
type PVEBasePKECallbacks = cinterop.PVEBasePKECallbacks

// PVERSAOAEPHSMDecapCallbacks provides an HSM-backed RSA-OAEP decapsulation hook for PVE decryption.
type PVERSAOAEPHSMDecapCallbacks = cinterop.PVERSAOAEPHSMDecapCallbacks

// PVEECIESP256HSMECDHCallbacks provides an HSM-backed ECIES-P256 ECDH hook for PVE decryption.
type PVEECIESP256HSMECDHCallbacks = cinterop.PVEECIESP256HSMECDHCallbacks

// PVEBaseKEMCallbacks provides encapsulate/decapsulate hooks for a KEM-based PVE scheme.
type PVEBaseKEMCallbacks = cinterop.PVEBaseKEMCallbacks

// PVEGenerateBasePKERSAKeypair generates an RSA encryption/decryption keypair
// for use with PVE, returning the encryption key followed by the decryption
// key.
func PVEGenerateBasePKERSAKeypair() (ek []byte, dk []byte, err error) {
	ek, dk, err = cinterop.PVEGenerateBasePKERSAKeypair()
	return wrapResult2("PVEGenerateBasePKERSAKeypair", ek, dk, err)
}

// PVEGenerateBasePKEECIESP256Keypair generates an ECIES-P256
// encryption/decryption keypair for use with PVE, returning the encryption key
// followed by the decryption key.
func PVEGenerateBasePKEECIESP256Keypair() (ek []byte, dk []byte, err error) {
	ek, dk, err = cinterop.PVEGenerateBasePKEECIESP256Keypair()
	return wrapResult2("PVEGenerateBasePKEECIESP256Keypair", ek, dk, err)
}

// PVEBasePKEECIESP256EKFromOCT constructs an ECIES-P256 encryption key from a raw octet-string public key.
func PVEBasePKEECIESP256EKFromOCT(pubKeyOct []byte) ([]byte, error) {
	ek, err := cinterop.PVEBasePKEECIESP256EKFromOCT(pubKeyOct)
	return wrapResult1("PVEBasePKEECIESP256EKFromOCT", ek, err)
}

// PVEBasePKERSAEKFromModulus constructs an RSA encryption key from a raw modulus.
func PVEBasePKERSAEKFromModulus(modulus []byte) ([]byte, error) {
	ek, err := cinterop.PVEBasePKERSAEKFromModulus(modulus)
	return wrapResult1("PVEBasePKERSAEKFromModulus", ek, err)
}

// PVEEncrypt performs publicly-verifiable encryption of scalar x on the given curve.
func PVEEncrypt(basePKE *PVEBasePKECallbacks, curve Curve, ek []byte, label []byte, x []byte) ([]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEEncrypt", err)
	}
	ciphertext, err := cinterop.PVEEncrypt(basePKE, cgoCurve, ek, label, x)
	return wrapResult1("PVEEncrypt", ciphertext, err)
}

// PVEVerify verifies a PVE ciphertext against a known public point and label.
func PVEVerify(basePKE *PVEBasePKECallbacks, curve Curve, ek []byte, ciphertext []byte, qCompressed []byte, label []byte) error {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return wrapPublicError("PVEVerify", err)
	}
	err = cinterop.PVEVerify(basePKE, cgoCurve, ek, ciphertext, qCompressed, label)
	return wrapPublicError("PVEVerify", err)
}

// PVEDecrypt decrypts a PVE ciphertext using the decryption key, returning the plaintext scalar.
func PVEDecrypt(basePKE *PVEBasePKECallbacks, curve Curve, dk []byte, ek []byte, ciphertext []byte, label []byte) ([]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEDecrypt", err)
	}
	plaintext, err := cinterop.PVEDecrypt(basePKE, cgoCurve, dk, ek, ciphertext, label)
	return wrapResult1("PVEDecrypt", plaintext, err)
}

// PVEDecryptRSAOAEPHSM decrypts a PVE ciphertext using an HSM-backed RSA-OAEP key.
func PVEDecryptRSAOAEPHSM(curve Curve, dkHandle []byte, ek []byte, ciphertext []byte, label []byte, callbacks *PVERSAOAEPHSMDecapCallbacks) ([]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEDecryptRSAOAEPHSM", err)
	}
	plaintext, err := cinterop.PVEDecryptRSAOAEPHSM(cgoCurve, dkHandle, ek, ciphertext, label, callbacks)
	return wrapResult1("PVEDecryptRSAOAEPHSM", plaintext, err)
}

// PVEDecryptECIESP256HSM decrypts a PVE ciphertext using an HSM-backed ECIES-P256 key.
func PVEDecryptECIESP256HSM(curve Curve, dkHandle []byte, ek []byte, ciphertext []byte, label []byte, callbacks *PVEECIESP256HSMECDHCallbacks) ([]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEDecryptECIESP256HSM", err)
	}
	plaintext, err := cinterop.PVEDecryptECIESP256HSM(cgoCurve, dkHandle, ek, ciphertext, label, callbacks)
	return wrapResult1("PVEDecryptECIESP256HSM", plaintext, err)
}

// PVEEncryptWithKEM performs PVE encryption using a KEM-based scheme instead of base PKE.
func PVEEncryptWithKEM(curve Curve, kemCallbacks *PVEBaseKEMCallbacks, ek []byte, label []byte, x []byte) ([]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEEncryptWithKEM", err)
	}
	ciphertext, err := cinterop.PVEEncryptWithKEM(cgoCurve, kemCallbacks, ek, label, x)
	return wrapResult1("PVEEncryptWithKEM", ciphertext, err)
}

// PVEVerifyWithKEM verifies a KEM-based PVE ciphertext against a known public point and label.
func PVEVerifyWithKEM(curve Curve, kemCallbacks *PVEBaseKEMCallbacks, ek []byte, ciphertext []byte, qCompressed []byte, label []byte) error {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return wrapPublicError("PVEVerifyWithKEM", err)
	}
	err = cinterop.PVEVerifyWithKEM(cgoCurve, kemCallbacks, ek, ciphertext, qCompressed, label)
	return wrapPublicError("PVEVerifyWithKEM", err)
}

// PVEDecryptWithKEM decrypts a KEM-based PVE ciphertext, returning the plaintext scalar.
func PVEDecryptWithKEM(curve Curve, kemCallbacks *PVEBaseKEMCallbacks, dk []byte, ek []byte, ciphertext []byte, label []byte) ([]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEDecryptWithKEM", err)
	}
	plaintext, err := cinterop.PVEDecryptWithKEM(cgoCurve, kemCallbacks, dk, ek, ciphertext, label)
	return wrapResult1("PVEDecryptWithKEM", plaintext, err)
}

// PVEGetQ extracts the compressed public point Q from a PVE ciphertext.
func PVEGetQ(ciphertext []byte) ([]byte, error) {
	qCompressed, err := cinterop.PVEGetQ(ciphertext)
	return wrapResult1("PVEGetQ", qCompressed, err)
}

// PVEGetLabel extracts the label from a PVE ciphertext.
func PVEGetLabel(ciphertext []byte) ([]byte, error) {
	label, err := cinterop.PVEGetLabel(ciphertext)
	return wrapResult1("PVEGetLabel", label, err)
}

// PVEBatchEncrypt encrypts multiple scalars in a single PVE batch for one recipient.
func PVEBatchEncrypt(basePKE *PVEBasePKECallbacks, curve Curve, ek []byte, label []byte, xs [][]byte) ([]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEBatchEncrypt", err)
	}
	ciphertext, err := cinterop.PVEBatchEncrypt(basePKE, cgoCurve, ek, label, xs)
	return wrapResult1("PVEBatchEncrypt", ciphertext, err)
}

// PVEBatchVerify verifies a PVE batch ciphertext against known public points and a label.
func PVEBatchVerify(basePKE *PVEBasePKECallbacks, curve Curve, ek []byte, ciphertext []byte, qsCompressed [][]byte, label []byte) error {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return wrapPublicError("PVEBatchVerify", err)
	}
	err = cinterop.PVEBatchVerify(basePKE, cgoCurve, ek, ciphertext, qsCompressed, label)
	return wrapPublicError("PVEBatchVerify", err)
}

// PVEBatchDecrypt decrypts a PVE batch ciphertext, returning all plaintext scalars.
func PVEBatchDecrypt(basePKE *PVEBasePKECallbacks, curve Curve, dk []byte, ek []byte, ciphertext []byte, label []byte) ([][]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEBatchDecrypt", err)
	}
	plaintextRows, err := cinterop.PVEBatchDecrypt(basePKE, cgoCurve, dk, ek, ciphertext, label)
	return wrapResult1("PVEBatchDecrypt", plaintextRows, err)
}

// PVEBatchDecryptRSAOAEPHSM decrypts a PVE batch ciphertext using an HSM-backed RSA-OAEP key.
func PVEBatchDecryptRSAOAEPHSM(curve Curve, dkHandle []byte, ek []byte, ciphertext []byte, label []byte, callbacks *PVERSAOAEPHSMDecapCallbacks) ([][]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEBatchDecryptRSAOAEPHSM", err)
	}
	plaintextRows, err := cinterop.PVEBatchDecryptRSAOAEPHSM(cgoCurve, dkHandle, ek, ciphertext, label, callbacks)
	return wrapResult1("PVEBatchDecryptRSAOAEPHSM", plaintextRows, err)
}

// PVEBatchDecryptECIESP256HSM decrypts a PVE batch ciphertext using an HSM-backed ECIES-P256 key.
func PVEBatchDecryptECIESP256HSM(curve Curve, dkHandle []byte, ek []byte, ciphertext []byte, label []byte, callbacks *PVEECIESP256HSMECDHCallbacks) ([][]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEBatchDecryptECIESP256HSM", err)
	}
	plaintextRows, err := cinterop.PVEBatchDecryptECIESP256HSM(cgoCurve, dkHandle, ek, ciphertext, label, callbacks)
	return wrapResult1("PVEBatchDecryptECIESP256HSM", plaintextRows, err)
}

// PVEBatchGetCount returns the number of encrypted scalars in a PVE batch ciphertext.
func PVEBatchGetCount(ciphertext []byte) (int, error) {
	count, err := cinterop.PVEBatchGetCount(ciphertext)
	return wrapResult1("PVEBatchGetCount", count, err)
}

// PVEBatchGetQs extracts the compressed public points from a PVE batch ciphertext.
func PVEBatchGetQs(ciphertext []byte) ([][]byte, error) {
	qsCompressed, err := cinterop.PVEBatchGetQs(ciphertext)
	return wrapResult1("PVEBatchGetQs", qsCompressed, err)
}

// PVEBatchGetLabel extracts the label from a PVE batch ciphertext.
func PVEBatchGetLabel(ciphertext []byte) ([]byte, error) {
	label, err := cinterop.PVEBatchGetLabel(ciphertext)
	return wrapResult1("PVEBatchGetLabel", label, err)
}

// PVEACEncrypt encrypts multiple scalars under an access-structure PVE scheme with per-leaf encryption keys.
func PVEACEncrypt(basePKE *PVEBasePKECallbacks, curve Curve, access *AccessStructure, leafNames []string, leafEKs [][]byte, label []byte, xs [][]byte) ([]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEACEncrypt", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, wrapPublicError("PVEACEncrypt", err)
	}
	ciphertext, err := cinterop.PVEACEncrypt(basePKE, cgoCurve, cgoAccess, leafNames, leafEKs, label, xs)
	return wrapResult1("PVEACEncrypt", ciphertext, err)
}

// PVEACVerify verifies an access-structure PVE ciphertext against known public points and a label.
func PVEACVerify(basePKE *PVEBasePKECallbacks, curve Curve, access *AccessStructure, leafNames []string, leafEKs [][]byte, ciphertext []byte, qsCompressed [][]byte, label []byte) error {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return wrapPublicError("PVEACVerify", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return wrapPublicError("PVEACVerify", err)
	}
	err = cinterop.PVEACVerify(basePKE, cgoCurve, cgoAccess, leafNames, leafEKs, ciphertext, qsCompressed, label)
	return wrapPublicError("PVEACVerify", err)
}

// PVEACPartyDecryptRow decrypts a single row of an AC PVE ciphertext using a
// leaf party's decryption key.
//
// This is the Go wrapper for the stepwise C API call
// `cbmpc_pve_ac_partial_decrypt_attempt`.
func PVEACPartyDecryptRow(basePKE *PVEBasePKECallbacks, curve Curve, access *AccessStructure, ciphertext []byte, rowIndex int, leafName string, dk []byte, label []byte) ([]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEACPartyDecryptRow", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, wrapPublicError("PVEACPartyDecryptRow", err)
	}
	share, err := cinterop.PVEACPartyDecryptRow(basePKE, cgoCurve, cgoAccess, ciphertext, rowIndex, leafName, dk, label)
	return wrapResult1("PVEACPartyDecryptRow", share, err)
}

// PVEACPartyDecryptRowRSAOAEPHSM decrypts a single AC PVE row using an HSM-backed RSA-OAEP key.
func PVEACPartyDecryptRowRSAOAEPHSM(curve Curve, access *AccessStructure, ciphertext []byte, rowIndex int, leafName string, dkHandle []byte, ek []byte, label []byte, callbacks *PVERSAOAEPHSMDecapCallbacks) ([]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEACPartyDecryptRowRSAOAEPHSM", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, wrapPublicError("PVEACPartyDecryptRowRSAOAEPHSM", err)
	}
	share, err := cinterop.PVEACPartyDecryptRowRSAOAEPHSM(cgoCurve, cgoAccess, ciphertext, rowIndex, leafName, dkHandle, ek, label, callbacks)
	return wrapResult1("PVEACPartyDecryptRowRSAOAEPHSM", share, err)
}

// PVEACPartyDecryptRowECIESP256HSM decrypts a single AC PVE row using an HSM-backed ECIES-P256 key.
func PVEACPartyDecryptRowECIESP256HSM(curve Curve, access *AccessStructure, ciphertext []byte, rowIndex int, leafName string, dkHandle []byte, ek []byte, label []byte, callbacks *PVEECIESP256HSMECDHCallbacks) ([]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEACPartyDecryptRowECIESP256HSM", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, wrapPublicError("PVEACPartyDecryptRowECIESP256HSM", err)
	}
	share, err := cinterop.PVEACPartyDecryptRowECIESP256HSM(cgoCurve, cgoAccess, ciphertext, rowIndex, leafName, dkHandle, ek, label, callbacks)
	return wrapResult1("PVEACPartyDecryptRowECIESP256HSM", share, err)
}

// PVEACAggregateToRestoreRow combines per-party decryption shares from a
// qualified quorum to restore a single row's plaintext.
//
// This is the Go wrapper for the stepwise C API call `cbmpc_pve_ac_combine`.
func PVEACAggregateToRestoreRow(basePKE *PVEBasePKECallbacks, curve Curve, access *AccessStructure, quorumLeafNames []string, quorumShares [][]byte, ciphertext []byte, rowIndex int, label []byte) ([][]byte, error) {
	cgoCurve, err := toCGOCurve(curve)
	if err != nil {
		return nil, wrapPublicError("PVEACAggregateToRestoreRow", err)
	}
	cgoAccess, err := toCGOAccessStructure(access)
	if err != nil {
		return nil, wrapPublicError("PVEACAggregateToRestoreRow", err)
	}
	plaintextRows, err := cinterop.PVEACAggregateToRestoreRow(basePKE, cgoCurve, cgoAccess, quorumLeafNames, quorumShares, ciphertext, rowIndex, label)
	return wrapResult1("PVEACAggregateToRestoreRow", plaintextRows, err)
}

// PVEACGetCount returns the number of encrypted rows in an AC PVE ciphertext.
func PVEACGetCount(ciphertext []byte) (int, error) {
	count, err := cinterop.PVEACGetCount(ciphertext)
	return wrapResult1("PVEACGetCount", count, err)
}

// PVEACGetQs extracts the compressed public points from an AC PVE ciphertext.
func PVEACGetQs(ciphertext []byte) ([][]byte, error) {
	qsCompressed, err := cinterop.PVEACGetQs(ciphertext)
	return wrapResult1("PVEACGetQs", qsCompressed, err)
}
