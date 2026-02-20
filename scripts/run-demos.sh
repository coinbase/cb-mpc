#!/bin/bash

set -e

SCRIPT_PATH="$(
  cd -- "$(dirname "$0")" >/dev/null 2>&1
  pwd -P
)"

ROOT_PATH="${SCRIPT_PATH}/.."
DEMOS_CPP_DIR="${ROOT_PATH}/demo-cpp"
DEMO_API_DIR="${ROOT_PATH}/demo-api"
DEMO_GO_DIR="${ROOT_PATH}/demo-go"

BUILD_TYPE="${BUILD_TYPE:-Release}"
CBMPC_PREFIX_PUBLIC="${CBMPC_PREFIX_PUBLIC:-${ROOT_PATH}/build/install/public}"
CBMPC_PREFIX_FULL="${CBMPC_PREFIX_FULL:-${ROOT_PATH}/build/install/full}"
# OpenSSL path is used by demos (C++ via CMake; Go via CGO_LDFLAGS). Keep it
# configurable and consistent with `cmake/openssl.cmake`.
CBMPC_OPENSSL_ROOT="${CBMPC_OPENSSL_ROOT:-/usr/local/opt/openssl@3.6.1}"

CPP_DEMOS=("basic_primitive" "zk" "parallel_transport")
API_DEMOS=("pve" "hd_keyset_ecdsa_2p" "ecdsa_mp_pve_backup" "schnorr_2p_pve_batch_backup")
GO_DEMOS=("pve" "sign" "tdh2" "eddsa_mp_pve_ac_backup")

go_is_supported() {
  if ! command -v go >/dev/null 2>&1; then
    echo "Skipping Go demos (go not installed)."
    return 1
  fi

  local ver
  ver="$(go version 2>/dev/null || true)"
  # Example: "go version go1.21.5 darwin/arm64"
  if [[ "$ver" =~ go([0-9]+)\.([0-9]+) ]]; then
    local major="${BASH_REMATCH[1]}"
    local minor="${BASH_REMATCH[2]}"
    if (( major > 1 || (major == 1 && minor >= 20) )); then
      return 0
    fi
    echo "Skipping Go demos (Go >= 1.20 required; found go${major}.${minor})."
    return 1
  fi

  # If parsing fails, attempt to run anyway.
  return 0
}

resolve_openssl_lib_dir() {
  local root="$1"
  if [[ -z "$root" ]]; then
    return 1
  fi

  # Prefer lib64 when present (common on Linux) *and* contains libcrypto.
  if [[ -d "${root}/lib64" ]] && compgen -G "${root}/lib64/libcrypto.*" >/dev/null; then
    echo "${root}/lib64"
    return 0
  fi
  if [[ -d "${root}/lib" ]] && compgen -G "${root}/lib/libcrypto.*" >/dev/null; then
    echo "${root}/lib"
    return 0
  fi

  # Fall back to lib directories even if we can't find a concrete libcrypto
  # artifact (caller will still attempt to link and may succeed via defaults).
  if [[ -d "${root}/lib64" ]]; then
    echo "${root}/lib64"
    return 0
  fi
  if [[ -d "${root}/lib" ]]; then
    echo "${root}/lib"
    return 0
  fi
  return 1
}

clean() {
  for proj in ${CPP_DEMOS[@]}; do
    rm -rf $DEMOS_CPP_DIR/$proj/build/
  done
  for proj in ${API_DEMOS[@]}; do
    rm -rf $DEMO_API_DIR/$proj/build/
  done
}

build_all_cpp() {
  for proj in ${CPP_DEMOS[@]}; do
    cd ${DEMOS_CPP_DIR}/$proj
    cmake -Bbuild/${BUILD_TYPE} -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCBMPC_SOURCE_DIR=${CBMPC_PREFIX_FULL}
    cmake --build build/${BUILD_TYPE}/
  done
}

run_all_cpp() {
  build_all_cpp
  for proj in ${CPP_DEMOS[@]}; do
    ${DEMOS_CPP_DIR}/$proj/build/${BUILD_TYPE}/mpc-demo-$proj
  done
}

