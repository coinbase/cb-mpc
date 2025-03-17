# Coinbase MPC

# Table of Contents

- [Introduction](#introduction)
  - [Overview](#overview)
  - [Key Features](#key-features)
- [Directory Structure](#directory-structure)
- [Supported Protocols](#supported-protocols)
- [Design Principles and Secure Usage](#design-principles-and-secure-usage)
- [External Dependencies](#external-dependencies)
  - [OpenSSL](#openssl)
    - [Internal Header Files](#internal-header-files)
    - [RSA OAEP Padding Modification](#rsa-oaep-padding-modification)
  - [Bitcoin Secp256k1 Curve implementation](#bitcoin-secp256k1-curve-implementation)


# Introduction

Welcome to the Coinbase Open Source MPC Library. This repository provides the essential cryptographic protocols that can be utilized to secure asset keys in a decentralized manner using MPC (secure multiparty computation / threshold signing) for blockchain networks.

## Overview

This cryptographic library is based on the MPC library used at Coinbase to protect cryptoassets, with modifications to make it suitable for public use. The library is designed as a general-purpose cryptographic library for securing cryptoasset keys, allowing developers to build their own applications. Coinbase has invested significantly in building a secure MPC library, and it is our hope that this library will help those interested in deploying MPC to do so easily and securely.

## Key Features

- **Safety by Default:** Prioritizing safe cryptographic practices to minimize security errors.
- **Custom Networking Layer:** Versatile integration with any networking setup.
- **General-Purpose Use:** Focuses solely on cryptographic functions, enabling varied applications.
- **Theoretical and Specification Docs:** Includes both theoretical foundations and detailed cryptographic specifications for all primitives and protocols.

The code in this open source library is derived from the code used at Coinbase, with significant changes in order to make it a general-purpose library. In particular, Coinbase applies these protocols with very specific flows as needed in our relevant applications, whereas this code is designed to enable general-purpose use and therefore supports arbitrary flows. In some cases, this generality impacts efficiency, in order to ensure safe default usage.

In addition to releasing the source code for our library, we have published the underlying theoretical work along with detailed specifications. This is a crucial step because merely implementing a theoretical paper can overlook significant errors. At Coinbase, we adhere to the following development process:

1. Review existing research and, if necessary, re-validate the proofs or conduct original research.
2. Draft a detailed specification encompassing all the necessary details for accurately implementing a protocol.
3. After thorough review of the research and specifications, proceed with implementation and code review.

The theory documents and specifications are a considerable contribution within themselves, as a resource for cryptographers and practitioners.

Although this library is designed for general use, we have included examples showcasing common applications:

1. **HD-MPC**: This is the MPC version of an HD-Wallet where the keys are derived according to an HD tree. The library contains the source code for how to generate keys and also to derive keys for the tree (see [src/cbmpc/protocol/hd_keyset_ecdsa_2p.cpp](src/cbmpc/protocol/hd_keyset_ecdsa_2p.cpp)). This can be used to perform a batch ECDSA signature or sequential signatures as shown in the test file, [src/cbmpc/tests/mpc_hdmpc_ecdsa_2p_test.cpp](src/cbmpc/tests/mpc_hdmpc_ecdsa_2p_test.cpp). We stress that this is not BIP32-compliant, but is indistinguishable from it; more details can be found in [docs/theory/mpc-friendly-derivation-theory.pdf](docs/theory/mpc-friendly-derivation-theory.pdf).
2. **ECDSA-MPC with Threshold EC-DKG**: This example showcases how a threshold of parties (or more generally any quorum of parties according to a given access structure) can perform ECDSA-MPC. The code can be found in [src/cbmpc/protocol/ec_dkg.cpp](src/cbmpc/protocol/ec_dkg.cpp) and its usage can be found in [tests/unit/protocol/test_ecdsa_mp.cpp](tests/unit/protocol/test_ecdsa_mp.cpp).
3. **ECDSA-MPC with Threshold Backup**: This example showcases various things. First, the code is in Go, [demos/demos-go/ecdsa-mpc-with-backup/main.go](demos/demos-go/ecdsa-mpc-with-backup/main.go) and therefore showcases how the C++ core library can be used in a Go project. Second, it showcases how different protocols can be combined together to create a full solution. In this case, we use PVE (publicly-verifiable encryption) as a way of creating verifiable backup of keyshares according to an access structure (e.g., a threshold of `t` out of `n` parties). The code shows how the backup can be created and restored. It also shows how the backup can be used to generate a signature. Note that the key generation can be done using the threshold EC-DKG protocol, which is showcased in the previous example. However, for simplicity a normal additive DKG is used in this example.
4. **Various other uses cases, including ZKPs**: The demo code under [demos/demos-cpp](demos/demos-cpp) and [demos/demos-go](demos/demos-go), and the tests under [tests](tests), contain various examples of how the different protocols can be used. Specifically, for the case of ZKPs, the tests can be found under [tests/unit/zk/test_zk.cpp](tests/unit/zk/test_zk.cpp).

The library comes with various tests and checks to increase the confidence in the code including:

- Constant time tests: See `make dudect`
- Unit tests: See `make test`
- Benchmarks: See `make bench`
- Linting: See `make lint`

# Directory Structure

- `docs`: the pdf files that define the detailed cryptographic specification and theoretical documentation (you need to enable git-lfs to get them)
- `src`: contains the cpp library and its unit tests
- `cb-mpc-go`: contains an example of how a go wrapper for the cpp library can be written
- `demos/demos-cpp`: a collection of examples of common use cases
- `demos/demos-go`: an example of how `cb-mpc-go` can be used to run an example use case
- `demos/mocknet`: an example of how a network infra can be implemented
- `scripts`: a collection of scripts used by the Makefile
- `tools/benchmark`: a collection of benchmarks for the library
- `tests/{dudect,integration,unit}`: a collection of tests for the library

# Initial Clone and Setup

After cloning the repo, you need to update the submodules with the following command.

```
git submodule update --init --recursive
```

Furthermore, to obtain the documentations (in pdf form), you need to enable [git-lfs](https://git-lfs.com/)

# Building the code

## Build Modes

There are three build modes available:

- **Dev**: This mode has no optimization and includes debug information for development and debugging purposes.
- **Test**: This mode enables security checks and validations to ensure the code is robust and secure.
- **Release**: This mode applies the highest level of optimization for maximum performance and disables checks to improve runtime efficiency.

## On Mac

The library depends on OpenSSL. Therefore, the first step is to build the proper version of OpenSSL. The write permission to the `/usr/local/opt` may be required

```bash
scripts/openssl/build-static-openssl-macos.sh
or
scripts/openssl/build-static-openssl-macos-m1.sh
```

Build the library by running

`make build`

To test the library, run

`make test`


To run the demos and benchmarks, you first need to install the library:

`sudo make install`

This will copy the `.a` files and header files to `/usr/local/opt/cbmpc/lib`

To run the demos (both cpp and go), run

`make demos`

To run the benchmarks, run

`make bench`

Our benchmark results can be found at <https://coinbase.github.io/cb-mpc>

Finally, to clean up, run

```bash
make clean
make clean-demos
```

To use `clang-format` to lint, we use the clang-format version 14.
Install it with

```
brew install llvm@14
brew link --force --overwrite llvm@14
```

then `make lint` will format all `.cpp` and `.h` files in `src` and `tests`

## In Docker

We have a Dockerfile that already contains steps for building the proper OpenSSL files. Therefore, the first step is to create the image

`make image`

You can run the rest of the `make` commands by invoking them inside docker.
For example, for a one-off testing, you can run

`docker run -it --rm -v $(pwd):/code -t cb-mpc bash -c 'make test'`


## Supported Protocols

Please note that all cryptographic code has a specification (except for code like wrappers around OpenSSL and the like), but there are some protocol specifications that are not implemented but still appear in the specifications since they may be useful for some application developers.

<table>
  <tr>
    <td><b> Name </b></td>
    <td><b> Spec </b></td>
    <td><b> Theory </b></td>
    <td><b> Code </b></td>
  </tr>
    <tr>
    <td>Basic Primitives</td>
    <td><a href="/docs/spec/basic-primitives-spec.pdf">spec</a></td>
    <td><a href="/docs/theory/basic-primitives-theory.pdf">theory</a></td>
    <td><a href="/src/cbmpc/crypto/">code folder</a></td>
  </tr>
    <tr>
    <td>Zero-Knowledge Proofs</td>
    <td><a href="/docs/spec/zk-proofs-spec.pdf">spec</a></td>
    <td><a href="/docs/theory/zk-proofs-theory.pdf">theory</a></td>
    <td><a href="/src/cbmpc/zk/">code folder</a></td>
  </tr>
  <tr>
    <td>EC-DKG</td>
    <td><a href="/docs/spec/ec-dkg-spec.pdf">spec</a></td>
    <td><a href="/docs/theory/ec-dkg-theory.pdf">theory</a></td>
    <td><a href="/src/cbmpc/protocol/ec_dkg.h">coinbase::mpc::eckey</a></td>
  </tr>
  <tr>
    <td>ECDSA-2PC</td>
    <td><a href="/docs/spec/ecdsa-2pc-spec.pdf">spec</a></td>
    <td><a href="/docs/theory/ecdsa-2pc-theory.pdf">theory</a></td>
    <td><a href="/src/cbmpc/protocol/ecdsa_2p.h">coinbase::mpc::ecdsa2pc</a></td>
  </tr>
    <td>ECDSA-MPC</td>
    <td><a href="/docs/spec/ecdsa-mpc-spec.pdf">spec</a></td>
    <td><a href="/docs/theory/ecdsa-mpc-theory.pdf">theory</a></td>
    <td><a href="/src/cbmpc/protocol/ecdsa_mp.h">coinbase::mpc::ecdsampc</a></td>
  </tr>
  <tr>
    <td>MPC Friendly Derivation</td>
    <td><a href="/docs/spec/mpc-friendly-derivation-spec.pdf">spec</a></td>
    <td><a href="/docs/theory/mpc-friendly-derivation-theory.pdf">theory</a></td>
    <td><a href="/src/cbmpc/protocol/hd_keyset_ecdsa_2p.h">key_share_ecdsa_hdmpc_2p_t</a></td>
  </tr>
  <tr>
    <td>Oblivious Transfer (OT) and OT Extension</td>
    <td><a href="/docs/spec/oblivious-transfer-spec.pdf">spec</a></td>
    <td><a href="/docs/theory/oblivious-transfer-theory.pdf">theory</a></td>
    <td><a href="/src/cbmpc/protocol/ot.h">ot</a></td>
  </tr>
  <tr>
    <td>Publicly Verifiable Encryption (PVE)</td>
    <td><a href="/docs/spec/publicly-verifiable-encryption-spec.pdf">spec</a></td>
    <td><a href="/docs/theory/publicly-verifiable-encryption-as-ZK-theory.pdf">theory</a></td>
    <td><a href="/src/cbmpc/protocol/pve.h">pve</a></td>
  </tr>
  <tr>
    <td>Schnorr</td>
    <td><a href="/docs/spec/schnorr-spec.pdf">spec</a></td>
    <td><a href="/docs/theory/schnorr-theory.pdf">theory</a></td>
    <td><a href="/src/cbmpc/protocol/schnorr_2p.h">coinbase::mpc::schnorr2p</a> and <a href="/src/cbmpc/protocol/schnorr_mp.h">coinbase::mpc::schnorrmp</a></td>
  </tr>
  <tr>
    <td>Threshold Encryption (TDH2)</td>
    <td><a href="/docs/spec/tdh2-spec.pdf">spec</a></td>
    <td><a href="/docs/theory/tdh2-theory.pdf">theory</a></td>
    <td><a href="/src/cbmpc/crypto/tdh2.h">coinbase::crypto::tdh2</a></td>
  </tr>
  <tr>
  </tr>
</table>


# Design Principles and Secure Usage

We have outlined our cryptographic design principles and some conventions regarding our documentation in our [design principles document](/docs/design-principles.pdf). Furthermore, our [secure usage document](/docs/secure-usage.pdf) describes important security guidelines that should be followed when using the library. Finally, we have strived to create a library that is constant-time to prevent side-channel attacks. This effort is highly dependent on the architecture of the CPU and the compiler used to build the library and therefore is not guaranteed on all platforms. We have outlined our efforts in the [constant-time document](/docs/constant-time.pdf).

# External Dependencies

## OpenSSL
### Internal Header Files

We have included copies of certain OpenSSL internal header files that are not exposed through OpenSSL's public API but are necessary for our implementation. These files can be found in our codebase and are used to access specific OpenSSL functionality that we require. This approach ensures we can maintain compatibility while accessing needed internal features.

### RSA OAEP Padding Modification

Our implementation modifies OpenSSL's OAEP padding algorithm to support deterministic padding when provided with a seed. The key changes are in the `ossl_rsa_padding_add_PKCS1_OAEP_mgf1_ex` function, specifically in steps 3e-3h of the PKCS#1 v2.0 (RFC 2437) OAEP encoding process:

- Instead of generating a random seed internally using `RAND_bytes_ex()`, our implementation accepts an external seed parameter
- We use a simplified MGF1 implementation that directly XORs the mask with the data in a single pass, rather than using separate buffer allocations
- This allows for deterministic padding when the same seed is provided, which is useful for testing and certain cryptographic protocols that require reproducible results

The security properties of OAEP remain intact as long as the provided seed maintains appropriate randomness and uniqueness requirements. For standard encryption operations, we recommend using the non-deterministic version that generates random seeds internally.

## Bitcoin Secp256k1 Curve implementation

We used a modified version of the secp256k1 curve implementation from [coinbase/secp256k1](https://github.com/coinbase/secp256k1) which is forked from [bitcoin-core/secp256k1](https://github.com/bitcoin-core/secp256k1). The change made is to allow calling the curve operations from within our C++ codebase.

Note that as indicated in their repository, the curve addition operations of `secp256k1` are not constant time. To work around this, we have devised a custom point addition operation that is constant time. Please refer to our [documentation](/docs/constant-time.pdf) for more details.