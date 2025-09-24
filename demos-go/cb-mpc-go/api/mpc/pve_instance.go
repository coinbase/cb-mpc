package mpc

import (
	"fmt"
	"unsafe"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/internal/cgobinding"
)

// KEM is a pluggable "key encapsulation mechanism" backend.
// We alias the low-level cgobinding.KEM interface so callers only have to
// satisfy a single contract across the whole code base.
type KEM = cgobinding.KEM

type Config struct {
	KEM KEM
}

func (c *Config) normalise() error {
	if c.KEM == nil {
		return fmt.Errorf("pve: Config.KEM cannot be nil")
	}
	return nil
}

// PVE is an instance-level façade around the PVE helpers.
type PVE struct {
	kem KEM
	ctx unsafe.Pointer // context handle passed to C
}

func NewPVE(cfg Config) (*PVE, error) {
	if err := cfg.normalise(); err != nil {
		return nil, err
	}
	ctxPtr, err := cgobinding.RegisterKEMInstance(cfg.KEM)
	if err != nil {
		return nil, err
	}
	return &PVE{kem: cfg.KEM, ctx: ctxPtr}, nil
}

func (p *PVE) activateCtx() { cgobinding.ActivateCtx(p.ctx) }
