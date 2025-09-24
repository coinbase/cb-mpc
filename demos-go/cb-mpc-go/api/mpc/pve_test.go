package mpc

import (
	"bytes"
	"crypto/ecdh"
	"crypto/rand"
	"crypto/rsa"
	"crypto/sha256"
	"crypto/x509"
	"encoding/binary"
	"fmt"
	"io"
	"sync"
	"testing"
	"unsafe"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/testutil"
	"github.com/stretchr/testify/require"
)

// Deterministic reader derived from a seed using SHA-256(counter || seed)
type ctrRand struct {
	seed    [32]byte
	counter uint64
	buf     []byte
	off     int
}

func newCTRRand(seed []byte) *ctrRand {
	var s [32]byte
	copy(s[:], seed)
	return &ctrRand{seed: s}
}

func (r *ctrRand) refill() {
	ctrBytes := make([]byte, 8)
	for i := 0; i < 8; i++ {
		ctrBytes[7-i] = byte(r.counter >> (8 * i))
	}
	h := sha256.Sum256(append(ctrBytes, r.seed[:]...))
	r.buf = h[:]
	r.off = 0
	r.counter++
}

func (r *ctrRand) Read(p []byte) (int, error) {
	n := 0
	for n < len(p) {
		if r.off >= len(r.buf) {
			r.refill()
		}
		m := copy(p[n:], r.buf[r.off:])
		r.off += m
		n += m
	}
	return n, nil
}

