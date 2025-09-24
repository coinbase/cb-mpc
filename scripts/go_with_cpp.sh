#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

BUILD_TYPE="${BUILD_TYPE:-Release}"

DO_CD=1
if [[ $# -gt 0 && "$1" == "--no-cd" ]]; then
  DO_CD=0
  shift
fi

INC_DIR="${REPO_ROOT}/src"
LIB_DIRS=(
  "${REPO_ROOT}/build/${BUILD_TYPE}/lib"
  "${REPO_ROOT}/lib/${BUILD_TYPE}"
)

LDFLAGS_ACCUM=()
for d in "${LIB_DIRS[@]}"; do
  LDFLAGS_ACCUM+=("-L${d}")
done

export CGO_CFLAGS="-I${INC_DIR}"
export CGO_CXXFLAGS="-I${INC_DIR}"
export CGO_LDFLAGS="${LDFLAGS_ACCUM[*]}"
export BUILD_TYPE

bash "${SCRIPT_DIR}/auto_build_cpp.sh"

if [[ ${DO_CD} -eq 1 ]]; then
  cd "${REPO_ROOT}/demos-go/cb-mpc-go"
fi

bash "${SCRIPT_DIR}/auto_build_cpp.sh"
exec "$@"


