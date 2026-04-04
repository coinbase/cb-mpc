#!/bin/bash
set -euo pipefail

# Build script for cb-mpc C++ library
# Usage: ./scripts/build_cpp.sh [Debug|Test|Release]

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd -P)"
CB_MPC_DIR="${CB_MPC_DIR:-$REPO_ROOT/..}"
CB_MPC_DIR="$(cd "$CB_MPC_DIR" && pwd -P)"

BUILD_TYPE="${1:-Release}"
CBMPC_OPENSSL_ROOT="${CBMPC_OPENSSL_ROOT:-/usr/local/opt/openssl@3.6.1}"
export CBMPC_OPENSSL_ROOT

echo "Building cb-mpc C++ library (BUILD_TYPE=$BUILD_TYPE)..."

cd "$CB_MPC_DIR"

# Initialize submodules if needed
if [ ! -f "vendors/secp256k1/include/secp256k1.h" ]; then
    echo "Initializing submodules..."
    git submodule update --init --recursive
fi

# Build OpenSSL if missing on macOS (cb-mpc default expects 3.6.1 layout).
if [[ "${OSTYPE:-}" == "darwin"* ]] && [ ! -f "$CBMPC_OPENSSL_ROOT/lib/libcrypto.a" ]; then
    echo "OpenSSL static library not found at $CBMPC_OPENSSL_ROOT, building..."
    if [[ $(uname -m) == "arm64" ]]; then
        bash "$REPO_ROOT/scripts/openssl/build-static-openssl-macos-m1.sh"
    else
        bash "$REPO_ROOT/scripts/openssl/build-static-openssl-macos.sh"
    fi
fi

# Build cb-mpc library
echo "Building cb-mpc library..."
BUILD_TYPE="$BUILD_TYPE" make build

# Install public headers + library for FFI/CGO consumers.
echo "Installing cb-mpc public artifacts..."
bash scripts/install.sh --mode public --build-type "$BUILD_TYPE"

echo "cb-mpc public install ready at $CB_MPC_DIR/build/install/public"