// TestPVEEncryptDecryptSingle performs an encrypt → verify → decrypt round-trip
// for the single-party PVE helpers.
func TestPVEEncryptDecryptSingle(t *testing.T) {
	// Create a fresh PVE handle bound to the dummy XOR scheme so that tests
	// run side-by-side without touching global state.
	xorKEM := newTestXorKEM()
	pve, err := NewPVE(Config{KEM: xorKEM})
	require.NoError(t, err)

	// Prepare curve (P-256).
	cv, err := curve.NewP256()
	require.NoError(t, err)
	defer cv.Free()

	// Generate base enc key pair.
	pub, prv, err := xorKEM.Generate()
	require.NoError(t, err)

	// Secret scalar x and its public share Q = x*G.
	x, err := cv.RandomScalar()
	require.NoError(t, err)

	Q, err := cv.MultiplyGenerator(x)
	require.NoError(t, err)

	// Encrypt.
	encResp, err := pve.Encrypt(&PVEEncryptRequest{
		PublicKey:    pub,
		PrivateValue: x,
		Curve:        cv,
		Label:        "pve-single-test",
	})
	require.NoError(t, err)
	require.Greater(t, len(encResp.Ciphertext), 0)

	_, _ = prv, Q

	// Verify authentic ciphertext.
	verResp, err := pve.Verify(&PVEVerifyRequest{
		PublicKey:   pub,
		Ciphertext:  encResp.Ciphertext,
		PublicShare: Q,
		Label:       "pve-single-test",
	})
	require.NoError(t, err)
	require.True(t, verResp.Valid)

	// Tamper with ciphertext and expect failure (silence C/C++ stderr during this expected error).
	tampered := make([]byte, len(encResp.Ciphertext))
	copy(tampered, encResp.Ciphertext)
	if len(tampered) > 0 {
		tampered[len(tampered)-1] ^= 0xFF
	}
	testutil.TSilence(t, func(t *testing.T) {
		_, err = pve.Verify(&PVEVerifyRequest{
			PublicKey:   pub,
			Ciphertext:  PVECiphertext(tampered),
			PublicShare: Q,
			Label:       "pve-single-test",
		})
	})
	require.Error(t, err)

	// Derive public key from private key and print it.
	_, err = xorKEM.DerivePub(prv)
	require.NoError(t, err)

	// Decrypt.
	decResp, err := pve.Decrypt(&PVEDecryptRequest{
		PrivateKey: prv,
		Ciphertext: encResp.Ciphertext,
		Curve:      cv,
		Label:      "pve-single-test",
	})
	_ = decResp
	require.NoError(t, err)
	require.NotNil(t, decResp.PrivateValue)
	require.Equal(t, x.Bytes, decResp.PrivateValue.Bytes)

	// Concurrency coverage for thread-safety
	t.Run("concurrent encrypt operations", func(t *testing.T) {
		const goroutines = 10
		var wg sync.WaitGroup
		errCh := make(chan error, goroutines)
		wg.Add(goroutines)
		for i := 0; i < goroutines; i++ {
			go func() {
				defer wg.Done()
				xi, err := cv.RandomScalar()
				if err != nil {
					errCh <- err
					return
				}
				_, err = pve.Encrypt(&PVEEncryptRequest{
					PublicKey:    pub,
					PrivateValue: xi,
					Curve:        cv,
					Label:        "concurrent-test",
				})
				errCh <- err
			}()
		}
		wg.Wait()
		close(errCh)
		for e := range errCh {
			require.NoError(t, e)
		}
	})

	t.Run("concurrent verify operations", func(t *testing.T) {
		const goroutines = 10
		var wg sync.WaitGroup
		errCh := make(chan error, goroutines)
		wg.Add(goroutines)
		for i := 0; i < goroutines; i++ {
			go func() {
				defer wg.Done()
				_, err := pve.Verify(&PVEVerifyRequest{
					PublicKey:   pub,
					Ciphertext:  encResp.Ciphertext,
					PublicShare: Q,
					Label:       "pve-single-test",
				})
				errCh <- err
			}()
		}
		wg.Wait()
		close(errCh)
		for e := range errCh {
			require.NoError(t, e)
		}
	})

	t.Run("concurrent decrypt operations", func(t *testing.T) {
		const goroutines = 10
		var wg sync.WaitGroup
		errCh := make(chan error, goroutines)
		wg.Add(goroutines)
		for i := 0; i < goroutines; i++ {
			go func() {
				defer wg.Done()
				dec, err := pve.Decrypt(&PVEDecryptRequest{
					PrivateKey: prv,
					Ciphertext: encResp.Ciphertext,
					Curve:      cv,
					Label:      "pve-single-test",
				})
				if err == nil && dec != nil && dec.PrivateValue != nil && !bytes.Equal(dec.PrivateValue.Bytes, x.Bytes) {
					err = fmt.Errorf("decrypted value mismatch")
				}
				errCh <- err
			}()
		}
		wg.Wait()
		close(errCh)
		for e := range errCh {
			require.NoError(t, e)
		}
	})
}

type testXorKEM struct{}

func newTestXorKEM() testXorKEM { return testXorKEM{} }

func (testXorKEM) Generate() ([]byte, []byte, error) {
	key := make([]byte, 1)
	rand.Read(key) // any single byte != 0 is fine
	return key, key, nil
}

func (testXorKEM) Encapsulate(ek []byte, rho [32]byte) ([]byte, []byte, error) {
	// Use rho directly as the shared secret for determinism.
	ss := make([]byte, 32)
	copy(ss, rho[:])
	ct := make([]byte, len(ss))
	for i := range ss {
		ct[i] = ss[i] ^ ek[0]
	}
	return ct, ss, nil
}

func (testXorKEM) Decapsulate(skHandle unsafe.Pointer, ct []byte) ([]byte, error) {
	var keyByte byte
	if skHandle != nil {
		// In this test, skHandle may point to a cmem_t with the bytes
		type cmem_t struct {
			data *byte
			size int32
		}
		cm := (*cmem_t)(skHandle)
		if cm != nil && cm.data != nil && cm.size > 0 {
			keyByte = *(*byte)(unsafe.Pointer(cm.data))
		} else {
			keyByte = byte(uintptr(skHandle) & 0xFF)
		}
	}
	out := make([]byte, len(ct))
	for i := range ct {
		out[i] = ct[i] ^ keyByte
	}
	return out, nil
}

