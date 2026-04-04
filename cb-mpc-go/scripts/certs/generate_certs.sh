#!/bin/bash
# Generate TLS certificates for local demos/tests only.
# Usage: ./generate_certs.sh <num_parties> [output_dir]
# This is not production PKI automation.

set -euo pipefail
umask 077

NUM_PARTIES=${1:-2}
OUTPUT_DIR=${2:-"./certs"}
PEER_LABELS=(alpha bravo charlie delta echo foxtrot golf hotel india juliet kilo lima mike november oscar papa quebec romeo sierra tango uniform victor whiskey xray yankee zulu)

if (( NUM_PARTIES > ${#PEER_LABELS[@]} )); then
    echo "error: NUM_PARTIES=$NUM_PARTIES exceeds supported demo peer identity count ${#PEER_LABELS[@]}" >&2
    exit 1
fi

peer_label() {
    local index="$1"
    printf '%s' "${PEER_LABELS[$index]}"
}

peer_dns_name() {
    local index="$1"
    printf 'peer-%s.demo.cbmpc.local' "$(peer_label "$index")"
}

echo "=== Generating TLS certificates for $NUM_PARTIES parties ==="
echo "Output directory: $OUTPUT_DIR"
echo "This script is for local demos/tests only. Do not use it for production PKI."
echo

# Create directory structure
mkdir -p "$OUTPUT_DIR"

# Step 1: Generate CA
echo "Step 1: Generating CA certificate..."
openssl genpkey -algorithm RSA -out "$OUTPUT_DIR/ca-key.pem" -pkeyopt rsa_keygen_bits:2048
openssl req -new -x509 -key "$OUTPUT_DIR/ca-key.pem" -out "$OUTPUT_DIR/ca.pem" -days 365 \
    -subj "/C=US/ST=California/L=San Francisco/O=CB-MPC/CN=CA Root Certificate"
echo "CA certificate generated"
echo

# Step 2: Generate certificates for each party
for ((i=0; i<NUM_PARTIES; i++)); do
    PEER_LABEL="$(peer_label "$i")"
    PEER_DNS_NAME="$(peer_dns_name "$i")"
    PARTY_DIR="$OUTPUT_DIR/peer-$PEER_LABEL"
    mkdir -p "$PARTY_DIR"

    echo "Step 2.$i: Generating certificate for demo peer '$PEER_LABEL' ($PEER_DNS_NAME)..."

    # Create OpenSSL config for this party
    cat > "$PARTY_DIR/openssl.cnf" <<EOF
[ req ]
default_bits       = 2048
distinguished_name = dn
req_extensions     = req_ext
x509_extensions    = v3_req
prompt             = no

[ dn ]
countryName                = US
stateOrProvinceName        = California
localityName               = San Francisco
organizationName           = CB-MPC
commonName                 = $PEER_DNS_NAME

[ req_ext ]
subjectAltName = @alt_names

[ v3_req ]
subjectAltName = @alt_names

[ alt_names ]
DNS.1 = $PEER_DNS_NAME
DNS.2 = localhost
IP.1  = 127.0.0.1
EOF

    # Generate private key
    openssl genpkey -algorithm RSA -out "$PARTY_DIR/key.pem" -pkeyopt rsa_keygen_bits:2048

    # Generate certificate signing request
    openssl req -new -key "$PARTY_DIR/key.pem" -out "$PARTY_DIR/cert.csr" \
        -config "$PARTY_DIR/openssl.cnf"

    # Sign certificate with CA
    openssl x509 -req -in "$PARTY_DIR/cert.csr" -CA "$OUTPUT_DIR/ca.pem" \
        -CAkey "$OUTPUT_DIR/ca-key.pem" -CAcreateserial -out "$PARTY_DIR/cert.pem" \
        -days 365 -extensions v3_req -extfile "$PARTY_DIR/openssl.cnf"

    # Convert to DER format (for demos compatibility if needed)
    openssl x509 -in "$PARTY_DIR/cert.pem" -outform DER -out "$PARTY_DIR/cert.der"

    # Clean up CSR
    rm "$PARTY_DIR/cert.csr"

    echo "Demo peer '$PEER_LABEL' certificate generated at $PARTY_DIR/"
done

echo
echo "=== Certificate generation complete ==="
echo "Files generated:"
echo "  - CA certificate: $OUTPUT_DIR/ca.pem"
echo "  - CA private key: $OUTPUT_DIR/ca-key.pem"
for ((i=0; i<NUM_PARTIES; i++)); do
    echo "  - Demo peer '$(peer_label "$i")': $OUTPUT_DIR/peer-$(peer_label "$i")/{key.pem,cert.pem}"
done
echo
echo "Keep private keys (*.pem) secure!"
echo "Generated certificates are for local demos/tests only."
echo "Ready for mTLS communication"
