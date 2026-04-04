package common

import (
	"context"
	"crypto/tls"
	"crypto/x509"
	"encoding/binary"
	"errors"
	"fmt"
	"io"
	"net"
	"os"
	"strings"
	"sync"
	"time"
)

// maxMessageSize is the maximum allowed message size (64 MB).
// This prevents a malicious peer from causing OOM via a large length prefix.
const maxMessageSize = 64 * 1024 * 1024

var demoPeerLabels = []string{
	"alpha",
	"bravo",
	"charlie",
	"delta",
	"echo",
	"foxtrot",
	"golf",
	"hotel",
	"india",
	"juliet",
	"kilo",
	"lima",
	"mike",
	"november",
	"oscar",
	"papa",
	"quebec",
	"romeo",
	"sierra",
	"tango",
	"uniform",
	"victor",
	"whiskey",
	"xray",
	"yankee",
	"zulu",
}

func demoPeerNames(count int) ([]string, error) {
	if count < 0 {
		return nil, fmt.Errorf("partyCount must be non-negative, got %d", count)
	}
	if count > len(demoPeerLabels) {
		return nil, fmt.Errorf("partyCount %d exceeds demo peer identity limit %d", count, len(demoPeerLabels))
	}
	names := make([]string, count)
	for i := 0; i < count; i++ {
		names[i] = fmt.Sprintf("peer-%s.demo.cbmpc.local", demoPeerLabels[i])
	}
	return names, nil
}

// MTLSTransport is a reference mTLS-backed transport implementation for local demos.
//
// It supports a local full-mesh topology where each party may listen on its own
// address and parties dial lower-index peers to avoid duplicate connections.
// It is intended to demonstrate how callers can satisfy the mpc.Transport
// contract over TCP+mTLS, not to serve as a production-ready transport stack.
type MTLSTransport struct {
	myIndex    int
	partyCount int
	peerNames  []string

	listener  net.Listener
	clients   []net.Conn
	clientsMu sync.RWMutex
	writeMu   []sync.Mutex

	messageQueues []chan []byte

	ctx    context.Context
	cancel context.CancelFunc
	closed bool
	mu     sync.Mutex
}

// NewMTLSTransport creates a new local demo mTLS transport.
//
// The caller provides:
//   - listenAddr for this party, if this party should accept inbound peer connections
//   - partyAddrs indexed by party, so this party can dial lower-index peers
//
// To avoid duplicate connections, parties only dial peers with indices lower
// than their own.
//
// This helper assumes a demo PKI model where peer identities are encoded as
// deterministic non-numeric certificate names such as
// `peer-alpha.demo.cbmpc.local`, while clients still verify servers against the
// shared `localhost` loopback SAN.
func NewMTLSTransport(myIndex, partyCount int, listenAddr string, partyAddrs []string, tlsConfig *tls.Config) (*MTLSTransport, error) {
	if tlsConfig == nil {
		return nil, fmt.Errorf("tlsConfig cannot be nil")
	}
	if partyCount < 2 {
		return nil, fmt.Errorf("partyCount must be at least 2, got %d", partyCount)
	}
	if len(partyAddrs) != partyCount {
		return nil, fmt.Errorf("partyAddrs length mismatch: got %d want %d", len(partyAddrs), partyCount)
	}
	if myIndex < 0 || myIndex >= partyCount {
		return nil, fmt.Errorf("myIndex %d out of range [0, %d)", myIndex, partyCount)
	}
	if myIndex < partyCount-1 && listenAddr == "" {
		return nil, fmt.Errorf("listenAddr is required for party %d because higher-index peers dial this party", myIndex)
	}
	for peer := 0; peer < myIndex; peer++ {
		if partyAddrs[peer] == "" {
			return nil, fmt.Errorf("partyAddrs[%d] is required for party %d to dial lower-index peers", peer, myIndex)
		}
	}
	peerNames, err := demoPeerNames(partyCount)
	if err != nil {
		return nil, err
	}

	ctx, cancel := context.WithCancel(context.Background())
	session := &MTLSTransport{
		myIndex:       myIndex,
		partyCount:    partyCount,
		peerNames:     peerNames,
		clients:       make([]net.Conn, partyCount),
		writeMu:       make([]sync.Mutex, partyCount),
		messageQueues: make([]chan []byte, partyCount),
		ctx:           ctx,
		cancel:        cancel,
	}
	for i := 0; i < partyCount; i++ {
		session.messageQueues[i] = make(chan []byte, 100)
	}

	if listenAddr != "" {
		listener, err := tls.Listen("tcp", listenAddr, tlsConfig)
		if err != nil {
			cancel()
			return nil, fmt.Errorf("failed to start TLS listener: %w", err)
		}
		session.listener = listener
		go session.acceptConnections()
	}

	go session.connectToParties(partyAddrs, tlsConfig)
	return session, nil
}

