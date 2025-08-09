package main

import (
	"encoding/json"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"strings"

	"gopkg.in/yaml.v3"
)

var timeUnitDisplay = map[string]string{
	"ns": "ns",
	"us": "μs",
	"ms": "ms",
	"s":  "s",
}

var spaceUnitDisplay = map[string]string{
	"B":  "Bytes",
	"KB": "KB",
	"MB": "MB",
	"GB": "GB",
}

var bidirSpaceUnitDisplay = map[string]string{
	"2B":  "Bytes",
	"2KB": "KB",
	"2MB": "MB",
	"2GB": "GB",
}

type ContextData struct {
	Date              string `json:"date"`
	HostName          string `json:"host_name"`
	Executable        string `json:"executable"`
	NumCpus           int    `json:"num_cpus"`
	MhzPerCpu         int    `json:"mhz_per_cpu"`
	CpuScalingEnabled bool   `json:"cpu_scaling_enabled"`
	CommitSha         string // this is read from a different source
}

type BenchmarkData struct {
	Name       string  `json:"name"`
	CpuTime    float64 `json:"cpu_time"`
	TimeUnit   string  `json:"time_unit"`
	Iterations int     `json:"iterations"`
	Size       float64 `json:"size"`
	SSize      float64 `json:"r_size"`
	RSize      float64 `json:"s_size"`
	RealTime   float64 `json:"real_time"`
}

func (b *BenchmarkData) toTooltip() Tooltip {
	return Tooltip{
		Name:       b.Name,
		RealTime:   fmt.Sprintf("%.2f", b.RealTime),
		TimeUnit:   b.TimeUnit,
		Iterations: fmt.Sprintf("%d", b.Iterations),
	}
}

type Tooltip struct {
	Name       string
	RealTime   string
	TimeUnit   string
	Iterations string
}

type Cell struct {
	ValueType string `yaml:"value_type"`
	Source    string `yaml:"source"`
	Tooltip   Tooltip
	Value     string `yaml:"value"`
}

type Row struct {
	Name  string `yaml:"name"`
	Cells []Cell `yaml:"cells"`
}

type Table struct {
	Name          string   `yaml:"name"`
	HeaderStrings []string `yaml:"headerStrings"`
	Rows          []Row    `yaml:"rows"`
}

type Table2PC struct {
	Name          string `yaml:"name"`
	P1Total       Cell   `yaml:"p1_total"`
	P2Total       Cell   `yaml:"p2_total"`
	P1Times       []Cell `yaml:"p1_times"`
	P2Times       []Cell `yaml:"p2_times"`
	Msgs          []Cell `yaml:"msgs"`
	P1OutputRound Cell   `yaml:"p1_output_round"`
	P2OutputRound Cell   `yaml:"p2_output_round"`
}

type TableMPC struct {
	Name        string   `yaml:"name"`
	NParties    int      `yaml:"n_parties"`
	TotalTimes  []Cell   `yaml:"total_time"`
	RoundTimes  [][]Cell `yaml:"round_times"`
	Msgs        [][]Cell `yaml:"msgs"`
	OutputRound []Cell   `yaml:"output_round"`
}

// ====================================================
// Expanded Approach: define expansions that generate Tables
// ====================================================
type BenchmarkExpansion struct {
	Name                string   `yaml:"name"`
	KeyPrefix           string   `yaml:"key_prefix"`
	HeaderRowStrings    []string `yaml:"header_row_strings"`
	ColumnKeys          []string `yaml:"column_keys"`
	HeaderColumnStrings []string `yaml:"header_column_strings"`
	RowKeys             []string `yaml:"row_keys"`
	RowKeys2            []string `yaml:"row_keys2"`
	Unit                string   `yaml:"unit"`
}

type BenchmarkExpansion2PC struct {
	Name         string   `yaml:"name"`
	ProtocolName string   `yaml:"protocol_name"`
	NRounds      int      `yaml:"n_rounds"`
	Unit         string   `yaml:"unit"`
	MsgUnit      string   `yaml:"msg_unit"`
	Variants     []string `yaml:"variants"`
	VariantNames []string `yaml:"variant_names"`
}

type BenchmarkExpansionMPC struct {
	Name         string   `yaml:"name"`
	ProtocolName string   `yaml:"protocol_name"`
	NRounds      int      `yaml:"n_rounds"`
	NParties     int      `yaml:"n_parties"`
	Unit         string   `yaml:"unit"`
	MsgUnit      string   `yaml:"msg_unit"`
	Variants     []string `yaml:"variants"`
	VariantNames []string `yaml:"variant_names"`
}

