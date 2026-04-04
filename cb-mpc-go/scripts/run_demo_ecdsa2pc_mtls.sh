#!/bin/bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CERT_DIR="${1:-"$ROOT_DIR/demos/tmp/certs/ecdsa2pc_mtls"}"
LISTEN_ADDR="${2:-127.0.0.1:9440}"
PEER0_CERT_DIR="$CERT_DIR/peer-alpha"
PEER1_CERT_DIR="$CERT_DIR/peer-bravo"

PARTY0_LOG="$(mktemp)"
PARTY1_LOG="$(mktemp)"
PARTY0_PID=""
PARTY1_PID=""

cleanup() {
  if [[ -n "$PARTY0_PID" ]]; then
    kill "$PARTY0_PID" >/dev/null 2>&1 || true
  fi
  if [[ -n "$PARTY1_PID" ]]; then
    kill "$PARTY1_PID" >/dev/null 2>&1 || true
  fi
  rm -f "$PARTY0_LOG" "$PARTY1_LOG"
}
trap cleanup EXIT

bash "$ROOT_DIR/scripts/go_with_cpp.sh" go run ./demos/ecdsa2pc_mtls/main.go \
  --self 0 \
  --listen "$LISTEN_ADDR" \
  --ca "$CERT_DIR/ca.pem" \
  --cert "$PEER0_CERT_DIR/cert.pem" \
  --key "$PEER0_CERT_DIR/key.pem" \
  >"$PARTY0_LOG" 2>&1 &
PARTY0_PID=$!

sleep 1

bash "$ROOT_DIR/scripts/go_with_cpp.sh" go run ./demos/ecdsa2pc_mtls/main.go \
  --self 1 \
  --peer "$LISTEN_ADDR" \
  --ca "$CERT_DIR/ca.pem" \
  --cert "$PEER1_CERT_DIR/cert.pem" \
  --key "$PEER1_CERT_DIR/key.pem" \
  >"$PARTY1_LOG" 2>&1 &
PARTY1_PID=$!

alpha_status=0
bravo_status=0
wait "$PARTY0_PID" || alpha_status=$?
wait "$PARTY1_PID" || bravo_status=$?

echo "--- self 0 / peer alpha ---"
cat "$PARTY0_LOG"
echo "--- self 1 / peer bravo ---"
cat "$PARTY1_LOG"

if [[ $alpha_status -ne 0 || $bravo_status -ne 0 ]]; then
  exit 1
fi
