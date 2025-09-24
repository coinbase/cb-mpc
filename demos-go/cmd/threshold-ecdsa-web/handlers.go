package main

import (
	"crypto/sha256"
	"crypto/tls"
	"crypto/x509"
	"encoding/asn1"
	"encoding/base64"
	"fmt"
	"math/big"
	"os"
	"path/filepath"
	"strconv"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/mpc"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mtls"
)

const LEADER_INDEX = 0

type KeyShareData struct {
	KeyShare  [][]byte `json:"keyShare"`
	PartyName string   `json:"partyName"`
}

func runThresholdDKG(partyIndex int, quorumCount int, allPNameList []string, ac *mpc.AccessStructure, curve curve.Curve, messenger transport.Messenger) (*mpc.ECDSAMPCKey, error) {
	totalPartyCount := len(allPNameList) // since dkg involves all parties
	job, err := mpc.NewJobMP(messenger, totalPartyCount, partyIndex, allPNameList)
	if err != nil {
		return nil, fmt.Errorf("failed to create job: %v", err)
	}
	defer job.Free()

	if quorumCount < 2 {
		return nil, fmt.Errorf("threshold must be at least 1")
	}
	if quorumCount > totalPartyCount {
		return nil, fmt.Errorf("threshold must be less than or equal to the number of online parties")
	}

	sid := make([]byte, 0) // empty sid means that the dkg will generate it internally
	keyShare, err := mpc.ECDSAMPCThresholdDKG(job, &mpc.ECDSAMPCThresholdDKGRequest{
		Curve: curve, SessionID: sid, AccessStructure: ac,
	})
	if err != nil {
		return nil, fmt.Errorf("threshold DKG failed: %v", err)
	}
	return &keyShare.KeyShare, nil
}

func runThresholdSign(keyShareRef *mpc.ECDSAMPCKey, ac *mpc.AccessStructure, partyIndex int, quorumCount int, quorumPNames []string, inputMessage []byte, messenger transport.Messenger) ([]byte, []byte, error) {
	keyShare := *keyShareRef

	job, err := mpc.NewJobMP(messenger, quorumCount, partyIndex, quorumPNames)
	if err != nil {
		return nil, nil, fmt.Errorf("failed to create job: %v", err)
	}
	defer job.Free()

	if quorumCount != len(quorumPNames) {
		return nil, nil, fmt.Errorf("quorum count does not match the number of participants")
	}

	hashedMessage := sha256.Sum256(inputMessage)
	message := hashedMessage[:]

	additiveShare, err := keyShare.ToAdditiveShare(ac, quorumPNames)
	if err != nil {
		return nil, nil, fmt.Errorf("converting to additive share: %v", err)
	}

	sigResponse, err := mpc.ECDSAMPCSign(job, &mpc.ECDSAMPCSignRequest{
		KeyShare:          additiveShare,
		Message:           message,
		SignatureReceiver: LEADER_INDEX,
	})
	if err != nil {
		return nil, nil, fmt.Errorf("signing failed: %v", err)
	}

	derSig := []byte{}
	pemKey := []byte{}
	if partyIndex == LEADER_INDEX {
		derSig, err = createDERSignature(sigResponse.Signature)
		if err != nil {
			return nil, nil, fmt.Errorf("creating DER signature: %v", err)
		}

		pemKey, err = createPEMPublicKey(&additiveShare)
		if err != nil {
			return nil, nil, fmt.Errorf("creating PEM public key: %v", err)
		}

	}
	return derSig, pemKey, nil
}

func loadPartyConfig(certPEM []byte, partyAddress string) (mtls.PartyConfig, error) {
	cert, err := x509.ParseCertificate(certPEM)
	if err != nil {
		return mtls.PartyConfig{}, fmt.Errorf("failed to parse expected server cert: %v", err)
	}
	return mtls.PartyConfig{
		Address: partyAddress,
		Cert:    cert,
	}, nil
}