// WaitForPeers blocks until connections to the requested peers are available.
func (s *MTLSTransport) WaitForPeers(ctx context.Context, peers []int) error {
	if ctx == nil {
		ctx = context.Background()
	}
	for _, peer := range peers {
		if peer < 0 || peer >= s.partyCount || peer == s.myIndex {
			return fmt.Errorf("invalid peer index %d", peer)
		}
	}

	ticker := time.NewTicker(50 * time.Millisecond)
	defer ticker.Stop()

	for {
		if err := s.ctx.Err(); err != nil {
			return err
		}
		if err := ctx.Err(); err != nil {
			return err
		}

		ready := true
		s.clientsMu.RLock()
		for _, peer := range peers {
			if s.clients[peer] == nil {
				ready = false
				break
			}
		}
		s.clientsMu.RUnlock()
		if ready {
			return nil
		}

		select {
		case <-ticker.C:
		case <-ctx.Done():
			return ctx.Err()
		case <-s.ctx.Done():
			return s.ctx.Err()
		}
	}
}

func (s *MTLSTransport) acceptConnections() {
	defer s.listener.Close()

	for {
		select {
		case <-s.ctx.Done():
			return
		default:
		}

		conn, err := s.listener.Accept()
		if err != nil {
			select {
			case <-s.ctx.Done():
				return
			default:
			}
			fmt.Printf("Error accepting connection: %v\n", err)
			continue
		}

		go s.handleConnection(conn)
	}
}

func (s *MTLSTransport) connectToParties(partyAddrs []string, tlsConfig *tls.Config) {
	for i, addr := range partyAddrs {
		if i >= s.myIndex || addr == "" {
			continue
		}

		go func(partyIndex int, address string) {
			for {
				select {
				case <-s.ctx.Done():
					return
				default:
				}

				conn, err := tls.Dial("tcp", address, tlsConfig)
				if err != nil {
					time.Sleep(1 * time.Second)
					continue
				}
				peerIndex, err := s.peerPartyIndex(conn)
				if err != nil || peerIndex != partyIndex {
					_ = conn.Close()
					time.Sleep(1 * time.Second)
					continue
				}

				registered := s.registerPeerConn(partyIndex, conn)
				if !registered {
					_ = conn.Close()
					return
				}

				go s.handleConnection(conn)
				return
			}
		}(i, addr)
	}
}

func (s *MTLSTransport) peerPartyIndex(conn net.Conn) (int, error) {
	tlsConn, ok := conn.(*tls.Conn)
	if !ok {
		return -1, fmt.Errorf("expected TLS connection, got %T", conn)
	}
	if err := tlsConn.Handshake(); err != nil {
		return -1, fmt.Errorf("TLS handshake failed: %w", err)
	}

	state := tlsConn.ConnectionState()
	if len(state.PeerCertificates) == 0 {
		return -1, fmt.Errorf("missing peer certificate")
	}
	cert := state.PeerCertificates[0]

	candidates := make([]string, 0, len(cert.DNSNames)+1)
	candidates = append(candidates, cert.DNSNames...)
	if cert.Subject.CommonName != "" {
		candidates = append(candidates, cert.Subject.CommonName)
	}

	for _, name := range candidates {
		for idx, expected := range s.peerNames {
			if name != expected {
				continue
			}
			if idx == s.myIndex {
				return -1, fmt.Errorf("peer identity %q matches local index", name)
			}
			return idx, nil
		}
	}

	return -1, fmt.Errorf("unable to derive peer identity from certificate (expected one of %v, CN=%q DNSNames=%v)", s.peerNames, cert.Subject.CommonName, cert.DNSNames)
}

func (s *MTLSTransport) handleConnection(conn net.Conn) {
	defer conn.Close()

	peerIndex, err := s.peerPartyIndex(conn)
	if err != nil {
		fmt.Printf("Error identifying peer: %v\n", err)
		return
	}

	if !s.registerPeerConn(peerIndex, conn) {
		return
	}

	for {
		select {
		case <-s.ctx.Done():
			return
		default:
		}

		var length uint32
		if err := binary.Read(conn, binary.BigEndian, &length); err != nil {
			if err != io.EOF && !isExpectedCloseError(err) {
				fmt.Printf("Error reading message length: %v\n", err)
			}
			return
		}
		if length > maxMessageSize {
			fmt.Printf("Error: message too large: %d bytes (max %d)\n", length, maxMessageSize)
			return
		}

		data := make([]byte, length)
		if _, err := io.ReadFull(conn, data); err != nil {
			if !isExpectedCloseError(err) {
				fmt.Printf("Error reading message data: %v\n", err)
			}
			return
		}

		if peerIndex >= 0 && peerIndex < s.partyCount {
			select {
			case s.messageQueues[peerIndex] <- data:
			case <-s.ctx.Done():
				return
			}
		}
	}
}

func (s *MTLSTransport) registerPeerConn(peerIndex int, conn net.Conn) bool {
	s.clientsMu.Lock()
	defer s.clientsMu.Unlock()

	if existing := s.clients[peerIndex]; existing != nil {
		// Prefer the already-registered connection and drop duplicates.
		return existing == conn
	}
	s.clients[peerIndex] = conn
	return true
}

