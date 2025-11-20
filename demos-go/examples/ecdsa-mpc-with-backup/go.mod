module github.com/coinbase/cb-mpc/demo-go-ecdsa-mpc-with-backup

go 1.24.0

toolchain go1.24.2

replace github.com/coinbase/cb-mpc/demos-go/cb-mpc-go => ../../cb-mpc-go

require (
	github.com/btcsuite/btcd/btcec/v2 v2.3.5
	github.com/coinbase/cb-mpc/demos-go/cb-mpc-go v0.0.0-20240501131245-1eee31b51009
	golang.org/x/sync v0.15.0
)

require (
	github.com/decred/dcrd/dcrec/secp256k1/v4 v4.0.1 // indirect
	golang.org/x/crypto v0.45.0
)
