#!/bin/bash
set -euo pipefail

OPENSSL_VERSION="3.6.1"
OPENSSL_PREFIX="${CBMPC_OPENSSL_ROOT:-/usr/local/opt/openssl@${OPENSSL_VERSION}}"
TARBALL="openssl-${OPENSSL_VERSION}.tar.gz"
URL="https://github.com/openssl/openssl/releases/download/openssl-${OPENSSL_VERSION}/${TARBALL}"
EXPECTED_SHA256="b1bfedcd5b289ff22aee87c9d600f515767ebf45f77168cb6d64f231f518a82e"

if ! command -v sha256sum >/dev/null 2>&1; then
  echo "ERROR: sha256sum not found (install coreutils)." >&2
  exit 1
fi
if ! command -v curl >/dev/null 2>&1; then
  echo "ERROR: curl not found." >&2
  exit 1
fi
if ! command -v perl >/dev/null 2>&1; then
  echo "ERROR: perl not found." >&2
  exit 1
fi

workdir="$(mktemp -d)"
cleanup() {
  rm -rf "$workdir"
}
trap cleanup EXIT

cd "$workdir"

echo "Downloading OpenSSL ${OPENSSL_VERSION}..."
curl -fL --retry 3 --retry-delay 1 --output "$TARBALL" "$URL"

fileHash="$(sha256sum "$TARBALL" | cut -d " " -f 1)"
if [ "$EXPECTED_SHA256" != "$fileHash" ]; then
  echo "ERROR: SHA256 does not match!" >&2
  echo "expected: $EXPECTED_SHA256" >&2
  echo "file:     $fileHash" >&2
  exit 1
fi

tar -xzf "$TARBALL"
cd "openssl-${OPENSSL_VERSION}"

# Apply the minimal reviewed local source patch required by cb-mpc after
# verifying the tarball hash above. Any change to this patch should receive
# dedicated security review.
perl -pi -e 's/^static//' crypto/ec/curve25519.c

jobs="$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)"

./config -g3 -static -DOPENSSL_THREADS -fPIC no-shared \
  no-afalgeng no-apps no-aria no-autoload-config no-bf no-camellia no-cast no-chacha no-cmac no-cms no-crypto-mdebug \
  no-comp no-cmp no-ct no-des no-dh no-dgram no-dsa no-dso no-dtls no-dynamic-engine no-ec2m no-egd no-engine no-external-tests \
  no-gost no-http no-idea no-mdc2 no-md2 no-md4 no-module no-nextprotoneg no-ocb no-ocsp no-psk no-padlockeng no-poly1305 \
  no-quic no-rc2 no-rc4 no-rc5 no-rfc3779 no-scrypt no-sctp no-seed no-siphash no-sm2 no-sm3 no-sm4 no-sock no-srtp no-srp \
  no-ssl-trace no-ssl3 no-stdio no-tests no-tls no-ts no-unit-test no-uplink no-whirlpool no-zlib \
  --prefix="$OPENSSL_PREFIX" --libdir=lib64

make build_generated install_sw -j"$jobs"

echo "OpenSSL static install complete at $OPENSSL_PREFIX"