// Now we extend the Benchmark struct with expansions:
type Benchmark struct {
	Category      string                  `yaml:"category"`
	Tables        []Table                 `yaml:"tables"` // old manual tables
	Table2PCs     []Table2PC              `yaml:"table2pcs"`
	TableMPCs     []TableMPC              `yaml:"tablempcs"`
	Expansions    []BenchmarkExpansion    `yaml:"expansions"`
	Expansions2PC []BenchmarkExpansion2PC `yaml:"expansions2pc"`
	ExpansionsMPC []BenchmarkExpansionMPC `yaml:"expansionsmpcs"`
}

type HomepageContent struct {
	Context    ContextData
	Benchmarks []Benchmark `yaml:"benchmarks"`
}

// LoadBenchmarks parses the JSON file with all aggregated data.
func LoadBenchmarks(path string) (*AllBenchmarksData, error) {
	content, err := os.ReadFile(path)
	if err != nil {
		return nil, err
	}
	var data AllBenchmarksData
	err = json.Unmarshal(content, &data)
	if err != nil {
		return nil, err
	}
	return &data, nil
}

// ====================================================
// Parsing bench data into cells
// ====================================================

func processCell(allBenches map[string]BenchmarkData, cell *Cell) *BenchmarkData {
	if cell.Source == "" {
		return nil
	}
	if rawBench, exists := allBenches[cell.Source]; exists {
		rowBenchToCell(&rawBench, cell)
		return &rawBench
	} else if cell.ValueType == "string" {
		rowBenchToCell(nil, cell)
	} else {
		log.Printf("bench key %s does not exist", cell.Source)
	}
	return nil
}

func ParseBenchmarkData(path string, pageContent *HomepageContent, allBenches map[string]BenchmarkData) error {
	// For each table in each benchmark...
	for _, benchmark := range pageContent.Benchmarks {
		for tableIndex := range benchmark.Tables {
			for rowIndex := range benchmark.Tables[tableIndex].Rows {
				for cellIndex := range benchmark.Tables[tableIndex].Rows[rowIndex].Cells {
					cell := &benchmark.Tables[tableIndex].Rows[rowIndex].Cells[cellIndex]
					if cell.Source == "" {
						continue
					}
					if rawBench, exists := parseSource(allBenches, cell.Source); exists {
						rowBenchToCell(&rawBench, cell)
					} else if cell.ValueType == "string" {
						// string means we just display cell.Source literally
						rowBenchToCell(nil, cell)
					} else {
						log.Printf("bench key %s does not exist", cell.Source)
					}
				}
			}
		}
	}
	return nil
}

//
// ====================================================
// Expansions -> Tables
// ====================================================

// expandAllBenchmarks loops over each Benchmark and expands any "expansions" into normal Tables.
func expandAllBenchmarks(pageContent *HomepageContent, allBenches map[string]BenchmarkData) {
	for i := range pageContent.Benchmarks {
		expandBenchmarkExpansions(&pageContent.Benchmarks[i], allBenches)
	}
}

