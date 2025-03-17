#!/bin/bash

set -e

SCRIPT_PATH="$(
  cd -- "$(dirname "$0")" >/dev/null 2>&1
  pwd -P
)"
ROOT_PATH="${SCRIPT_PATH}/.."

cd $ROOT_PATH

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <ref_name>"
  echo "  ref_name: The release tag name (e.g. cb-mpc-1.0.0)"
  exit 1
fi

REF_NAME=$1

make clean
make clean-demos
make clean-bench

tar -czf "${REF_NAME}.tar.gz" \
  --exclude='.git' \
  --exclude='.github' \
  --exclude='.buildkite' \
  *

