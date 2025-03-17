module github.com/coinbase/cb-mpc/demo-mocknet

go 1.21

toolchain go1.21.3

require (
	github.com/coinbase/cb-mpc/cb-mpc-go v0.0.0-20240501131245-1eee31b51009
	github.com/stretchr/testify v1.10.0
)

require (
	github.com/davecgh/go-spew v1.1.1 // indirect
	github.com/pmezard/go-difflib v1.0.0 // indirect
	gopkg.in/yaml.v3 v3.0.1 // indirect
)

replace github.com/coinbase/cb-mpc/cb-mpc-go => ../../cb-mpc-go