// expandBenchmarkExpansions expands one Benchmark's expansions into actual Table objects.
func expandBenchmarkExpansions(b *Benchmark, allBenches map[string]BenchmarkData) {
	for _, exp2pc := range b.Expansions2PC {
		if len(exp2pc.Variants) == 0 {
			exp2pc.Variants = []string{""}
			exp2pc.VariantNames = []string{exp2pc.Name}
		} else {
			if len(exp2pc.VariantNames) != len(exp2pc.Variants) {
				panic(fmt.Sprintf("variant names length %d != variants length %d", len(exp2pc.VariantNames), len(exp2pc.Variants)))
			}
			for i := range exp2pc.VariantNames {
				exp2pc.VariantNames[i] = exp2pc.Name + " - " + exp2pc.VariantNames[i]
			}
		}
		for varI := range exp2pc.Variants {
			log.Printf("expanding 2PC benchmark \"%s\"", exp2pc.VariantNames[varI])
			nRounds := exp2pc.NRounds
			t := Table2PC{
				Name:          exp2pc.VariantNames[varI],
				P1Total:       Cell{Value: "P1 Total"},
				P2Total:       Cell{Value: "P2 Total"},
				P1Times:       make([]Cell, nRounds),
				P2Times:       make([]Cell, nRounds),
				Msgs:          make([]Cell, nRounds),
				P1OutputRound: Cell{Source: fmt.Sprintf("%s/%d/1%s", exp2pc.ProtocolName, nRounds+1, exp2pc.Variants[varI]), ValueType: exp2pc.Unit},
				P2OutputRound: Cell{Source: fmt.Sprintf("%s/%d/2%s", exp2pc.ProtocolName, nRounds+1, exp2pc.Variants[varI]), ValueType: exp2pc.Unit},
			}
			p1OutputRoundBench := processCell(allBenches, &t.P1OutputRound)
			p2OutputRoundBench := processCell(allBenches, &t.P2OutputRound)
			p1Total := 0.0
			p2Total := 0.0
			rawUnit := ""
			if p1OutputRoundBench != nil {
				p1Total = p1OutputRoundBench.RealTime
				rawUnit = p1OutputRoundBench.TimeUnit
			}
			if p2OutputRoundBench != nil {
				p2Total = p2OutputRoundBench.RealTime
				rawUnit = p2OutputRoundBench.TimeUnit
			}
			for i := 1; i <= nRounds; i++ {
				t.P1Times[i-1] = Cell{Source: fmt.Sprintf("%s/%d/1%s", exp2pc.ProtocolName, i, exp2pc.Variants[varI]), ValueType: exp2pc.Unit}
				t.P2Times[i-1] = Cell{Source: fmt.Sprintf("%s/%d/2%s", exp2pc.ProtocolName, i, exp2pc.Variants[varI]), ValueType: exp2pc.Unit}
				t.Msgs[i-1] = Cell{Source: fmt.Sprintf("%s/%d/%d%s", exp2pc.ProtocolName, i+1, i%2+1, exp2pc.Variants[varI]), ValueType: exp2pc.MsgUnit}
				p1bench := processCell(allBenches, &t.P1Times[i-1])
				p2bench := processCell(allBenches, &t.P2Times[i-1])
				processCell(allBenches, &t.Msgs[i-1])
				if i%2 == 0 {
					t.Msgs[i-1].Value = "←" + t.Msgs[i-1].Value
				} else {
					t.Msgs[i-1].Value = "→" + t.Msgs[i-1].Value
				}
				if p1bench != nil {
					p1Total += p1bench.RealTime
				}
				if p2bench != nil {
					p2Total += p2bench.RealTime
				}
			}
			convertedP1Total, err := convertTime(rawUnit, exp2pc.Unit, p1Total)
			if err != nil {
				panic(err)
			}
			convertedP2Total, err := convertTime(rawUnit, exp2pc.Unit, p2Total)
			if err != nil {
				panic(err)
			}
			t.P1Total.Value = fmt.Sprintf("%.2f %s", convertedP1Total, timeUnitDisplay[exp2pc.Unit])
			t.P2Total.Value = fmt.Sprintf("%.2f %s", convertedP2Total, timeUnitDisplay[exp2pc.Unit])

			b.Table2PCs = append(b.Table2PCs, t)
		}
	}
	for _, exp := range b.ExpansionsMPC {
		if len(exp.Variants) == 0 {
			exp.Variants = []string{""}
			exp.VariantNames = []string{exp.Name}
		} else {
			if len(exp.VariantNames) != len(exp.Variants) {
				panic(fmt.Sprintf("variant names length %d != variants length %d", len(exp.VariantNames), len(exp.Variants)))
			}
			for i := range exp.VariantNames {
				exp.VariantNames[i] = exp.Name + " - " + exp.VariantNames[i]
			}
		}
		for varI := range exp.Variants {
			log.Printf("expanding MPC benchmark \"%s\"", exp.VariantNames[varI])
			nParties := exp.NParties
			nRounds := exp.NRounds
			t := TableMPC{
				Name:        exp.VariantNames[varI],
				NParties:    nParties,
				TotalTimes:  make([]Cell, nParties),
				RoundTimes:  make([][]Cell, nRounds),
				Msgs:        make([][]Cell, nRounds),
				OutputRound: make([]Cell, nParties),
			}
			totalTimes := make([]float64, nParties)
			var rawUnit string
			for partyI := range t.OutputRound {
				t.OutputRound[partyI] = Cell{Source: fmt.Sprintf("%s/%d/%d%s", exp.ProtocolName, nRounds+1, partyI, exp.Variants[varI]), ValueType: exp.Unit}
				bench := processCell(allBenches, &t.OutputRound[partyI])
				if bench != nil {
					totalTimes[partyI] = bench.RealTime
					rawUnit = bench.TimeUnit
				}
			}
			for i := 0; i < nRounds; i++ {
				t.RoundTimes[i] = make([]Cell, nParties)
				t.Msgs[i] = make([]Cell, nParties)
				for partyI := range t.RoundTimes[i] {
					t.RoundTimes[i][partyI] = Cell{Source: fmt.Sprintf("%s/%d/%d%s", exp.ProtocolName, i+1, partyI, exp.Variants[varI]), ValueType: exp.Unit}
					t.Msgs[i][partyI] = Cell{Source: fmt.Sprintf("%s/%d/%d%s", exp.ProtocolName, i+2, partyI, exp.Variants[varI]), ValueType: exp.MsgUnit}
					bench := processCell(allBenches, &t.RoundTimes[i][partyI])
					if bench != nil {
						totalTimes[partyI] += bench.RealTime
					}
					processCell(allBenches, &t.Msgs[i][partyI])
				}
			}
			convertedTotalTimes := make([]float64, nParties)
			for i := range totalTimes {
				convVal, err := convertTime(rawUnit, exp.Unit, totalTimes[i])
				if err != nil {
					panic(err)
				}
				convertedTotalTimes[i] = convVal
			}
			for i := range convertedTotalTimes {
				t.TotalTimes[i] = Cell{Value: fmt.Sprintf("%.2f %s", convertedTotalTimes[i], timeUnitDisplay[exp.Unit])}
			}

			b.TableMPCs = append(b.TableMPCs, t)
		}
	}

	for _, exp := range b.Expansions {
		log.Printf("expanding benchmark \"%s\"", exp.Name)
		nColumns := len(exp.ColumnKeys)
		nRows := len(exp.RowKeys)
		// Check that the header row strings and column keys are consistent
		if len(exp.HeaderRowStrings) != len(exp.ColumnKeys)+1 {
			panic(fmt.Sprintf("header row strings length %d != column keys length %d + 1", len(exp.HeaderRowStrings), len(exp.ColumnKeys)))
		}
		if len(exp.HeaderColumnStrings) != len(exp.RowKeys) {
			panic(fmt.Sprintf("header column strings length %d != row keys length %d", len(exp.HeaderColumnStrings), len(exp.RowKeys)))
		}
		// Construct the table with the name and columns.
		t := Table{
			Name:          exp.Name,
			HeaderStrings: exp.HeaderRowStrings,
			Rows:          make([]Row, nRows),
		}

		// Build each row from the expansions
		for row_i := 0; row_i < nRows; row_i++ {
			row := &t.Rows[row_i]
			row.Name = exp.HeaderColumnStrings[row_i]
			row.Cells = make([]Cell, nColumns) // create a size-nColumns array of empty cells
			for col_j := 0; col_j < nColumns; col_j++ {
				source_tokens := []string{}
				if exp.KeyPrefix != "" {
					source_tokens = append(source_tokens, exp.KeyPrefix)
				}
				if exp.RowKeys[row_i] != "" {
					source_tokens = append(source_tokens, exp.RowKeys[row_i])
				}
				if exp.ColumnKeys[col_j] != "" {
					source_tokens = append(source_tokens, exp.ColumnKeys[col_j])
				}
				if exp.RowKeys2 != nil && len(exp.RowKeys2) > row_i && exp.RowKeys2[row_i] != "" {
					source_tokens = append(source_tokens, exp.RowKeys2[row_i])
				}
				source := strings.Join(source_tokens, "/")
				row.Cells[col_j] = Cell{
					Source:    source,
					ValueType: exp.Unit,
				}
			}
		}

		// Append this new table to the existing slice
		b.Tables = append(b.Tables, t)
	}
}

