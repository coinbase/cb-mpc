// Package mtls provides a production-ready implementation of the Messenger interface using mutual TLS.
// This serves as a reference implementation showing how to build secure, authenticated transport
// for multi-party computation protocols.
package mtls

import (
	"context"
	"crypto/sha256"
	"crypto/tls"
	"crypto/x509"
	"encoding/binary"
	"encoding/hex"
	"fmt"
	"io"
	"net"
	"sync"
	"time"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport"
	"golang.org/x/sync/errgroup"
)

// MTLSMessenger implements the Messenger interface using mutual TLS authentication.
// It provides secure, authenticated communication between MPC parties.
type MTLSMessenger struct {
	connections map[int]*tls.Conn
	nameToIndex map[string]int
	listener    net.Listener
	mu          sync.RWMutex
	timeout     time.Duration
	selfIndex   int
}

// Ensure MTLSMessenger implements the Messenger interface
var _ transport.Messenger = (*MTLSMessenger)(nil)

// PartyConfig contains the configuration for a single party
type PartyConfig struct {
	// Address should include the IP/hostname and port
	Address string
	Cert    *x509.Certificate
}

// Config contains the configuration for setting up mutual TLS transport
type Config struct {
	// Parties must include the current party as well as all other parties,
	// the key is the index of the party among all possible parties
	Parties     map[int]PartyConfig
	CertPool    *x509.CertPool
	TLSCert     tls.Certificate
	NameToIndex map[string]int
	SelfIndex   int
}

// PartyNameFromCertificate extracts a unique party name from a certificate by hashing its public key
func PartyNameFromCertificate(cert *x509.Certificate) (string, error) {
	pubKeyBytes, err := x509.MarshalPKIXPublicKey(cert.PublicKey)
	if err != nil {
		return "", fmt.Errorf("marshaling public key: %v", err)
	}
	hash := sha256.Sum256(pubKeyBytes)
	pname := hex.EncodeToString(hash[:])
	return pname, nil
}

// NewMTLSMessenger creates a new MTLSMessenger instance with the given configuration.
// It establishes TLS connections with all other parties according to a deterministic connection pattern.
func NewMTLSMessenger(config Config) (*MTLSMessenger, error) {
	tlsConfig := &tls.Config{
		MinVersion:   tls.VersionTLS13,
		CipherSuites: nil, // use the safe default cipher suites
		Certificates: []tls.Certificate{config.TLSCert},
		RootCAs:      config.CertPool,
		ClientCAs:    config.CertPool,
		ClientAuth:   tls.RequireAndVerifyClientCert,
		VerifyPeerCertificate: func(rawCerts [][]byte, verifiedChains [][]*x509.Certificate) error {
			if len(rawCerts) == 0 {
				return fmt.Errorf("no server certificate provided")
			}

			serverCert, err := x509.ParseCertificate(rawCerts[0])
			if err != nil {
				return fmt.Errorf("parsing server certificate: %v", err)
			}

			peerPname, err := PartyNameFromCertificate(serverCert)
			if err != nil {
				return fmt.Errorf("extracting peer name from server certificate: %v", err)
			}
			peerIndex, ok := config.NameToIndex[peerPname]
			if !ok {
				return fmt.Errorf("peer name %s not found in name to index map", peerPname)
			}
			if !serverCert.Equal(config.Parties[peerIndex].Cert) {
				return fmt.Errorf("server certificate does not match the expected certificate: %v", peerIndex)
			}

			return nil
		},
	}

	expectedIncomingConnectionsCount := 0
	expectedOutgoingConnectionsCount := 0
	for i := range config.Parties {
		if i < config.SelfIndex {
			expectedOutgoingConnectionsCount++
		}
		if i > config.SelfIndex {
			expectedIncomingConnectionsCount++
		}
	}

	fmt.Printf("Party %d: expected %d incoming connections and %d outgoing connections\n", config.SelfIndex, expectedIncomingConnectionsCount, expectedOutgoingConnectionsCount)

	transport := &MTLSMessenger{
		connections: make(map[int]*tls.Conn),
		timeout:     time.Hour * 1,
		selfIndex:   config.SelfIndex,
		nameToIndex: config.NameToIndex,
	}

	wg := sync.WaitGroup{}

	if expectedIncomingConnectionsCount != 0 {
		myAddress := config.Parties[config.SelfIndex].Address
		ln, err := tls.Listen("tcp", myAddress, tlsConfig)
		if err != nil {
			return nil, fmt.Errorf("starting server on %s: %v", myAddress, err)
		}
		transport.listener = ln

		for i := 0; i < expectedIncomingConnectionsCount; i++ {
			wg.Add(1)
			go func(i int) {
				defer wg.Done()
				conn, err := ln.Accept()
				if err != nil {
					fmt.Printf("error accepting connection on %s: %v\n", myAddress, err)
					return
				}
				c := conn.(*tls.Conn)

				// Explicitly complete the TLS handshake. This will let us access the peer certificates.
				if err := c.Handshake(); err != nil {
					fmt.Printf("TLS handshake failed: %v\n", err)
					c.Close()
					return
				}
				peerCerts := c.ConnectionState().PeerCertificates
				if len(peerCerts) == 0 {
					fmt.Printf("No peer certificates found\n")
					return
				}
				peerName, err := PartyNameFromCertificate(peerCerts[0])
				if err != nil {
					fmt.Printf("error extracting peer name from certificate: %v\n", err)
					return
				}
				peerIndex, ok := transport.nameToIndex[peerName]
				if !ok {
					fmt.Printf("peer name %s not found in name to index map\n", peerName)
					return
				}
				fmt.Printf("Party %d: peer %d connected\n", config.SelfIndex, peerIndex)

				transport.mu.Lock()
				transport.connections[peerIndex] = c
				transport.mu.Unlock()
			}(i)
		}
	}

	for i, party := range config.Parties {
		if i < config.SelfIndex {
			// Exponential backoff with cap for connection retries
			backoff := 1 * time.Second
			maxBackoff := 20 * time.Second
			attempts := 0
			for {
				counterPartyAddress := party.Address
				conn, err := tls.Dial("tcp", counterPartyAddress, tlsConfig)
				if err != nil {
					attempts++
					if attempts > 10 {
						return nil, fmt.Errorf("connecting to %s: %v", counterPartyAddress, err)
					}
					time.Sleep(backoff)
					backoff *= 2
					if backoff > maxBackoff {
						backoff = maxBackoff
					}
					continue
				}
				transport.mu.Lock()
				transport.connections[i] = conn
				transport.mu.Unlock()
				break
			}
		}
	}

	// Wait for all incoming connections to be established
	wg.Wait()

	return transport, nil
}

