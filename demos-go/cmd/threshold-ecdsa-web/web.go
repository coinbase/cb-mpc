package main

import (
	"encoding/base64"
	"fmt"
	"html/template"
	"log"
	"net/http"
	"sort"
	"strconv"
	"strings"
	"sync"
	"time"

	"slices"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/transport/mtls"
	"github.com/labstack/echo/v4"
)

type MockDB struct {
	dkg     *DKGCoordinationState
	signing *SigningCoordinationState
}

// Global DKG coordination state

type DKGCoordinationState struct {
	mutex           sync.RWMutex
	initiated       bool
	threshold       int
	initiatedAt     time.Time
	completionFlags map[int]bool
}

// Global Signing coordination state

type SigningCoordinationState struct {
	mutex           sync.RWMutex
	initiated       bool
	threshold       int
	selectedParties []int
	message         string
	initiatedAt     time.Time
	completionFlags map[int]bool
}

type PageData struct {
	Title   string
	Parties []PartyConfig
}

type SigningPageData struct {
	Title           string
	Parties         []PartyConfig
	CurrentParty    int
	Threshold       int
	MaxOtherParties int
}

type ConnectionData struct {
	ConnectionTime string
	MaxThreshold   int
	Party0BaseUrl  string
}

type SigningConnectionData struct {
	ConnectionTime   string
	TotalParties     int
	Threshold        int
	ConnectedParties []int
	Party0BaseUrl    string
}

type SigningWaitingData struct {
	ConnectionTime string
	Party0BaseUrl  string
	AllParties     []PartyConfig
	CurrentParty   int
}

type SigningCoordinationData struct {
	Initiated         bool
	Threshold         int
	SelectedParties   []int
	Message           string
	ShouldParticipate bool
}

type DKGResultData struct {
	IsParty0       bool
	ConnectionTime string
	XShare         string
	PemKey         string
}

type SigningResultData struct {
	ConnectionTime  string
	Message         string
	SignatureBase64 string
	PublicKey       string
	PartyIndex      int
}

type ErrorData struct {
	Message string
}

var templates *template.Template

var db = &MockDB{
	dkg: &DKGCoordinationState{
		completionFlags: make(map[int]bool),
	},
	signing: &SigningCoordinationState{
		completionFlags: make(map[int]bool),
	},
}

func (d *DKGCoordinationState) Set(initiated bool, threshold int, initiatedAt time.Time) {
	d.mutex.Lock()
	d.initiated = initiated
	d.threshold = threshold
	d.initiatedAt = initiatedAt
	d.completionFlags = make(map[int]bool)
	d.mutex.Unlock()
}

func (s *SigningCoordinationState) Set(initiated bool, threshold int, selectedParties []int, message string, initiatedAt time.Time) {
	s.mutex.Lock()
	s.initiated = initiated
	s.threshold = threshold
	s.selectedParties = selectedParties
	s.message = message
	s.initiatedAt = initiatedAt
	s.completionFlags = make(map[int]bool)
	s.mutex.Unlock()
}

func (d *DKGCoordinationState) SetCompletionFlag(partyIndex int, completed bool) {
	d.mutex.Lock()
	d.completionFlags[partyIndex] = completed
	d.mutex.Unlock()
}

func (s *SigningCoordinationState) SetCompletionFlag(partyIndex int, completed bool) {
	s.mutex.Lock()
	s.completionFlags[partyIndex] = completed
	s.mutex.Unlock()
}

func closeExistingConnections(dkgTransport transport.Messenger, signingTransport transport.Messenger) {
	fmt.Printf("Closing existing connections...\n")

	hadConnections := false

	if dkgTransport != nil {
		hadConnections = true
		if err := dkgTransport.(*mtls.MTLSMessenger).Close(); err != nil {
			fmt.Printf("Error closing DKG transport: %v\n", err)
		}
		dkgTransport = nil
	}

	if signingTransport != nil {
		hadConnections = true
		if err := signingTransport.(*mtls.MTLSMessenger).Close(); err != nil {
			fmt.Printf("Error closing signing transport: %v\n", err)
		}
		signingTransport = nil
	}

	if hadConnections {
		fmt.Printf("Waiting briefly for ports to be released...\n")
		time.Sleep(100 * time.Millisecond)
	}

	fmt.Printf("Connection cleanup completed\n")
}

