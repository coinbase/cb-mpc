set -e

# The main project (when built with upstream Clang 20+) targets macOS 16.0 by default
# (e.g., via `-platform_version macos 16.0 26.x`). If OpenSSL is built with AppleClang,
# it will typically default to targeting the *current* macOS version (26.x), which then
# causes linker warnings like:
#   "object file ... was built for newer 'macOS' version (26.0) than being linked (16.0)"
#
# Prefer upstream Clang if available (matches project recommendation) and default the
# deployment target to 16.0 unless the caller overrides it.
if [ -z "${CC:-}" ]; then
  if [ -x "/opt/homebrew/opt/llvm/bin/clang" ]; then
    export CC="/opt/homebrew/opt/llvm/bin/clang"
    export CXX="/opt/homebrew/opt/llvm/bin/clang++"
  elif [ -x "/usr/local/opt/llvm/bin/clang" ]; then
    export CC="/usr/local/opt/llvm/bin/clang"
    export CXX="/usr/local/opt/llvm/bin/clang++"
  fi
fi
export MACOSX_DEPLOYMENT_TARGET="${MACOSX_DEPLOYMENT_TARGET:-16.0}"

cd /tmp
curl -L https://github.com/openssl/openssl/releases/download/openssl-3.6.1/openssl-3.6.1.tar.gz --output openssl-3.6.1.tar.gz
expectedHash='b1bfedcd5b289ff22aee87c9d600f515767ebf45f77168cb6d64f231f518a82e'
fileHash=$(sha256sum openssl-3.6.1.tar.gz | cut -d " " -f 1 )

if [ $expectedHash != $fileHash ]
then
  echo 'ERROR: SHA256 DOES NOT MATCH!'
  echo 'expected: ' $expectedHash
  echo 'file:     ' $fileHash
  exit 1
fi


tar -xzf openssl-3.6.1.tar.gz
cd openssl-3.6.1
sed -i -e 's/^static//' crypto/ec/curve25519.c


./Configure -g3 -static -DOPENSSL_THREADS no-shared \
  no-afalgeng no-apps no-aria no-autoload-config no-bf no-camellia no-cast no-chacha no-cmac no-cms no-crypto-mdebug \
  no-comp no-cmp no-ct no-des no-dh no-dgram no-dsa no-dso no-dtls no-dynamic-engine no-ec2m no-egd no-engine no-external-tests \
  no-gost no-http no-idea no-mdc2 no-md2 no-md4 no-module no-nextprotoneg no-ocb no-ocsp no-psk no-padlockeng no-poly1305 \
  no-quic no-rc2 no-rc4 no-rc5 no-rfc3779 no-scrypt no-sctp no-seed no-siphash no-sm2 no-sm3 no-sm4 no-sock no-srtp no-srp \
  no-ssl-trace no-ssl3 no-stdio no-tests no-tls no-ts no-unit-test no-uplink no-whirlpool no-zlib \
  --prefix=/usr/local/opt/openssl@3.6.1 darwin64-x86_64-cc

make -j
make install_sw
make clean
