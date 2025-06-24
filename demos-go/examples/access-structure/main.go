package main

import (
	"fmt"

	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/curve"
	"github.com/coinbase/cb-mpc/demos-go/cb-mpc-go/api/mpc"
)

func main() {
	// Build the same sample tree described in the AccessNode documentation.
	root := mpc.And("",
		mpc.Or("role",
			mpc.Leaf("role:Admin"),
			mpc.Leaf("dept:HR"),
		),
		mpc.Threshold("sig", 2,
			mpc.Leaf("sig:A"),
			mpc.Leaf("sig:B"),
			mpc.Leaf("sig:C"),
		),
	)

	// Use secp256k1 curve in this example.
	c, err := curve.NewSecp256k1()
	if err != nil {
		panic(err)
	}

	as := &mpc.AccessStructure{
		Root:  root,
		Curve: c,
	}

	fmt.Print(as)
}