func writeAll(conn net.Conn, data []byte) error {
	for len(data) > 0 {
		n, err := conn.Write(data)
		if err != nil {
			return err
		}
		data = data[n:]
	}
	return nil
}

func isExpectedCloseError(err error) bool {
	return errors.Is(err, net.ErrClosed) || strings.Contains(err.Error(), "use of closed network connection")
}

// Send implements mpc.Transport.
func (s *MTLSTransport) Send(ctx context.Context, toParty int, msg []byte) error {
	if ctx == nil {
		ctx = context.Background()
	}
	if err := ctx.Err(); err != nil {
		return err
	}
	if toParty < 0 || toParty >= s.partyCount || toParty == s.myIndex {
		return fmt.Errorf("invalid target party: %d", toParty)
	}

	s.clientsMu.RLock()
	conn := s.clients[toParty]
	s.clientsMu.RUnlock()
	if conn == nil {
		return fmt.Errorf("no connection to party %d", toParty)
	}
	if len(msg) > maxMessageSize {
		return fmt.Errorf("message too large: %d bytes (max %d)", len(msg), maxMessageSize)
	}

	s.writeMu[toParty].Lock()
	defer s.writeMu[toParty].Unlock()

	if deadline, ok := ctx.Deadline(); ok {
		if err := conn.SetWriteDeadline(deadline); err != nil {
			return fmt.Errorf("failed to set write deadline: %w", err)
		}
		defer func() {
			if err := conn.SetWriteDeadline(time.Time{}); err != nil {
				fmt.Printf("Error clearing write deadline: %v\n", err)
			}
		}()
	}

	// #nosec G115 -- len(msg) is capped at maxMessageSize (64 MiB), well below uint32 max.
	length := uint32(len(msg))
	if err := binary.Write(conn, binary.BigEndian, length); err != nil {
		return fmt.Errorf("failed to send message length: %w", err)
	}
	if err := writeAll(conn, msg); err != nil {
		return fmt.Errorf("failed to send message: %w", err)
	}
	return nil
}

// Receive implements mpc.Transport.
func (s *MTLSTransport) Receive(ctx context.Context, fromParty int) ([]byte, error) {
	if ctx == nil {
		ctx = context.Background()
	}
	if fromParty < 0 || fromParty >= s.partyCount || fromParty == s.myIndex {
		return nil, fmt.Errorf("invalid source party: %d", fromParty)
	}

	select {
	case msg := <-s.messageQueues[fromParty]:
		return msg, nil
	case <-ctx.Done():
		return nil, ctx.Err()
	case <-s.ctx.Done():
		return nil, net.ErrClosed
	}
}

// ReceiveAll implements mpc.Transport.
func (s *MTLSTransport) ReceiveAll(ctx context.Context, fromParties []int) ([][]byte, error) {
	results := make([][]byte, len(fromParties))
	for i, party := range fromParties {
		msg, err := s.Receive(ctx, party)
		if err != nil {
			return nil, err
		}
		results[i] = msg
	}
	return results, nil
}

// MyIndex returns this transport's local party index.
func (s *MTLSTransport) MyIndex() int {
	return s.myIndex
}

// PartyCount returns the total number of parties wired into this transport.
func (s *MTLSTransport) PartyCount() int {
	return s.partyCount
}

// Close implements mpc.Transport.
func (s *MTLSTransport) Close() error {
	s.mu.Lock()
	defer s.mu.Unlock()

	if s.closed {
		return nil
	}
	s.closed = true
	s.cancel()

	if s.listener != nil {
		_ = s.listener.Close()
	}

	s.clientsMu.Lock()
	for _, conn := range s.clients {
		if conn != nil {
			_ = conn.Close()
		}
	}
	s.clientsMu.Unlock()
	return nil
}

// LoadTLSConfig loads TLS configuration from PEM files for local loopback demos.
//
// For non-server callers it fixes ServerName to "localhost", matching the
// generated demo certificates from scripts/certs/generate_certs.sh. Real
// deployments should provide their own TLS configuration and naming model.
func LoadTLSConfig(caCertFile, clientCertFile, clientKeyFile string, isServer bool) (*tls.Config, error) {
	caCert, err := os.ReadFile(caCertFile)
	if err != nil {
		return nil, fmt.Errorf("failed to read CA certificate: %w", err)
	}

	caCertPool := x509.NewCertPool()
	if !caCertPool.AppendCertsFromPEM(caCert) {
		return nil, fmt.Errorf("failed to parse CA certificate")
	}

	clientCert, err := tls.LoadX509KeyPair(clientCertFile, clientKeyFile)
	if err != nil {
		return nil, fmt.Errorf("failed to load client certificate: %w", err)
	}

	config := &tls.Config{
		Certificates: []tls.Certificate{clientCert},
		ClientAuth:   tls.RequireAndVerifyClientCert,
		ClientCAs:    caCertPool,
		RootCAs:      caCertPool,
		MinVersion:   tls.VersionTLS12,
	}
	if !isServer {
		config.ServerName = "localhost"
	}
	return config, nil
}