func loadTemplates() error {
	var err error
	templates, err = template.ParseGlob("templates/*.html")
	if err != nil {
		return fmt.Errorf("failed to parse templates: %v", err)
	}
	return nil
}

func renderTemplate(name string, data interface{}) (string, error) {
	var buf strings.Builder
	err := templates.ExecuteTemplate(&buf, name, data)
	if err != nil {
		return "", fmt.Errorf("failed to execute template %s: %v", name, err)
	}
	return buf.String(), nil
}

func renderError(message string) (string, error) {
	return renderTemplate("error.html", ErrorData{Message: message})
}

func dkg(dkgPartyIndex int, dkgPartyName string, dkgAllPNames []string, dkgTransport transport.Messenger, threshold int, curve curve.Curve) (time.Duration, []byte, []byte, error) {
	ac := createThresholdAccessStructure(dkgAllPNames, threshold, curve)

	startTime := time.Now()
	keyShare, err := runThresholdDKG(dkgPartyIndex, threshold, dkgAllPNames, &ac, curve, dkgTransport)
	if err != nil {
		return 0, nil, nil, fmt.Errorf("threshold DKG failed: %v", err)
	}
	duration := time.Since(startTime)

	if err := saveKeyShare(keyShare, dkgPartyName); err != nil {
		return 0, nil, nil, fmt.Errorf("saving key share: %v", err)
	}

	if err := saveThreshold(threshold); err != nil {
		return 0, nil, nil, fmt.Errorf("saving threshold: %v", err)
	}

	pemKey, err := createPEMPublicKey(keyShare)
	if err != nil {
		return 0, nil, nil, fmt.Errorf("creating PEM public key: %v", err)
	}
	xShare, err := keyShare.XShare()
	if err != nil {
		return 0, nil, nil, fmt.Errorf("extracting public key: %v", err)
	}
	return duration, pemKey, []byte(xShare.String()), nil
}

