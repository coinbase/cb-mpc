#!/bin/bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CERT_DIR="${1:-"$ROOT_DIR/demos/tmp/certs/eddsa_mp_backup_mtls"}"
PARTY_ADDRS="${2:-auto}"
PEER_CERT_DIRS=("$CERT_DIR/peer-alpha" "$CERT_DIR/peer-bravo" "$CERT_DIR/peer-charlie")

if [[ "$PARTY_ADDRS" == "auto" ]]; then
  PARTY_ADDRS="$(python3 - <<'PY'
import socket
ports = []
for _ in range(3):
    sock = socket.socket()
    sock.bind(("127.0.0.1", 0))
    ports.append((sock.getsockname()[1], sock))
print(",".join(f"127.0.0.1:{port}" for port, _ in ports))
for _, sock in ports:
    sock.close()
PY
)"
fi

IFS=',' read -r PARTY0_ADDR PARTY1_ADDR PARTY2_ADDR <<<"$PARTY_ADDRS"
if [[ -z "${PARTY0_ADDR:-}" || -z "${PARTY1_ADDR:-}" || -z "${PARTY2_ADDR:-}" ]]; then
  echo "expected three comma-separated party addresses, got: $PARTY_ADDRS" >&2
  exit 1
fi

PARTY0_LOG="$(mktemp)"
PARTY1_LOG="$(mktemp)"
PARTY2_LOG="$(mktemp)"
PARTY0_PID=""
PARTY1_PID=""
PARTY2_PID=""
LAST_PID=""

cleanup() {
  if [[ -n "$PARTY0_PID" ]]; then
    kill "$PARTY0_PID" >/dev/null 2>&1 || true
  fi
  if [[ -n "$PARTY1_PID" ]]; then
    kill "$PARTY1_PID" >/dev/null 2>&1 || true
  fi
  if [[ -n "$PARTY2_PID" ]]; then
    kill "$PARTY2_PID" >/dev/null 2>&1 || true
  fi
  rm -f "$PARTY0_LOG" "$PARTY1_LOG" "$PARTY2_LOG"
}
trap cleanup EXIT

launch_party() {
  local self="$1"
  local listen_addr="$2"
  local log_file="$3"

  bash "$ROOT_DIR/scripts/go_with_cpp.sh" go run ./demos/eddsa_mp_backup_mtls/main.go \
    --self "$self" \
    --listen "$listen_addr" \
    --party-addrs "$PARTY_ADDRS" \
    --ca "$CERT_DIR/ca.pem" \
    --cert "${PEER_CERT_DIRS[$self]}/cert.pem" \
    --key "${PEER_CERT_DIRS[$self]}/key.pem" \
    >"$log_file" 2>&1 &
  LAST_PID=$!
}

launch_party 0 "$PARTY0_ADDR" "$PARTY0_LOG"
PARTY0_PID=$LAST_PID
sleep 1
launch_party 1 "$PARTY1_ADDR" "$PARTY1_LOG"
PARTY1_PID=$LAST_PID
sleep 1
launch_party 2 "$PARTY2_ADDR" "$PARTY2_LOG"
PARTY2_PID=$LAST_PID

alpha_status=0
bravo_status=0
charlie_status=0
wait "$PARTY0_PID" || alpha_status=$?
wait "$PARTY1_PID" || bravo_status=$?
wait "$PARTY2_PID" || charlie_status=$?

echo "--- self 0 / peer alpha ---"
cat "$PARTY0_LOG"
echo "--- self 1 / peer bravo ---"
cat "$PARTY1_LOG"
echo "--- self 2 / peer charlie ---"
cat "$PARTY2_LOG"

if [[ $alpha_status -ne 0 || $bravo_status -ne 0 || $charlie_status -ne 0 ]]; then
  exit 1
fi
