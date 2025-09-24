#!/usr/bin/env bash
set -euo pipefail

# Auto-rebuild the C++ library if sources changed since the last build.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

BUILD_TYPE="${BUILD_TYPE:-Release}"

SRC_DIR="${REPO_ROOT}/src"
LIB_CANDIDATES=(
  "${REPO_ROOT}/build/${BUILD_TYPE}/lib/libcbmpc.a"
  "${REPO_ROOT}/lib/${BUILD_TYPE}/libcbmpc.a"
)

stat_mtime() {
  if [[ "$(uname)" == "Darwin" ]]; then
    stat -f "%m" "$1"
  else
    stat -c "%Y" "$1"
  fi
}

latest_src_mtime() {
  # Consider C++ sources and headers
  local latest=0
  while IFS= read -r -d '' f; do
    local t
    t=$(stat_mtime "$f")
    if (( t > latest )); then
      latest=$t
    fi
  done < <(find "${SRC_DIR}" -type f \( -name '*.cpp' -o -name '*.h' \) -print0)
  echo "$latest"
}

need_build=1
for lib in "${LIB_CANDIDATES[@]}"; do
  if [[ -f "$lib" ]]; then
    lib_mtime=$(stat_mtime "$lib")
    src_mtime=$(latest_src_mtime)
    if (( src_mtime > lib_mtime )); then
      need_build=1
    else
      need_build=0
    fi
    break
  fi
done

if (( need_build == 1 )); then
  echo "[auto_build_cpp] Building C++ library (${BUILD_TYPE})..."
  make -C "${REPO_ROOT}" build-no-test BUILD_TYPE="${BUILD_TYPE}"
else
  echo "[auto_build_cpp] C++ library up-to-date (${BUILD_TYPE})."
fi