func (testXorKEM) DerivePub(dk []byte) ([]byte, error) {
	// For this toy scheme, public key equals private key (XOR key byte).
	ek := make([]byte, len(dk))
	copy(ek, dk)
	return ek, nil
}

func TestPVERoundTrip(t *testing.T) {
	// Create a fresh PVE handle bound to the XOR backend.
	pve, err := NewPVE(Config{KEM: newTestXorKEM()})
	require.NoError(t, err)

	// 1) Create demo key-pair using the XOR backend.
	dk, ek, err := newTestXorKEM().Generate()
	if err != nil {
		t.Fatalf("generate: %v", err)
	}

	rho := []byte("demo‑rho 32 bytes pad pad pad pad!!")[:32]
	var rhoArr [32]byte
	copy(rhoArr[:], rho)

	// 2) Run through the bridges without touching any C++ code:
	ct, ss, err := pve.kem.Encapsulate(ek, rhoArr)
	if err != nil {
		t.Fatalf("encapsulate: %v", err)
	}
	// Wrap dk bytes into a temporary cmem_t and pass its address as the handle
	type cmem_t struct {
		data *byte
		size int32
	}
	var cm cmem_t
	if len(dk) > 0 {
		cm.data = (*byte)(unsafe.Pointer(&dk[0]))
		cm.size = int32(len(dk))
	}
	ss2, err := pve.kem.Decapsulate(unsafe.Pointer(&cm), ct)
	if err != nil {
		t.Fatalf("decapsulate: %v", err)
	}
	if !bytes.Equal(ss2, ss) {
		t.Fatal("round‑trip failed")
	}
}

// =============================================================================
// Additional back-ends used for coexistence / HSM style tests

type testShiftKEM struct{}

func (testShiftKEM) Generate() (skRef, ek []byte, err error) {
	b := make([]byte, 1)
	rand.Read(b)
	return b, b, nil // same byte represents both halves
}

func (testShiftKEM) Encapsulate(ek []byte, rho [32]byte) ([]byte, []byte, error) {
	ss := make([]byte, 32)
	copy(ss, rho[:])
	shift := ek[0] + 1
	ct := make([]byte, len(ss))
	for i := range ss {
		ct[i] = ss[i] ^ shift
	}
	return ct, ss, nil
}

func (testShiftKEM) Decapsulate(skHandle unsafe.Pointer, ct []byte) ([]byte, error) {
	// Prefer reading from cmem_t (byte-backed dk)
	type cmem_t struct {
		data *byte
		size int32
	}
	var keyByte byte
	cm := (*cmem_t)(skHandle)
	if cm != nil && cm.data != nil && cm.size > 0 {
		keyByte = *(*byte)(unsafe.Pointer(cm.data))
	} else {
		keyByte = byte(uintptr(skHandle) & 0xFF)
	}
	shift := keyByte + 1
	out := make([]byte, len(ct))
	for i := range ct {
		out[i] = ct[i] ^ shift
	}
	return out, nil
}

func (testShiftKEM) DerivePub(skRef []byte) ([]byte, error) {
	ek := make([]byte, len(skRef))
	copy(ek, skRef)
	return ek, nil
}

// hsmStubKEM imitates a hardware token: Generate returns a 32-bit handle.

type Handle = uint32

type hsmStubKEM struct{ store map[Handle]byte }

func newHSMStub() *hsmStubKEM { return &hsmStubKEM{store: make(map[Handle]byte)} }

func (h *hsmStubKEM) Generate() (skRef, ek []byte, err error) {
	// 4-byte little-endian handle
	handle := make([]byte, 4)
	rand.Read(handle)
	key := make([]byte, 1)
	rand.Read(key)
	h.store[Handle(binary.LittleEndian.Uint32(handle))] = key[0]
	return handle, key, nil
}

