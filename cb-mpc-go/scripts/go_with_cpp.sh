#!/bin/bash
set -euo pipefail

# Wrapper script to run Go commands with proper CGO environment
# Usage: ./scripts/go_with_cpp.sh go test ./...

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd -P)"
CB_MPC_DIR="${CB_MPC_DIR:-$REPO_ROOT/..}"
CB_MPC_DIR="$(cd "$CB_MPC_DIR" && pwd -P)"

BUILD_TYPE="${BUILD_TYPE:-Release}"
CBMPC_PREFIX="${CBMPC_PREFIX:-$CB_MPC_DIR/build/install/public}"
CBMPC_INCLUDE_DIR="$CBMPC_PREFIX/include"
CBMPC_LIB_DIR="$CBMPC_PREFIX/lib"
CBMPC_OPENSSL_ROOT="${CBMPC_OPENSSL_ROOT:-/usr/local/opt/openssl@3.6.1}"
CBMPC_EXTRA_CGO_CFLAGS="${CBMPC_EXTRA_CGO_CFLAGS:-}"
CBMPC_EXTRA_CGO_CXXFLAGS="${CBMPC_EXTRA_CGO_CXXFLAGS:-}"
CBMPC_EXTRA_CGO_LDFLAGS="${CBMPC_EXTRA_CGO_LDFLAGS:-}"
OPENSSL_LIB_DIR="$CBMPC_OPENSSL_ROOT/lib"
if [ -f "$CBMPC_OPENSSL_ROOT/lib64/libcrypto.a" ]; then
    OPENSSL_LIB_DIR="$CBMPC_OPENSSL_ROOT/lib64"
fi

# Auto-build + install cb-mpc public artifacts if needed
if [ ! -f "$CBMPC_LIB_DIR/libcbmpc.a" ] || [ ! -f "$CBMPC_INCLUDE_DIR/cbmpc/c_api/common.h" ]; then
    echo "cb-mpc public install not found, building..."
    bash "$SCRIPT_DIR/build_cpp.sh" "$BUILD_TYPE"
fi

# Set CGO environment variables
export CGO_ENABLED=1
CGO_CFLAGS_BASE="-I${CBMPC_INCLUDE_DIR}"
CGO_CXXFLAGS_BASE="-I${CBMPC_INCLUDE_DIR}"
if [ -f "$OPENSSL_LIB_DIR/libcrypto.a" ] || [ -f "$OPENSSL_LIB_DIR/libcrypto.dylib" ] || [ -f "$OPENSSL_LIB_DIR/libcrypto.so" ]; then
    CGO_LDFLAGS_BASE="-L${CBMPC_LIB_DIR} -L${OPENSSL_LIB_DIR}"
else
    CGO_LDFLAGS_BASE="-L${CBMPC_LIB_DIR}"
fi
export CGO_CFLAGS="${CGO_CFLAGS_BASE}${CBMPC_EXTRA_CGO_CFLAGS:+ ${CBMPC_EXTRA_CGO_CFLAGS}}"
export CGO_CXXFLAGS="${CGO_CXXFLAGS_BASE}${CBMPC_EXTRA_CGO_CXXFLAGS:+ ${CBMPC_EXTRA_CGO_CXXFLAGS}}"
export CGO_LDFLAGS="${CGO_LDFLAGS_BASE}${CBMPC_EXTRA_CGO_LDFLAGS:+ ${CBMPC_EXTRA_CGO_LDFLAGS}}"

# Print the resolved CGO environment only when explicitly troubleshooting.
if [ "${GO_WITH_CPP_DEBUG:-0}" = "1" ]; then
    echo "CGO_CFLAGS=$CGO_CFLAGS"
    echo "CGO_CXXFLAGS=$CGO_CXXFLAGS"
    echo "CGO_LDFLAGS=$CGO_LDFLAGS"
fi

# Execute the provided command
exec "$@"