func setupTransport(config Config, partyIndex int, participantsIndices map[int]bool) (int, string, []string, []string, transport.Messenger, error) {
	cert, err := tls.LoadX509KeyPair(config.CertFile, config.KeyFile)
	if err != nil {
		return 0, "", nil, nil, nil, fmt.Errorf("loading key pair %s and %s: %v", config.CertFile, config.KeyFile, err)
	}

	caCert, err := os.ReadFile(config.CaFile)
	if err != nil {
		return 0, "", nil, nil, nil, fmt.Errorf("reading CA cert %s: %v", config.CaFile, err)
	}
	caCertPool := x509.NewCertPool()
	caCertPool.AppendCertsFromPEM(caCert)

	allPNames := make([]string, 0)
	participantPNames := make([]string, 0)

	myPname := ""
	myNetworkPartyIndex := 0
	networkPartyIndex := 0
	networkParties := make(map[int]mtls.PartyConfig)
	nameToIndex := make(map[string]int)
	for i, party := range config.Parties {
		certPEM, err := os.ReadFile(party.Cert)
		if err != nil {
			return 0, "", nil, nil, nil, fmt.Errorf("failed to read expected server cert: %v", err)
		}
		networkParty, err := loadPartyConfig(certPEM, party.Address)
		if err != nil {
			return 0, "", nil, nil, nil, fmt.Errorf("loading party config: %v", err)
		}
		pname, err := mtls.PartyNameFromCertificate(networkParty.Cert)
		if err != nil {
			return 0, "", nil, nil, nil, fmt.Errorf("extracting pname from cert: %v", err)
		}
		allPNames = append(allPNames, pname)

		if i == partyIndex {
			myNetworkPartyIndex = networkPartyIndex
			myPname = pname
		}
		if _, ok := participantsIndices[i]; ok {
			participantPNames = append(participantPNames, pname)
			networkParties[networkPartyIndex] = networkParty
			nameToIndex[pname] = networkPartyIndex
			networkPartyIndex++
		}
	}

	transport, err := mtls.NewMTLSMessenger(mtls.Config{
		Parties:     networkParties,
		CertPool:    caCertPool,
		TLSCert:     cert,
		SelfIndex:   myNetworkPartyIndex,
		NameToIndex: nameToIndex,
	})
	if err != nil {
		return 0, "", nil, nil, nil, fmt.Errorf("failed to create transport: %v", err)
	}
	fmt.Printf("transport:\n")
	fmt.Printf("   - myPname: %s\n", myPname)
	fmt.Printf("   - myIndex: %d\n", myNetworkPartyIndex)
	fmt.Printf("   - networkParties: %+v\n", networkParties)

	fmt.Println("MTLSDataTransport initialized successfully")
	return myNetworkPartyIndex, myPname, allPNames, participantPNames, transport, nil
}

func createThresholdAccessStructure(pnameList []string, threshold int, curve curve.Curve) mpc.AccessStructure {
	root := mpc.Threshold("", threshold)
	for _, pname := range pnameList {
		child := mpc.Leaf(pname)
		root.Children = append(root.Children, child)
	}

	ac := mpc.AccessStructure{
		Root:  root,
		Curve: curve,
	}
	return ac
}

func saveKeyShare(keyShare *mpc.ECDSAMPCKey, partyName string) error {
	ser, err := keyShare.MarshalBinary()
	if err != nil {
		return fmt.Errorf("serializing key share: %v", err)
	}

	filename := fmt.Sprintf("keyshare_party_%s.json", partyName)
	if err := os.WriteFile(filename, ser, 0600); err != nil {
		return fmt.Errorf("writing key file: %v", err)
	}

	return nil
}

func saveThreshold(threshold int) error {
	if err := os.WriteFile("threshold.txt", fmt.Appendf(nil, "%d", threshold), 0600); err != nil {
		return fmt.Errorf("writing key file: %v", err)
	}

	return nil
}

func loadThreshold() (int, error) {
	data, err := os.ReadFile("threshold.txt")
	if err != nil {
		return 0, fmt.Errorf("reading threshold file: %v", err)
	}
	return strconv.Atoi(string(data))
}