func (h *hsmStubKEM) Encapsulate(ek []byte, rho [32]byte) ([]byte, []byte, error) {
	ss := make([]byte, 32)
	copy(ss, rho[:])
	ct := make([]byte, len(ss))
	for i := range ss {
		ct[i] = ss[i] ^ ek[0]
	}
	return ct, ss, nil
}

func (h *hsmStubKEM) Decapsulate(skHandle unsafe.Pointer, ct []byte) ([]byte, error) {
	// Prefer cmem_t-backed handle; parse first 4 bytes little-endian.
	type cmem_t struct {
		data *byte
		size int32
	}
	var handle Handle
	cm := (*cmem_t)(skHandle)
	if cm != nil && cm.data != nil && cm.size > 0 {
		dk := unsafe.Slice((*byte)(unsafe.Pointer(cm.data)), int(cm.size))
		if len(dk) >= 4 {
			handle = Handle(binary.LittleEndian.Uint32(dk[:4]))
		} else {
			handle = Handle(dk[0])
		}
	} else {
		handle = Handle(uint32(uintptr(skHandle) & 0xffffffff))
	}
	keyByte, ok := h.store[handle]
	if !ok {
		return nil, fmt.Errorf("unknown handle")
	}
	out := make([]byte, len(ct))
	for i := range ct {
		out[i] = ct[i] ^ keyByte
	}
	return out, nil
}

func (h *hsmStubKEM) DerivePub(skRef []byte) ([]byte, error) {
	var handle Handle
	if len(skRef) >= 4 {
		handle = Handle(binary.LittleEndian.Uint32(skRef[:4]))
	} else if len(skRef) >= 1 {
		handle = Handle(skRef[0])
	} else {
		return nil, fmt.Errorf("invalid handle ref")
	}
	key, ok := h.store[handle]
	if !ok {
		return nil, fmt.Errorf("unknown handle %x", skRef)
	}
	return []byte{key}, nil
}

// Tests

func TestCoexistingBackends(t *testing.T) {
	xorPVE, err := NewPVE(Config{KEM: newTestXorKEM()})
	require.NoError(t, err)

	shiftPVE, err := NewPVE(Config{KEM: testShiftKEM{}})
	require.NoError(t, err)

	cv, _ := curve.NewP256()

	// Common inputs
	x, _ := cv.RandomScalar()
	Q, _ := cv.MultiplyGenerator(x)

	// XOR backend
	dkXor, ekXor, _ := newTestXorKEM().Generate()
	encXor, _ := xorPVE.Encrypt(&PVEEncryptRequest{PublicKey: ekXor, PrivateValue: x, Curve: cv, Label: "coexist"})
	decXor, _ := xorPVE.Decrypt(&PVEDecryptRequest{PrivateKey: dkXor, Ciphertext: encXor.Ciphertext, Curve: cv, Label: "coexist"})
	require.Equal(t, x.Bytes, decXor.PrivateValue.Bytes)

	// Shift backend
	skShift, ekShift, _ := testShiftKEM{}.Generate()
	encShift, _ := shiftPVE.Encrypt(&PVEEncryptRequest{PublicKey: ekShift, PrivateValue: x, Curve: cv, Label: "coexist"})
	decShift, _ := shiftPVE.Decrypt(&PVEDecryptRequest{PrivateKey: skShift, Ciphertext: encShift.Ciphertext, Curve: cv, Label: "coexist"})
	require.Equal(t, x.Bytes, decShift.PrivateValue.Bytes)

	// Final sanity: XOR ciphertext should not decrypt under Shift backend and vice-versa.
	testutil.TSilence(t, func(t *testing.T) {
		_, err = shiftPVE.Decrypt(&PVEDecryptRequest{PrivateKey: skShift, Ciphertext: encXor.Ciphertext, Curve: cv, Label: "coexist"})
	})
	require.Error(t, err)

	testutil.TSilence(t, func(t *testing.T) {
		_, err = xorPVE.Decrypt(&PVEDecryptRequest{PrivateKey: dkXor, Ciphertext: encShift.Ciphertext, Curve: cv, Label: "coexist"})
	})
	require.Error(t, err)
	_ = Q
}