build_all_api() {
  for proj in ${API_DEMOS[@]}; do
    cd ${DEMO_API_DIR}/$proj
    cmake -Bbuild/${BUILD_TYPE} -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCBMPC_SOURCE_DIR=${CBMPC_PREFIX_PUBLIC}
    cmake --build build/${BUILD_TYPE}/
  done
}

run_all_api() {
  build_all_api
  for proj in ${API_DEMOS[@]}; do
    ${DEMO_API_DIR}/$proj/build/${BUILD_TYPE}/mpc-demo-api-$proj
  done
}

run_all_go() {
  if ! go_is_supported; then
    return 0
  fi

  local openssl_lib_dir
  openssl_lib_dir="$(resolve_openssl_lib_dir "${CBMPC_OPENSSL_ROOT}" || true)"
  if [[ -z "${openssl_lib_dir}" ]]; then
    echo "Skipping Go demos (OpenSSL not found under CBMPC_OPENSSL_ROOT='${CBMPC_OPENSSL_ROOT}')."
    return 0
  fi

  for proj in ${GO_DEMOS[@]}; do
    cd ${DEMO_GO_DIR}/$proj
    CGO_ENABLED=1 \
      CGO_CFLAGS="-I${CBMPC_PREFIX_PUBLIC}/include" \
      CGO_LDFLAGS="-L${CBMPC_PREFIX_PUBLIC}/lib -L${openssl_lib_dir}" \
      go run .
  done
}

run_cpp_demo() {
  local proj="$1"
  cd "${DEMOS_CPP_DIR}/${proj}"
  cmake -Bbuild/${BUILD_TYPE} -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCBMPC_SOURCE_DIR=${CBMPC_PREFIX_FULL}
  cmake --build build/${BUILD_TYPE}/
  "${DEMOS_CPP_DIR}/${proj}/build/${BUILD_TYPE}/mpc-demo-${proj}"
}

run_api_demo() {
  local proj="$1"
  cd "${DEMO_API_DIR}/${proj}"
  cmake -Bbuild/${BUILD_TYPE} -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCBMPC_SOURCE_DIR=${CBMPC_PREFIX_PUBLIC}
  cmake --build build/${BUILD_TYPE}/
  "${DEMO_API_DIR}/${proj}/build/${BUILD_TYPE}/mpc-demo-api-${proj}"
}

run_go_demo() {
  local proj="$1"
  if ! go_is_supported; then
    return 0
  fi

  local openssl_lib_dir
  openssl_lib_dir="$(resolve_openssl_lib_dir "${CBMPC_OPENSSL_ROOT}" || true)"
  if [[ -z "${openssl_lib_dir}" ]]; then
    echo "Skipping Go demo '${proj}' (OpenSSL not found under CBMPC_OPENSSL_ROOT='${CBMPC_OPENSSL_ROOT}')."
    return 0
  fi
  cd "${DEMO_GO_DIR}/${proj}"
  CGO_ENABLED=1 \
    CGO_CFLAGS="-I${CBMPC_PREFIX_PUBLIC}/include" \
    CGO_LDFLAGS="-L${CBMPC_PREFIX_PUBLIC}/lib -L${openssl_lib_dir}" \
    go run .
}

POSITIONAL_ARGS=()

while [[ $# -gt 0 ]]; do
  case $1 in
  --run-all)
    run_all_cpp
    run_all_api
    run_all_go
    shift # past argument
    ;;
  --run)
    TEST_NAME="$2"
    run_cpp_demo "$TEST_NAME"
    shift # past argument
    shift # past value
    ;;
  --run-api)
    TEST_NAME="$2"
    run_api_demo "$TEST_NAME"
    shift # past argument
    shift # past value
    ;;
  --run-go)
    TEST_NAME="$2"
    run_go_demo "$TEST_NAME"
    shift # past argument
    shift # past value
    ;;
  --clean)
    clean
    shift # past argument
    ;;
  -* | --*)
    echo "Unknown option $1"
    exit 1
    ;;
  *)
    POSITIONAL_ARGS+=("$1") # save positional arg
    shift                   # past argument
    ;;
  esac
done

set -- "${POSITIONAL_ARGS[@]}" # restore positional parameters
