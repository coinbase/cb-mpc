module github.com/coinbase/cb-mpc/demo-go-ecdsa-2pc

go 1.21

toolchain go1.21.3

require (
	github.com/coinbase/cb-mpc/cb-mpc-go v0.0.0-20240501131245-1eee31b51009
	github.com/coinbase/cb-mpc/demos/mocknet v0.0.0-20240501131245-1eee31b51009
)

replace github.com/coinbase/cb-mpc/cb-mpc-go => ../../../cb-mpc-go

replace github.com/coinbase/cb-mpc/demos/mocknet => ../../mocknet