// ====================================================
// Render homepage
// ====================================================
func RenderHomepage(aggregatedBenchmarksPath string, pageNames []string, dataPath, destinationPath string) error {
	layout, err := os.ReadFile("homepage.yml")
	if err != nil {
		log.Fatalf("error: %v", err)
	}

	// Unmarshal the YAML into HomepageContent (which includes expansions)
	var homepageContent HomepageContent
	err = yaml.Unmarshal(layout, &homepageContent)
	if err != nil {
		log.Fatalf("error: %v", err)
	}

	rawData, err := LoadBenchmarks(aggregatedBenchmarksPath)
	if err != nil {
		return err
	}
	homepageContent.Context = rawData.Context
	// put all benches in a dictionary for easy lookup
	allBenches := make(map[string]BenchmarkData)
	for _, bench := range rawData.Benchmarks {
		allBenches[bench.Name] = bench
	}

	// 1) Expand expansions into normal Table structures:
	expandAllBenchmarks(&homepageContent, allBenches)

	// 2) Parse real data from aggregated_benchmarks.json
	err = ParseBenchmarkData(aggregatedBenchmarksPath, &homepageContent, allBenches)
	if err != nil {
		return err
	}

	// 3) Add commit info
	commitSha, err := os.ReadFile(filepath.Join(dataPath, "commit-sha.txt"))
	if err != nil {
		return err
	}
	homepageContent.Context.CommitSha = string(commitSha)

	// 4) Render the homepage (index.html)
	type Data struct {
		HomepageContent
		PageNames []string
	}
	data := Data{
		HomepageContent: homepageContent,
		PageNames:       pageNames,
	}

	if err := ExecuteTemplate(
		fmt.Sprintf("%s/index.html", destinationPath),
		data,
		"templates/pages/index.html",
	); err != nil {
		return err
	}
	return nil
}
