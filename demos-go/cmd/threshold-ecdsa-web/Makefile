.PHONY: generate-ca
generate-ca:
	@echo "Generating CA RSA key..."
	openssl genpkey -algorithm RSA -out certs/ca-key.pem -pkeyopt rsa_keygen_bits:2048
	@echo "Generating CA certificate..."
	openssl req -new -x509 -key certs/ca-key.pem -out certs/ca.pem -days 365 -subj "/C=US/ST=California/L=San Francisco/O=My Company/CN=CA Root Certificate"

.PHONY: generate-cert
generate-cert:
	@echo "Generating RSA key with index $(INDEX)..."
	openssl genpkey -algorithm RSA -out certs/party-$(INDEX)/key-$(INDEX).pem -pkeyopt rsa_keygen_bits:2048
	@echo "Generating CSR with index $(INDEX)..."
	openssl req -new -key certs/party-$(INDEX)/key-$(INDEX).pem -out certs/party-$(INDEX)/cert-$(INDEX).csr -config certs/party-$(INDEX)/openssl-$(INDEX).cnf
	# @echo "Generating self-signed certificate with index $(INDEX)..."
	# openssl x509 -req -in certs/party-$(INDEX)/cert-$(INDEX).csr -signkey certs/party-$(INDEX)/key-$(INDEX).pem -out unsigned-cert-$(INDEX).pem -days 365
	@echo "Signing certificate with CA..."
	openssl x509 -req -in certs/party-$(INDEX)/cert-$(INDEX).csr -CA certs/ca.pem -CAkey certs/ca-key.pem -CAcreateserial -out certs/party-$(INDEX)/cert-$(INDEX).pem -days 365 -extensions v3_req -extfile certs/party-$(INDEX)/openssl-$(INDEX).cnf
	@echo "Certificate signed and saved as certs/party-$(INDEX)/cert-$(INDEX).pem"
	@echo "Converting cert-$(INDEX).pem to ASN.1 format..."
	openssl x509 -in certs/party-$(INDEX)/cert-$(INDEX).pem -outform DER -out certs/party-$(INDEX)/cert-$(INDEX).der
	@echo "Conversion complete. ASN.1 format saved as certs/party-$(INDEX)/cert-$(INDEX).der"

.PHONY: certs
certs:
	make clean-all
	mkdir -p certs certs/party-0 certs/party-1 certs/party-2 certs/party-3
	make generate-ca
	make generate-cert INDEX=0
	make generate-cert INDEX=1
	make generate-cert INDEX=2
	make generate-cert INDEX=3

.PHONY: clean-all
clean-all:
	rm -rf certs/*/{*.pem,*.csr,*.srl,*.der}
	make clean-logs
	rm -f demo
	rm -rf keyshare_party*
	rm -f threshold.txt
	rm -rf tmp

.PHONY: clean-logs
clean-logs:
	rm -rf *.log

.PHONY: clean-processes
clean-processes:
	lsof -ti tcp:8080 | xargs -r kill -TERM
	lsof -ti tcp:8081 | xargs -r kill -TERM
	lsof -ti tcp:8082 | xargs -r kill -TERM
	lsof -ti tcp:8083 | xargs -r kill -TERM

.PHONY: run-dkg
run-dkg:
	make clean-processes
	make clean-logs
	go run *.go -index=0 -phase=dkg -mode=cli -participants=0,1,2,3 -threshold=3 > dkg-0.log 2>&1 &
	sleep 5 && go run *.go -index=1 -phase=dkg -mode=cli -participants=0,1,2,3 -threshold=3 > dkg-1.log 2>&1 &
	sleep 10 && go run *.go -index=2 -phase=dkg -mode=cli -participants=0,1,2,3 -threshold=3 > dkg-2.log 2>&1 &
	sleep 15 && go run *.go -index=3 -phase=dkg -mode=cli -participants=0,1,2,3 -threshold=3 > dkg-3.log 2>&1 &

.PHONY: run-sign
run-sign:
	make clean-processes
	make clean-logs
	go run *.go -index=0 -phase=sign -mode=cli -participants=0,2,3 -threshold=3 > sign-0.log 2>&1 &
	sleep 5 && go run *.go -index=2 -phase=sign -mode=cli -participants=0,2,3 -threshold=3 > sign-2.log 2>&1 &
	sleep 10 && go run *.go -index=3 -phase=sign -mode=cli -participants=0,2,3 -threshold=3 > sign-3.log 2>&1 &

.PHONY: run-server
run-server:
	cd ../../../ && BUILD_TYPE=Release bash scripts/go_with_cpp.sh --no-cd bash -lc "cd 'demos-go/cmd/threshold-ecdsa-web' && env CGO_ENABLED=1 go run *.go -index=$(INDEX)"