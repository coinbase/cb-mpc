package main

import (
	"encoding/asn1"
	"flag"
	"fmt"
	"log"
	"math/big"
	"strconv"
	"strings"

	"github.com/spf13/viper"
)

// ASN.1 structures for OpenSSL compatibility
type ecdsaSignature struct {
	R *big.Int
	S *big.Int
}

type publicKeyInfo struct {
	Algorithm algorithmIdentifier
	PublicKey asn1.BitString
}

type algorithmIdentifier struct {
	Algorithm  asn1.ObjectIdentifier
	Parameters asn1.ObjectIdentifier `asn1:"optional"`
}

// secp256k1 OID: 1.3.132.0.10
var secp256k1OID = asn1.ObjectIdentifier{1, 3, 132, 0, 10}

// ecPublicKey OID: 1.2.840.10045.2.1
var ecPublicKeyOID = asn1.ObjectIdentifier{1, 2, 840, 10045, 2, 1}

type PartyConfig struct {
	Address string `yaml:"address"`
	Cert    string `yaml:"cert"`
}

type Config struct {
	CaFile     string        `yaml:"caFile"`
	CertFile   string        `yaml:"certFile"`
	WebAddress string        `yaml:"webAddress"`
	KeyFile    string        `yaml:"keyFile"`
	Parties    []PartyConfig `yaml:"parties"`
}

type RunConfig struct {
	Config              Config
	ParticipantsIndices map[int]bool
	Phase               string // dkg or sign
	Threshold           int    // threshold for DKG or signing
	MyIndex             int    // my index in the list of participants
}

func readConfig() (*RunConfig, error) {
	var configFile string
	var participants string
	var phase string
	var threshold int
	var myIndex int

	flag.StringVar(&configFile, "config", "", "path to config file")
	flag.StringVar(&participants, "participants", "", "comma-separated list of participant indices")
	flag.StringVar(&phase, "phase", "", "phase to run: agree-random or dkg or sign")
	flag.IntVar(&threshold, "threshold", 3, "threshold for DKG or signing")
	flag.IntVar(&myIndex, "index", 0, "my index in the list of participants")
	flag.Parse()

	if configFile == "" {
		configFile = fmt.Sprintf("config-%d.yaml", myIndex)
	}
	if strings.HasSuffix(configFile, ".yaml") {
		configFile = configFile[:len(configFile)-5]
	}

	viper.SetConfigName(configFile)
	viper.SetConfigType("yaml")
	viper.AddConfigPath(".")

	if err := viper.ReadInConfig(); err != nil {
		return nil, fmt.Errorf("Error reading config file, %s", err)
	}

	var config Config
	if err := viper.Unmarshal(&config); err != nil {
		return nil, fmt.Errorf("unable to decode into struct, %v", err)
	}

	participantsIndices := make(map[int]bool)
	if participants != "" {
		tokens := strings.SplitSeq(participants, ",")
		for token := range tokens {
			participantIndex, err := strconv.Atoi(token)
			if err != nil {
				return nil, fmt.Errorf("invalid participant index: %v", err)
			}
			participantsIndices[participantIndex] = true
		}
	} else {
		for i := range len(config.Parties) {
			participantsIndices[i] = true
		}
	}
	fmt.Printf("Running with %d total parties\n", len(participantsIndices))

	return &RunConfig{
		Config:              config,
		ParticipantsIndices: participantsIndices,
		Phase:               phase,
		Threshold:           threshold,
		MyIndex:             myIndex,
	}, nil
}

func main() {
	runConfig, err := readConfig()
	if err != nil {
		log.Fatalf("Error reading config: %v", err)
	}

	if err := main_web(runConfig); err != nil {
		log.Fatalf("Error running web: %v", err)
	}
}
