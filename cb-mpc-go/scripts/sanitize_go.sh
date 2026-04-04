#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd -P)"
CB_MPC_DIR="${CB_MPC_DIR:-$REPO_ROOT/..}"
CB_MPC_DIR="$(cd "$CB_MPC_DIR" && pwd -P)"

CBMPC_SANITIZE_BUILD_DIR="${CBMPC_SANITIZE_BUILD_DIR:-$CB_MPC_DIR/build/sanitize-go}"
CBMPC_SANITIZE_BUILD_TYPE="${CBMPC_SANITIZE_BUILD_TYPE:-Debug}"
CBMPC_SANITIZE_PREFIX="${CBMPC_SANITIZE_PREFIX:-$CB_MPC_DIR/build/install/public-sanitize}"

SANITIZE_CFLAGS="${SANITIZE_CFLAGS:- -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize=enum}"
SANITIZE_CXXFLAGS="${SANITIZE_CXXFLAGS:- -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize=enum}"
SANITIZE_LDFLAGS="${SANITIZE_LDFLAGS:- -fsanitize=address,undefined}"

ASAN_OPTIONS_VALUE="${ASAN_OPTIONS:-detect_leaks=0}"
UBSAN_OPTIONS_VALUE="${UBSAN_OPTIONS:-print_stacktrace=1:halt_on_error=1}"

if [ "$#" -eq 0 ]; then
    set -- ./internal/cgo
fi

NCORES=1
if command -v nproc >/dev/null 2>&1; then
    NCORES="$(nproc)"
elif [ "$(uname)" = "Darwin" ]; then
    NCORES="$(sysctl -n hw.ncpu)"
fi

src_dir="$(cd "$CB_MPC_DIR" && pwd -P)"
cache_path="$CBMPC_SANITIZE_BUILD_DIR/CMakeCache.txt"
if [ -f "$cache_path" ] && ! grep -Fq "CMAKE_HOME_DIRECTORY:INTERNAL=$src_dir" "$cache_path"; then
    echo "sanitize-go: removing stale build dir '$CBMPC_SANITIZE_BUILD_DIR' (source dir mismatch)"
    rm -rf "$CBMPC_SANITIZE_BUILD_DIR"
fi

cmake \
    -S "$CB_MPC_DIR" \
    -B "$CBMPC_SANITIZE_BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$CBMPC_SANITIZE_BUILD_TYPE" \
    -DBUILD_TESTS=OFF \
    -DBUILD_DUDECT=OFF \
    -DCMAKE_C_FLAGS="$SANITIZE_CFLAGS" \
    -DCMAKE_CXX_FLAGS="$SANITIZE_CXXFLAGS" \
    -DCMAKE_EXE_LINKER_FLAGS="$SANITIZE_LDFLAGS" \
    -DCMAKE_SHARED_LINKER_FLAGS="$SANITIZE_LDFLAGS"

cmake --build "$CBMPC_SANITIZE_BUILD_DIR" -- -j"$NCORES"

bash "$CB_MPC_DIR/scripts/install.sh" \
    --mode public \
    --build-type "$CBMPC_SANITIZE_BUILD_TYPE" \
    --prefix "$CBMPC_SANITIZE_PREFIX"

ASAN_OPTIONS="$ASAN_OPTIONS_VALUE" \
UBSAN_OPTIONS="$UBSAN_OPTIONS_VALUE" \
CBMPC_PREFIX="$CBMPC_SANITIZE_PREFIX" \
CBMPC_EXTRA_CGO_CFLAGS="$SANITIZE_CFLAGS" \
CBMPC_EXTRA_CGO_CXXFLAGS="$SANITIZE_CXXFLAGS" \
CBMPC_EXTRA_CGO_LDFLAGS="$SANITIZE_LDFLAGS" \
    bash "$SCRIPT_DIR/go_with_cpp.sh" go test -count=1 "$@"