func main_web(runConfig *RunConfig) error {
	e := echo.New()

	// Enable CORS to allow cross-origin requests between party instances
	e.Use(func(next echo.HandlerFunc) echo.HandlerFunc {
		return func(c echo.Context) error {
			c.Response().Header().Set("Access-Control-Allow-Origin", "*")
			c.Response().Header().Set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE")
			c.Response().Header().Set("Access-Control-Allow-Headers", "*")
			return next(c)
		}
	})

	// These will be populated in one endpoint call and used in another.
	// Proper implementation would use a session to store most of them in db.
	var err error
	var dkgTransport transport.Messenger
	var dkgPartyIndex int
	var dkgPartyName string
	var dkgAllPNames []string

	var signingTransport transport.Messenger
	var signingPartyIndex int
	var signingPartyName string
	var signingAllPNames []string
	var signingParticipantPNames []string

	curve, err := curve.NewSecp256k1()
	if err != nil {
		return fmt.Errorf("creating curve: %v", err)
	}

	defer func() {
		closeExistingConnections(dkgTransport, signingTransport)
	}()

	if err := loadTemplates(); err != nil {
		return fmt.Errorf("failed to load templates: %v", err)
	}

	// The initial dkg page

	e.GET("/page/dkg", func(c echo.Context) error {
		// Clean up any existing connections when loading the DKG page
		closeExistingConnections(dkgTransport, signingTransport)

		db.dkg.Set(false, 0, time.Time{})
		// removeKeyShares()

		dkgPartyIndex = 0
		dkgPartyName = ""
		dkgAllPNames = nil

		signingPartyIndex = 0
		signingPartyName = ""
		signingAllPNames = nil
		signingParticipantPNames = nil

		pageData := PageData{
			Title:   "Threshold DKG",
			Parties: runConfig.Config.Parties,
		}

		html, err := renderTemplate("dkg_base.html", pageData)
		if err != nil {
			log.Printf("Error rendering template: %v", err)
			return c.String(http.StatusInternalServerError, "Template error")
		}

		return c.HTML(http.StatusOK, html)
	})

	// HTMX endpoint to process connect request from the dkg page

	e.GET("/api/dkg/connect", func(c echo.Context) error {
		startTime := time.Now()

		dkgPartyIndex, dkgPartyName, dkgAllPNames, _, dkgTransport, err = setupTransport(runConfig.Config, runConfig.MyIndex, runConfig.ParticipantsIndices)
		if err != nil {
			return fmt.Errorf("setting up transport: %v", err)
		}

		duration := time.Since(startTime)

		connectionData := ConnectionData{
			ConnectionTime: duration.Round(time.Millisecond).String(),
			MaxThreshold:   len(dkgAllPNames),
			Party0BaseUrl:  "http://127.0.0.1:7080",
		}

		// Party 0 gets the control interface, others get waiting interface
		var templateName string
		if runConfig.MyIndex == 0 {
			templateName = "dkg_connection_success.html"
		} else {
			templateName = "dkg_connection_waiting.html"
		}

		html, err := renderTemplate(templateName, connectionData)
		if err != nil {
			log.Printf("Error rendering connection template: %v", err)
			errorHtml, _ := renderError("Template rendering failed")
			return c.HTML(http.StatusInternalServerError, errorHtml)
		}

		return c.HTML(http.StatusOK, html)
	})

	// HTMX endpoint to process execute request from the dkg page

	e.GET("/api/dkg/execute", func(c echo.Context) error {
		// Only party 0 should be able to call this endpoint directly
		if runConfig.MyIndex != 0 {
			errorHtml, _ := renderError("Only party 0 can initiate DKG")
			return c.HTML(http.StatusForbidden, errorHtml)
		}

		threshold := c.QueryParam("threshold")
		if threshold == "" {
			errorHtml, _ := renderError("Threshold parameter is required")
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		thresholdInt, err := strconv.Atoi(threshold)
		if err != nil {
			errorHtml, _ := renderError("Invalid threshold value")
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		if thresholdInt < 1 || thresholdInt > len(dkgAllPNames) {
			errorHtml, _ := renderError(fmt.Sprintf("Threshold must be between 1 and %d", len(dkgAllPNames)))
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		db.dkg.Set(true, thresholdInt, time.Now())

		duration, pemKey, xShare, err := dkg(dkgPartyIndex, dkgPartyName, dkgAllPNames, dkgTransport, thresholdInt, curve)
		if err != nil {
			return fmt.Errorf("dkg failed: %v", err)
		}

		db.dkg.SetCompletionFlag(runConfig.MyIndex, true)

		resultData := DKGResultData{
			IsParty0:       runConfig.MyIndex == 0,
			ConnectionTime: duration.Round(time.Millisecond).String(),
			XShare:         base64.StdEncoding.EncodeToString(xShare),
			PemKey:         string(pemKey),
		}

		html, err := renderTemplate("dkg_result.html", resultData)
		if err != nil {
			log.Printf("Error rendering DKG result template: %v", err)
			errorHtml, _ := renderError("Template rendering failed")
			return c.HTML(http.StatusInternalServerError, errorHtml)
		}

		return c.HTML(http.StatusOK, html)
	})

	// Polling endpoint for non-party-0 participants to check if DKG has been initiated

	e.GET("/api/dkg/poll", func(c echo.Context) error {
		db.dkg.mutex.RLock()
		initiated := db.dkg.initiated
		threshold := db.dkg.threshold
		db.dkg.mutex.RUnlock()

		if initiated {
			return c.JSON(http.StatusOK, map[string]interface{}{
				"initiated": true,
				"threshold": threshold,
			})
		}

		return c.JSON(http.StatusOK, map[string]interface{}{
			"initiated": false,
		})
	})

	// Auto-execute endpoint for non-party-0 participants

	e.GET("/api/dkg/auto-execute", func(c echo.Context) error {
		// Only non-party-0 participants should use this endpoint
		if runConfig.MyIndex == 0 {
			errorHtml, _ := renderError("Party 0 should use the regular execute endpoint")
			return c.HTML(http.StatusForbidden, errorHtml)
		}

		// Get threshold from query parameter since we already verified DKG initiation via polling
		thresholdStr := c.QueryParam("threshold")
		if thresholdStr == "" {
			errorHtml, _ := renderError("Threshold parameter is required")
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		threshold, err := strconv.Atoi(thresholdStr)
		if err != nil {
			errorHtml, _ := renderError("Invalid threshold value")
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		duration, pemKey, xShare, err := dkg(dkgPartyIndex, dkgPartyName, dkgAllPNames, dkgTransport, threshold, curve)
		if err != nil {
			return fmt.Errorf("dkg failed: %v", err)
		}

		resultData := DKGResultData{
			IsParty0:       runConfig.MyIndex == 0,
			ConnectionTime: duration.Round(time.Millisecond).String(),
			XShare:         base64.StdEncoding.EncodeToString(xShare),
			PemKey:         string(pemKey),
		}

		html, err := renderTemplate("dkg_result.html", resultData)
		if err != nil {
			log.Printf("Error rendering DKG result template: %v", err)
			errorHtml, _ := renderError("Template rendering failed")
			return c.HTML(http.StatusInternalServerError, errorHtml)
		}

		return c.HTML(http.StatusOK, html)
	})

	// The initial signing page

	e.GET("/page/sign", func(c echo.Context) error {
		// Clean up any existing connections when loading the signing page
		closeExistingConnections(dkgTransport, signingTransport)

		db.signing.Set(false, 0, nil, "", time.Time{})

		dkgPartyIndex = 0
		dkgPartyName = ""
		dkgAllPNames = nil

		signingPartyIndex = 0
		signingPartyName = ""
		signingAllPNames = nil
		signingParticipantPNames = nil

		// Party 0 gets immediate configuration interface, others get waiting interface
		if runConfig.MyIndex == 0 {
			threshold, err := loadThreshold()
			if err != nil {
				return fmt.Errorf("loading threshold: %v", err)
			}

			signingPageData := SigningPageData{
				Title:           "Threshold Signing",
				Parties:         runConfig.Config.Parties,
				CurrentParty:    runConfig.MyIndex,
				Threshold:       threshold,
				MaxOtherParties: threshold - 1,
			}

			html, err := renderTemplate("signing_leader_interface.html", signingPageData)
			if err != nil {
				log.Printf("Error rendering signing leader template: %v", err)
				return c.String(http.StatusInternalServerError, "Template error")
			}
			return c.HTML(http.StatusOK, html)
		} else {
			// Other parties get immediate waiting interface
			waitingData := SigningWaitingData{
				ConnectionTime: "Ready",
				Party0BaseUrl:  "http://127.0.0.1:7080",
				AllParties:     runConfig.Config.Parties,
				CurrentParty:   runConfig.MyIndex,
			}

			html, err := renderTemplate("signing_immediate_waiting.html", waitingData)
			if err != nil {
				log.Printf("Error rendering signing waiting template: %v", err)
				return c.String(http.StatusInternalServerError, "Template error")
			}
			return c.HTML(http.StatusOK, html)
		}
	})

	// HTMX endpoint to process execute request from the signing page

	e.GET("/api/sign/execute", func(c echo.Context) error {
		// Only party 0 should be able to call this endpoint directly
		if runConfig.MyIndex != 0 {
			errorHtml, _ := renderError("Only party 0 can initiate signing")
			return c.HTML(http.StatusForbidden, errorHtml)
		}

		selectedParties := c.QueryParams()["parties"]
		thresholdStr := c.QueryParam("threshold")
		message := c.QueryParam("message")

		if thresholdStr == "" {
			errorHtml, _ := renderError("Threshold parameter is required")
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		if message == "" {
			errorHtml, _ := renderError("Message parameter is required")
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		threshold, err := strconv.Atoi(thresholdStr)
		if err != nil {
			errorHtml, _ := renderError("Invalid threshold value")
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		// Always include party 0 in the selected parties
		selectedParties = append(selectedParties, "0")
		if len(selectedParties) != threshold {
			errorHtml, _ := renderError(fmt.Sprintf("Must select exactly %d parties, got %d", threshold, len(selectedParties)))
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		// Convert selected parties to integers and create participants map
		selectedPartyInts := make([]int, 0)
		signingParticipantsIndices := make(map[int]bool)

		for _, partyStr := range selectedParties {
			partyIdx, err := strconv.Atoi(partyStr)
			if err != nil {
				errorHtml, _ := renderError(fmt.Sprintf("Invalid party index: %s", partyStr))
				return c.HTML(http.StatusBadRequest, errorHtml)
			}
			selectedPartyInts = append(selectedPartyInts, partyIdx)
			signingParticipantsIndices[partyIdx] = true
		}
		sort.Ints(selectedPartyInts)

		db.signing.Set(true, threshold, selectedPartyInts, message, time.Now())
		db.signing.selectedParties = selectedPartyInts

		signingPartyIndex, signingPartyName, signingAllPNames, signingParticipantPNames, signingTransport, err = setupTransport(runConfig.Config, runConfig.MyIndex, signingParticipantsIndices)
		if err != nil {
			return fmt.Errorf("setting up transport for signing: %v", err)
		}

		// Execute signing for party 0
		keyShare, err := loadKeyShare(signingPartyName)
		if err != nil {
			return fmt.Errorf("loading key share: %v", err)
		}

		ac := createThresholdAccessStructure(signingAllPNames, threshold, curve)

		startTime := time.Now()
		signature, publicKey, err := runThresholdSign(keyShare, &ac, signingPartyIndex, threshold, signingParticipantPNames, []byte(message), signingTransport)
		if err != nil {
			log.Printf("Threshold signing failed: %v", err)
			errorHtml, _ := renderError(fmt.Sprintf("Signing failed: %v", err))
			return c.HTML(http.StatusInternalServerError, errorHtml)
		}
		duration := time.Since(startTime)

		db.signing.SetCompletionFlag(runConfig.MyIndex, true)

		resultData := SigningResultData{
			ConnectionTime:  duration.Round(time.Millisecond).String(),
			Message:         message,
			SignatureBase64: base64.StdEncoding.EncodeToString(signature),
			PublicKey:       string(publicKey),
			PartyIndex:      signingPartyIndex,
		}

		html, err := renderTemplate("signing_result.html", resultData)
		if err != nil {
			log.Printf("Error rendering signing result template: %v", err)
			errorHtml, _ := renderError("Template rendering failed")
			return c.HTML(http.StatusInternalServerError, errorHtml)
		}

		return c.HTML(http.StatusOK, html)
	})

	// Polling endpoint for non-party-0 participants to check if signing has been initiated
	e.GET("/api/sign/poll", func(c echo.Context) error {
		db.signing.mutex.RLock()
		initiated := db.signing.initiated
		threshold := db.signing.threshold
		selectedParties := db.signing.selectedParties
		message := db.signing.message
		db.signing.mutex.RUnlock()

		counterParty := c.QueryParam("party")
		counterPartyInt, err := strconv.Atoi(counterParty)
		if err != nil {
			errorHtml, _ := renderError("Invalid counter party value")
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		if initiated {
			shouldParticipate := slices.Contains(selectedParties, counterPartyInt)
			return c.JSON(http.StatusOK, map[string]interface{}{
				"initiated":         true,
				"threshold":         threshold,
				"selectedParties":   selectedParties,
				"message":           message,
				"shouldParticipate": shouldParticipate,
				"party":             counterPartyInt,
			})
		}

		return c.JSON(http.StatusOK, map[string]interface{}{
			"initiated": false,
		})
	})

	// Auto-execute endpoint for non-party-0 participants

	e.GET("/api/sign/auto-execute", func(c echo.Context) error {
		// Only non-party-0 participants should use this endpoint
		if runConfig.MyIndex == 0 {
			errorHtml, _ := renderError("Party 0 should use the regular execute endpoint")
			return c.HTML(http.StatusForbidden, errorHtml)
		}

		// Get parameters from query since we already verified signing initiation via polling
		thresholdStr := c.QueryParam("threshold")
		selectedPartiesStr := c.QueryParam("selectedParties")
		message := c.QueryParam("message")

		if thresholdStr == "" || selectedPartiesStr == "" || message == "" {
			errorHtml, _ := renderError("Missing required parameters")
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		threshold, err := strconv.Atoi(thresholdStr)
		if err != nil {
			errorHtml, _ := renderError("Invalid threshold value")
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		// Parse selected parties
		var selectedParties []int
		partyStrs := strings.Split(selectedPartiesStr, ",")
		signingParticipantsIndices := make(map[int]bool)

		for _, partyStr := range partyStrs {
			if strings.TrimSpace(partyStr) == "" {
				continue
			}
			partyIdx, err := strconv.Atoi(strings.TrimSpace(partyStr))
			if err != nil {
				errorHtml, _ := renderError(fmt.Sprintf("Invalid party index: %s", partyStr))
				return c.HTML(http.StatusBadRequest, errorHtml)
			}
			selectedParties = append(selectedParties, partyIdx)
			signingParticipantsIndices[partyIdx] = true
		}

		counterParty := c.QueryParam("party")
		counterPartyInt, err := strconv.Atoi(counterParty)
		if err != nil {
			errorHtml, _ := renderError("Invalid counter party value")
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		if !slices.Contains(selectedParties, counterPartyInt) {
			errorHtml, _ := renderError(fmt.Sprintf("Party %d is not in the selected parties list", counterPartyInt))
			return c.HTML(http.StatusBadRequest, errorHtml)
		}

		signingPartyIndex, signingPartyName, signingAllPNames, signingParticipantPNames, signingTransport, err = setupTransport(runConfig.Config, runConfig.MyIndex, signingParticipantsIndices)
		if err != nil {
			return fmt.Errorf("setting up transport for auto-signing: %v", err)
		}

		keyShare, err := loadKeyShare(signingPartyName)
		if err != nil {
			return fmt.Errorf("loading key share: %v", err)
		}

		ac := createThresholdAccessStructure(signingAllPNames, threshold, curve)

		startTime := time.Now()
		signature, publicKey, err := runThresholdSign(keyShare, &ac, signingPartyIndex, threshold, signingParticipantPNames, []byte(message), signingTransport)
		if err != nil {
			log.Printf("Auto threshold signing failed: %v", err)
			errorHtml, _ := renderError(fmt.Sprintf("Auto-signing failed: %v", err))
			return c.HTML(http.StatusInternalServerError, errorHtml)
		}
		duration := time.Since(startTime)

		db.signing.SetCompletionFlag(runConfig.MyIndex, true)

		resultData := SigningResultData{
			ConnectionTime:  duration.Round(time.Millisecond).String(),
			Message:         message,
			SignatureBase64: base64.StdEncoding.EncodeToString(signature),
			PublicKey:       string(publicKey),
			PartyIndex:      signingPartyIndex,
		}

		html, err := renderTemplate("signing_result.html", resultData)
		if err != nil {
			log.Printf("Error rendering signing result template: %v", err)
			errorHtml, _ := renderError("Template rendering failed")
			return c.HTML(http.StatusInternalServerError, errorHtml)
		}

		return c.HTML(http.StatusOK, html)
	})

	e.Logger.Fatal(e.Start(runConfig.Config.WebAddress))
	return nil
}
