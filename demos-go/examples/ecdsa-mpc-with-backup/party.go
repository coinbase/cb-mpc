package main

import (
	"fmt"
	"log"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/mpc"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mocknet"
)

type Party struct {
	Index      int
	Messenger  *mocknet.MockMessenger
	NParties   int
	PartyNames []string
	BatchSize  int
	pve        *mpc.PVE
	dkgResp    []*mpc.ECDSAMPCKeyGenResponse
	signResp   []*mpc.ECDSAMPCSignResponse
	rsaEk      mpc.BaseEncPublicKey
	rsaDk      mpc.BaseEncPrivateKey
}

func (p *Party) Dkg(c curve.Curve, batchId int) error {
	jb, err := mpc.NewJobMP(p.Messenger, p.NParties, p.Index, p.PartyNames)
	if err != nil {
		return err
	}
	defer jb.Free()

	resp, err := mpc.ECDSAMPCKeyGen(jb, &mpc.ECDSAMPCKeyGenRequest{Curve: c})
	if err != nil {
		return err
	}

	p.dkgResp[batchId] = resp
	return nil
}

func (p *Party) Sign(hash [32]byte, signatureReceiverId int, batchId int) error {
	jb, err := mpc.NewJobMP(p.Messenger, p.NParties, p.Index, p.PartyNames)
	if err != nil {
		return err
	}
	defer jb.Free()

	resp, err := mpc.ECDSAMPCSign(jb, &mpc.ECDSAMPCSignRequest{
		KeyShare:          p.dkgResp[batchId].KeyShare,
		Message:           hash[:],
		SignatureReceiver: signatureReceiverId,
	})
	if err != nil {
		return err
	}
	p.signResp[batchId] = resp

	return nil
}

func (p *Party) InitPVE() {
	var err error
	p.pve, err = mpc.NewPVE(mpc.Config{KEM: rsaDemoKEM{}})
	if err != nil {
		log.Fatal("failed to init PVE: %v", err)
	}
}

func (p *Party) Q(batchId int) *curve.Point {
	// will panic dkgResp is nil, ok for demo purposes
	Q, err := p.dkgResp[batchId].KeyShare.Q()
	if err != nil {
		log.Fatalf("failed to get public key: %v", err)
	}
	return Q
}

func (p *Party) RSAKeygen() []byte {
	dk, ek, err := rsaDemoKEM{}.Generate()
	if err != nil {
		log.Fatalf("failed to generate base encryption key pair: %v", err)
	}
	p.rsaDk = mpc.BaseEncPrivateKey(dk)
	p.rsaEk = mpc.BaseEncPublicKey(ek)
	return ek
}

func (p *Party) Backup(inputLabel string, c curve.Curve, ac *mpc.AccessStructure, pubKeys map[string]mpc.BaseEncPublicKey) *mpc.PVEAcEncryptResponse {
	xs := make([]*curve.Scalar, p.BatchSize)
	Xs := make([]*curve.Point, p.BatchSize)
	var err error
	for k := 0; k < p.BatchSize; k++ {
		xs[k], err = p.dkgResp[k].KeyShare.XShare()
		if err != nil {
			log.Fatalf("failed to get X share: %v", err)
		}
		Qis, err := p.dkgResp[k].KeyShare.Qis()
		if err != nil {
			log.Fatalf("failed to get Qis: %v", err)
		}
		Xs[k] = Qis[p.PartyNames[p.Index]]
	}

	pveEncResp, err := p.pve.AcEncrypt(&mpc.PVEAcEncryptRequest{
		AccessStructure: ac,
		PublicKeys:      pubKeys,
		PrivateValues:   xs,
		Label:           inputLabel,
		Curve:           c,
	})
	if err != nil {
		log.Fatalf("failed to encrypt: %v", err)
	}
	return pveEncResp
}

func (p *Party) VerifyAllBackups(backups []*mpc.PVEAcEncryptResponse, ac *mpc.AccessStructure, inputLabel string, pubKeys map[string]mpc.BaseEncPublicKey) error {
	for j := 0; j < p.NParties; j++ {
		// Party i creates what she thinks the public value for party j should be and verifies against that value
		Xs := make([]*curve.Point, p.BatchSize)
		var err error
		for k := 0; k < p.BatchSize; k++ {
			Qis, err := p.dkgResp[k].KeyShare.Qis()
			if err != nil {
				return fmt.Errorf("failed to get Qis: %v", err)
			}
			Xs[k] = Qis[p.PartyNames[j]]
		}

		verifyResp, err := p.pve.AcVerify(&mpc.PVEAcVerifyRequest{
			AccessStructure: ac,
			EncryptedBundle: backups[j].EncryptedBundle,
			PublicKeys:      pubKeys,
			PublicShares:    Xs,
			Label:           inputLabel,
		})
		if err != nil {
			return fmt.Errorf("failed to verify: %v", err)
		}
		if !verifyResp.Valid {
			return fmt.Errorf("PVE verification failed")
		}
	}
	return nil
}

func (p *Party) PartialBackupDecryption(backups []*mpc.PVEAcEncryptResponse, inputLabel string, ac *mpc.AccessStructure) []*mpc.PVEAcPartyDecryptRowResponse {
	resps := make([]*mpc.PVEAcPartyDecryptRowResponse, p.NParties)
	var err error
	for j := 0; j < p.NParties; j++ {
		resps[j], err = p.pve.AcPartyDecryptRow(&mpc.PVEAcPartyDecryptRowRequest{
			AccessStructure: ac,
			Path:            p.PartyNames[p.Index],
			PrivateKey:      p.rsaDk,
			EncryptedBundle: backups[j].EncryptedBundle,
			Label:           inputLabel,
			RowIndex:        0,
		})
		if err != nil {
			log.Fatalf("failed to party decrypt row: %v", err)
		}
	}
	return resps
}

func (p *Party) AggregatePartialDecryptedBackups(partialDecryptions map[string]*mpc.PVEAcPartyDecryptRowResponse, backup *mpc.PVEAcEncryptResponse, inputLabel string, ac *mpc.AccessStructure) *mpc.PVEAcAggregateToRestoreRowResponse {
	shares := make(map[string][]byte)
	for pname, resp := range partialDecryptions {
		shares[pname] = resp.Share
	}
	aggResp, err := p.pve.AcAggregateToRestoreRow(&mpc.PVEAcAggregateToRestoreRowRequest{
		AccessStructure: ac,
		EncryptedBundle: backup.EncryptedBundle,
		Label:           inputLabel,
		RowIndex:        0,
		Shares:          shares,
	})
	if err != nil {
		log.Fatalf("failed to aggregate: %v", err)
	}

	return aggResp
}