func TestHSMStub(t *testing.T) {
	hsm := newHSMStub()
	pve, err := NewPVE(Config{KEM: hsm})
	require.NoError(t, err)

	cv, _ := curve.NewP256()
	x, _ := cv.RandomScalar()

	skRef, ek, err := hsm.Generate()
	require.NoError(t, err)

	encResp, err := pve.Encrypt(&PVEEncryptRequest{PublicKey: ek, PrivateValue: x, Curve: cv, Label: "hsm-demo"})
	require.NoError(t, err)

	decResp, err := pve.Decrypt(&PVEDecryptRequest{PrivateKey: skRef, Ciphertext: encResp.Ciphertext, Curve: cv, Label: "hsm-demo"})
	require.NoError(t, err)
	require.Equal(t, x.Bytes, decResp.PrivateValue.Bytes)

	SecureWipe(skRef)
}

// =============================
// Go-defined RSA KEM (toy)
// =============================

type rsaGoKEM struct {
	prv *rsa.PrivateKey
	pub *rsa.PublicKey
}

func newRSAGoKEM() (*rsaGoKEM, error) {
	k, err := rsa.GenerateKey(rand.Reader, 2048)
	if err != nil {
		return nil, err
	}
	return &rsaGoKEM{prv: k, pub: &k.PublicKey}, nil
}

func (r *rsaGoKEM) Generate() ([]byte, []byte, error) {
	prvBytes := x509.MarshalPKCS1PrivateKey(r.prv)
	pubBytes := x509.MarshalPKCS1PublicKey(r.pub)
	return prvBytes, pubBytes, nil
}

func (r *rsaGoKEM) Encapsulate(ek []byte, rho [32]byte) ([]byte, []byte, error) {
	pub, err := x509.ParsePKCS1PublicKey(ek)
	if err != nil {
		return nil, nil, err
	}
	label := []byte("pve-rsa-go")
	ctr := newCTRRand(rho[:])
	ct, err := rsa.EncryptOAEP(sha256.New(), ctr, pub, rho[:], label)
	if err != nil {
		return nil, nil, err
	}
	ss := make([]byte, 32)
	copy(ss, rho[:])
	return ct, ss, nil
}

func (r *rsaGoKEM) Decapsulate(skHandle unsafe.Pointer, ct []byte) ([]byte, error) {
	// For Go RSA KEM tests we still expect raw bytes; when called directly we pass a cmem_t.
	type cmem_t struct {
		data *byte
		size int32
	}
	cm := (*cmem_t)(skHandle)
	if cm == nil || cm.data == nil || cm.size <= 0 {
		return nil, fmt.Errorf("invalid sk handle")
	}
	dk := unsafe.Slice((*byte)(unsafe.Pointer(cm.data)), int(cm.size))
	prv, err := x509.ParsePKCS1PrivateKey(dk)
	if err != nil {
		return nil, err
	}
	label := []byte("pve-rsa-go")
	ss, err := rsa.DecryptOAEP(sha256.New(), rand.Reader, prv, ct, label)
	if err != nil {
		return nil, err
	}
	out := make([]byte, 32)
	copy(out, ss)
	return out, nil
}

func (r *rsaGoKEM) DerivePub(dk []byte) ([]byte, error) {
	prv, err := x509.ParsePKCS1PrivateKey(dk)
	if err != nil {
		return nil, err
	}
	return x509.MarshalPKCS1PublicKey(&prv.PublicKey), nil
}

// =============================
// Go-defined ECDH KEM (toy, P-256 + HKDF=truncate)
// =============================

type ecdhGoKEM struct{}

func newECDHGoKEM() *ecdhGoKEM { return &ecdhGoKEM{} }

func (e *ecdhGoKEM) Generate() ([]byte, []byte, error) {
	cv := ecdh.P256()
	priv, err := cv.GenerateKey(rand.Reader)
	if err != nil {
		return nil, nil, err
	}
	return priv.Bytes(), priv.PublicKey().Bytes(), nil
}

