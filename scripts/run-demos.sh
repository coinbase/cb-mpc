#!/bin/bash

set -e

SCRIPT_PATH="$(
  cd -- "$(dirname "$0")" >/dev/null 2>&1
  pwd -P
)"

ROOT_PATH="${SCRIPT_PATH}/.."
DEMOS_CPP_DIR="${ROOT_PATH}/demos-cpp"
DEMOS_GO_DIR="${ROOT_PATH}/demos-go/examples"

CPP_DEMOS=("basic_primitive"  "zk")
GO_DEMOS=("access-structure" "agreerandom" "ecdsa-2pc" "ecdsa-mpc-with-backup" "zk")

clean() {
  for proj in ${CPP_DEMOS[@]}; do
    rm -rf $DEMOS_CPP_DIR/$proj/build/
  done
}

build_all_cpp() {
  for proj in ${CPP_DEMOS[@]}; do
    cd ${DEMOS_CPP_DIR}/$proj
    cmake -Bbuild
    cmake --build build/
  done
}

run_all_cpp() {
  build_all_cpp
  for proj in ${CPP_DEMOS[@]}; do
    ${DEMOS_CPP_DIR}/$proj/build/mpc-demo-$proj
  done
}

run_all_go() {
  # cd $ROOT_PATH
  # make install
  for proj in ${GO_DEMOS[@]}; do
    run_go_demo $proj
  done
}

run_go_demo() {
  cd $DEMOS_GO_DIR/$1
  go mod tidy
  # Ensure CGO uses the locally built C++ lib and auto-rebuilds if needed
  (cd "$ROOT_PATH" && BUILD_TYPE=${BUILD_TYPE:-Release} bash scripts/go_with_cpp.sh --no-cd bash -lc "cd '$DEMOS_GO_DIR/$1' && env CGO_ENABLED=1 go run *.go")
}

POSITIONAL_ARGS=()

while [[ $# -gt 0 ]]; do
  case $1 in
  --run-all)
    run_all_cpp
    run_all_go
    shift # past argument
    ;;
  --run)
    TEST_NAME="$2"
    run_go_demo $TEST_NAME
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