// MessageSend sends a message to the specified receiver party
func (dt *MTLSMessenger) MessageSend(_ context.Context, receiverIndex int, buffer []byte) error {
	conn, ok := dt.connections[receiverIndex]

	if !ok {
		return fmt.Errorf("no connection found for receiver index %d", receiverIndex)
	}

	// Send message length first (4 bytes, big endian)
	messageLength := uint32(len(buffer))
	lengthBytes := make([]byte, 4)
	binary.BigEndian.PutUint32(lengthBytes, messageLength)

	if _, err := conn.Write(lengthBytes); err != nil {
		return fmt.Errorf("writing message length: %v", err)
	}

	// Send the actual message
	if _, err := conn.Write(buffer); err != nil {
		return fmt.Errorf("writing message data: %v", err)
	}

	return nil
}

// MessageReceive receives a message from the specified sender party
func (dt *MTLSMessenger) MessageReceive(_ context.Context, senderIndex int) ([]byte, error) {
	conn, ok := dt.connections[senderIndex]

	if !ok {
		return nil, fmt.Errorf("no connection found for sender index %d", senderIndex)
	}

	// Read message length first (4 bytes)
	lengthBytes := make([]byte, 4)
	if _, err := io.ReadFull(conn, lengthBytes); err != nil {
		return nil, fmt.Errorf("reading message length: %v", err)
	}

	messageLength := binary.BigEndian.Uint32(lengthBytes)

	// Validate message length to prevent excessive memory allocation
	if messageLength > 10*1024*1024 { // 10MB limit
		return nil, fmt.Errorf("message too large: %d bytes", messageLength)
	}

	// Read the exact amount of message data
	buffer := make([]byte, messageLength)
	if _, err := io.ReadFull(conn, buffer); err != nil {
		return nil, fmt.Errorf("reading message data: %v", err)
	}

	return buffer, nil
}

// MessagesReceive receives messages from multiple sender parties concurrently
func (dt *MTLSMessenger) MessagesReceive(ctx context.Context, senderIndices []int) ([][]byte, error) {
	receivedMsgs := make([][]byte, len(senderIndices))

	eg := errgroup.Group{}
	wg := sync.WaitGroup{}
	wg.Add(len(senderIndices))

	for i, senderIndex := range senderIndices {
		eg.Go(func() error {
			defer wg.Done()
			msg, err := dt.MessageReceive(ctx, senderIndex)
			if err != nil {
				return fmt.Errorf("receiving message from %d: %v", senderIndex, err)
			}
			receivedMsgs[i] = msg
			return nil
		})
	}
	wg.Wait()

	if err := eg.Wait(); err != nil {
		return nil, fmt.Errorf("receiving messages: %v", err)
	}
	return receivedMsgs, nil
}

// Close closes all connections and cleans up resources
func (dt *MTLSMessenger) Close() error {
	dt.mu.Lock()
	defer dt.mu.Unlock()

	fmt.Printf("Closing MTLSMessenger for party %d\n", dt.selfIndex)

	// Close all connections
	for idx, conn := range dt.connections {
		if conn != nil {
			fmt.Printf("Closing connection to party %d\n", idx)
			conn.Close()
		}
	}

	// Clear the connections map
	dt.connections = make(map[int]*tls.Conn)

	// Close listener if it exists
	if dt.listener != nil {
		fmt.Printf("Closing listener for party %d\n", dt.selfIndex)
		err := dt.listener.Close()
		dt.listener = nil
		if err != nil {
			fmt.Printf("Error closing listener: %v\n", err)
			return err
		}
	}

	fmt.Printf("MTLSMessenger closed successfully for party %d\n", dt.selfIndex)
	return nil
}