func (e *ecdhGoKEM) Encapsulate(ek []byte, rho [32]byte) ([]byte, []byte, error) {
	cv := ecdh.P256()
	peerPub, err := cv.NewPublicKey(ek)
	if err != nil {
		return nil, nil, err
	}
	// ephemeral key
	ephPriv, err := cv.GenerateKey(rand.Reader)
	if err != nil {
		return nil, nil, err
	}
	ss, err := ephPriv.ECDH(peerPub)
	if err != nil {
		return nil, nil, err
	}
	ct := ephPriv.PublicKey().Bytes()
	return ct, ss, nil
}

func (e *ecdhGoKEM) Decapsulate(skHandle unsafe.Pointer, ct []byte) ([]byte, error) {
	// Expect cmem_t pointing to private key bytes
	type cmem_t struct {
		data *byte
		size int32
	}
	cm := (*cmem_t)(skHandle)
	if cm == nil || cm.data == nil || cm.size <= 0 {
		return nil, fmt.Errorf("invalid sk handle")
	}
	dk := unsafe.Slice((*byte)(unsafe.Pointer(cm.data)), int(cm.size))
	cv := ecdh.P256()
	priv, err := cv.NewPrivateKey(dk)
	if err != nil {
		return nil, err
	}
	pub, err := cv.NewPublicKey(ct)
	if err != nil {
		return nil, err
	}
	ss, err := priv.ECDH(pub)
	if err != nil {
		return nil, err
	}
	return ss, nil
}

func (e *ecdhGoKEM) DerivePub(dk []byte) ([]byte, error) {
	cv := ecdh.P256()
	priv, err := cv.NewPrivateKey(dk)
	if err != nil {
		return nil, err
	}
	return priv.PublicKey().Bytes(), nil
}

// =============================
// RSA KEM with HSM-like handle simulation
// =============================

type rsaHSMKEM struct {
	store map[Handle]*rsa.PrivateKey
}

func newRSAHSMKEM() *rsaHSMKEM { return &rsaHSMKEM{store: make(map[Handle]*rsa.PrivateKey)} }

func (h *rsaHSMKEM) Generate() (skRef, ek []byte, err error) {
	k, err := rsa.GenerateKey(rand.Reader, 2048)
	if err != nil {
		return nil, nil, err
	}
	handle := make([]byte, 4)
	rand.Read(handle)
	h.store[Handle(binary.LittleEndian.Uint32(handle))] = k
	pubBytes := x509.MarshalPKCS1PublicKey(&k.PublicKey)
	return handle, pubBytes, nil
}

func (h *rsaHSMKEM) Encapsulate(ek []byte, rho [32]byte) ([]byte, []byte, error) {
	pub, err := x509.ParsePKCS1PublicKey(ek)
	if err != nil {
		return nil, nil, err
	}
	label := []byte("pve-rsa-hsm")
	var rng io.Reader = newCTRRand(rho[:])
	ct, err := rsa.EncryptOAEP(sha256.New(), rng, pub, rho[:], label)
	if err != nil {
		return nil, nil, err
	}
	ss := make([]byte, 32)
	copy(ss, rho[:])
	return ct, ss, nil
}

func (h *rsaHSMKEM) Decapsulate(skHandle unsafe.Pointer, ct []byte) ([]byte, error) {
	// Prefer cmem_t-backed handle; parse first 4 bytes little-endian.
	type cmem_t struct {
		data *byte
		size int32
	}
	var handle Handle
	cm := (*cmem_t)(skHandle)
	if cm != nil && cm.data != nil && cm.size > 0 {
		dk := unsafe.Slice((*byte)(unsafe.Pointer(cm.data)), int(cm.size))
		if len(dk) >= 4 {
			handle = Handle(binary.LittleEndian.Uint32(dk[:4]))
		} else if len(dk) >= 1 {
			handle = Handle(dk[0])
		} else {
			return nil, fmt.Errorf("invalid handle")
		}
	} else {
		handle = Handle(uint32(uintptr(skHandle) & 0xffffffff))
	}
	k, ok := h.store[handle]
	if !ok {
		return nil, fmt.Errorf("unknown handle")
	}
	label := []byte("pve-rsa-hsm")
	ss, err := rsa.DecryptOAEP(sha256.New(), rand.Reader, k, ct, label)
	if err != nil {
		return nil, err
	}
	out := make([]byte, 32)
	copy(out, ss)
	return out, nil
}

