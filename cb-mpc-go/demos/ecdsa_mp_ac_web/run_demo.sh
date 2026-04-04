#!/bin/bash

set -euo pipefail

DEMO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$DEMO_DIR/../.." && pwd)"
CALLER_DIR="$PWD"
CERT_DIR_INPUT="${1:-"$DEMO_DIR/tmp/certs"}"
HTTP_ADDRS="${2:-127.0.0.1:7080,127.0.0.1:7081,127.0.0.1:7082}"
TRANSPORT_ADDRS="${3:-127.0.0.1:9680,127.0.0.1:9681,127.0.0.1:9682}"
STATE_ROOT_INPUT="${4:-"$DEMO_DIR/tmp/state"}"

resolve_path() {
  local path="$1"
  if [[ "$path" = /* ]]; then
    printf '%s\n' "$path"
    return
  fi
  printf '%s/%s\n' "$CALLER_DIR" "$path"
}

CERT_DIR="$(resolve_path "$CERT_DIR_INPUT")"
STATE_ROOT="$(resolve_path "$STATE_ROOT_INPUT")"
PEER_CERT_DIRS=("$CERT_DIR/peer-alpha" "$CERT_DIR/peer-bravo" "$CERT_DIR/peer-charlie")

if [[ -x "$REPO_ROOT/scripts/go_with_cpp.sh" ]]; then
  GO_RUN=(bash "$REPO_ROOT/scripts/go_with_cpp.sh" go)
else
  GO_RUN=(go)
fi

if [[ ! -f "$CERT_DIR/ca.pem" ]]; then
  echo "missing demo certs under $CERT_DIR" >&2
  if [[ -x "$REPO_ROOT/scripts/certs/generate_certs.sh" ]]; then
    echo "generate them with:" >&2
    echo "  bash \"$REPO_ROOT/scripts/certs/generate_certs.sh\" 3 \"$CERT_DIR\"" >&2
    echo "or:" >&2
    echo "  make demo-certs DEMO_CERT_PARTIES=3 DEMO_CERT_DIR=\"$CERT_DIR\"" >&2
  fi
  exit 1
fi

IFS=',' read -r HTTP0 HTTP1 HTTP2 <<<"$HTTP_ADDRS"
IFS=',' read -r MPC0 MPC1 MPC2 <<<"$TRANSPORT_ADDRS"
if [[ -z "${HTTP0:-}" || -z "${HTTP1:-}" || -z "${HTTP2:-}" ]]; then
  echo "expected three comma-separated HTTP addresses, got: $HTTP_ADDRS" >&2
  exit 1
fi
if [[ -z "${MPC0:-}" || -z "${MPC1:-}" || -z "${MPC2:-}" ]]; then
  echo "expected three comma-separated transport addresses, got: $TRANSPORT_ADDRS" >&2
  exit 1
fi

mkdir -p "$STATE_ROOT/party-0" "$STATE_ROOT/party-1" "$STATE_ROOT/party-2"

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
}
trap cleanup EXIT

launch_party() {
  local self="$1"
  local http_addr="$2"
  local log_file="$3"

  "${GO_RUN[@]}" -C "$DEMO_DIR" run . \
    --self "$self" \
    --http-listen "$http_addr" \
    --transport-addrs "$TRANSPORT_ADDRS" \
    --data-dir "$STATE_ROOT/party-$self" \
    --ca "$CERT_DIR/ca.pem" \
    --cert "${PEER_CERT_DIRS[$self]}/cert.pem" \
    --key "${PEER_CERT_DIRS[$self]}/key.pem" \
    >"$log_file" 2>&1 &
  LAST_PID=$!
}

launch_party 0 "$HTTP0" "$PARTY0_LOG"
PARTY0_PID=$LAST_PID
sleep 1
launch_party 1 "$HTTP1" "$PARTY1_LOG"
PARTY1_PID=$LAST_PID
sleep 1
launch_party 2 "$HTTP2" "$PARTY2_LOG"
PARTY2_PID=$LAST_PID

echo "ECDSA MP AC web demo is starting."
echo "Open these pages in your browser:"
echo "  http://$HTTP0"
echo "  http://$HTTP1"
echo "  http://$HTTP2"
echo
echo "State directories:"
echo "  $STATE_ROOT/party-0"
echo "  $STATE_ROOT/party-1"
echo "  $STATE_ROOT/party-2"
echo
echo "Log files:"
echo "  party 0: $PARTY0_LOG"
echo "  party 1: $PARTY1_LOG"
echo "  party 2: $PARTY2_LOG"
echo
echo "Press Ctrl-C to stop all party servers."

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
