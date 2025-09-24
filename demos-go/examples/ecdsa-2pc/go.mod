module github.com/coinbase/cb-mpc/demo-go-ecdsa-2pc

go 1.23.0

toolchain go1.24.2

require github.com/coinbase/cb-mpc/demos-go/cb-mpc-go v0.0.0-20240501131245-1eee31b51009

require github.com/decred/dcrd/dcrec/secp256k1/v4 v4.0.1 // indirect

require (
	github.com/btcsuite/btcd/btcec/v2 v2.3.5
	github.com/stretchr/testify v1.10.0 // indirect
	golang.org/x/sync v0.15.0 // indirect
)

replace github.com/coinbase/cb-mpc/demos-go/cb-mpc-go => ../../cb-mpc-go