func (h *rsaHSMKEM) DerivePub(skRef []byte) ([]byte, error) {
	var handle Handle
	if len(skRef) >= 4 {
		handle = Handle(binary.LittleEndian.Uint32(skRef[:4]))
	} else if len(skRef) >= 1 {
		handle = Handle(skRef[0])
	} else {
		return nil, fmt.Errorf("invalid handle ref")
	}
	k, ok := h.store[handle]
	if !ok {
		return nil, fmt.Errorf("unknown handle %x", skRef)
	}
	return x509.MarshalPKCS1PublicKey(&k.PublicKey), nil
}

// =============================
// Tests for the above KEMs
// =============================

func TestPVEWithRSAGoKEM(t *testing.T) {
	rsaK, err := newRSAGoKEM()
	require.NoError(t, err)
	pve, err := NewPVE(Config{KEM: rsaK})
	require.NoError(t, err)
	cv, _ := curve.NewP256()
	x, _ := cv.RandomScalar()
	prvBytes, pubBytes, err := rsaK.Generate()
	require.NoError(t, err)
	enc, err := pve.Encrypt(&PVEEncryptRequest{PublicKey: pubBytes, PrivateValue: x, Curve: cv, Label: "rsa-go"})
	require.NoError(t, err)
	dec, err := pve.Decrypt(&PVEDecryptRequest{PrivateKey: prvBytes, Ciphertext: enc.Ciphertext, Curve: cv, Label: "rsa-go"})
	require.NoError(t, err)
	require.Equal(t, x.Bytes, dec.PrivateValue.Bytes)
}

func TestPVEWithECDHGoKEM(t *testing.T) {
	ecdhK := newECDHGoKEM()
	pve, err := NewPVE(Config{KEM: ecdhK})
	require.NoError(t, err)
	cv, _ := curve.NewP256()
	x, _ := cv.RandomScalar()
	prvBytes, pubBytes, err := ecdhK.Generate()
	require.NoError(t, err)
	enc, err := pve.Encrypt(&PVEEncryptRequest{PublicKey: pubBytes, PrivateValue: x, Curve: cv, Label: "ecdh-go"})
	require.NoError(t, err)
	dec, err := pve.Decrypt(&PVEDecryptRequest{PrivateKey: prvBytes, Ciphertext: enc.Ciphertext, Curve: cv, Label: "ecdh-go"})
	require.NoError(t, err)
	require.Equal(t, x.Bytes, dec.PrivateValue.Bytes)
}

func TestPVEWithRSAHSMKEM(t *testing.T) {
	hsm := newRSAHSMKEM()
	pve, err := NewPVE(Config{KEM: hsm})
	require.NoError(t, err)
	cv, _ := curve.NewP256()
	x, _ := cv.RandomScalar()
	skRef, ek, err := hsm.Generate()
	require.NoError(t, err)
	enc, err := pve.Encrypt(&PVEEncryptRequest{PublicKey: ek, PrivateValue: x, Curve: cv, Label: "rsa-hsm"})
	require.NoError(t, err)
	dec, err := pve.Decrypt(&PVEDecryptRequest{PrivateKey: skRef, Ciphertext: enc.Ciphertext, Curve: cv, Label: "rsa-hsm"})
	require.NoError(t, err)
	require.Equal(t, x.Bytes, dec.PrivateValue.Bytes)
}