func removeKeyShares() {
	files, err := filepath.Glob("keyshare_party_*.json")
	if err != nil {
		fmt.Printf("finding key share files: %v\n", err)
	}
	for _, file := range files {
		if err := os.Remove(file); err != nil {
			fmt.Printf("removing key share file %s: %v\n", file, err)
		}
	}
}

func loadKeyShare(partyName string) (*mpc.ECDSAMPCKey, error) {
	filename := fmt.Sprintf("keyshare_party_%s.json", partyName)
	data, err := os.ReadFile(filename)
	if err != nil {
		return nil, fmt.Errorf("reading key file: %v", err)
	}

	keyShare := mpc.ECDSAMPCKey{}
	if err := keyShare.UnmarshalBinary(data); err != nil {
		return nil, fmt.Errorf("unmarshaling key data: %v", err)
	}

	return &keyShare, nil
}

// createOTRoleMap creates a default OT role map for a given number of parties
func createOTRoleMap(nParties int) [][]int {
	const (
		OT_NO_ROLE  = -1
		OT_SENDER   = 0
		OT_RECEIVER = 1
	)

	otRoleMap := make([][]int, nParties)
	for i := 0; i < nParties; i++ {
		otRoleMap[i] = make([]int, nParties)
		otRoleMap[i][i] = OT_NO_ROLE
	}

	for i := 0; i < nParties; i++ {
		for j := i + 1; j < nParties; j++ {
			otRoleMap[i][j] = OT_SENDER
			otRoleMap[j][i] = OT_RECEIVER
		}
	}

	return otRoleMap
}

func createPEMPublicKey(key *mpc.ECDSAMPCKey) ([]byte, error) {
	Q, err := key.Q()
	if err != nil {
		return nil, fmt.Errorf("extracting public key: %v", err)
	}
	pubKeyX, pubKeyY := Q.GetX(), Q.GetY()
	if err != nil {
		return nil, fmt.Errorf("extracting public key: %v", err)
	}

	// Create uncompressed public key (0x04 prefix + X + Y)
	pubKeyBytes := make([]byte, 1+len(pubKeyX)+len(pubKeyY))
	pubKeyBytes[0] = 0x04
	copy(pubKeyBytes[1:], pubKeyX)
	copy(pubKeyBytes[1+len(pubKeyX):], pubKeyY)

	// Create ASN.1 structure
	pubKeyInfo := publicKeyInfo{
		Algorithm: algorithmIdentifier{
			Algorithm:  ecPublicKeyOID,
			Parameters: secp256k1OID,
		},
		PublicKey: asn1.BitString{
			Bytes:     pubKeyBytes,
			BitLength: len(pubKeyBytes) * 8,
		},
	}

	derBytes, err := asn1.Marshal(pubKeyInfo)
	if err != nil {
		return nil, fmt.Errorf("marshaling public key: %v", err)
	}

	pemData := "-----BEGIN PUBLIC KEY-----\n"
	b64Data := base64.StdEncoding.EncodeToString(derBytes)
	for i := 0; i < len(b64Data); i += 64 {
		end := i + 64
		if end > len(b64Data) {
			end = len(b64Data)
		}
		pemData += b64Data[i:end] + "\n"
	}
	pemData += "-----END PUBLIC KEY-----\n"

	return []byte(pemData), nil
}

func createDERSignature(signature []byte) ([]byte, error) {
	// Parse signature - assuming it's already in DER format or raw r,s format
	// If it's 64 bytes, it's likely raw r,s (32 bytes each)
	if len(signature) == 64 {
		// Raw format: first 32 bytes = r, next 32 bytes = s
		r := new(big.Int).SetBytes(signature[:32])
		s := new(big.Int).SetBytes(signature[32:])

		sig := ecdsaSignature{R: r, S: s}
		derBytes, err := asn1.Marshal(sig)
		if err != nil {
			return nil, fmt.Errorf("marshaling signature: %v", err)
		}
		return derBytes, nil
	} else {
		return signature, nil
	}
}
