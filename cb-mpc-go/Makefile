.PHONY: help build test sanitize-go lint-install lint vulncheck clean deps examples demos-help demo-certs demo-ecdsa2pc-mtls demo-eddsa-mp-backup-mtls demo-ecdsa-mp-ac-mtls demo-ecdsa-mp-ac-web demo-tdh2-mtls force-build-cpp verify-mods

CB_MPC_DIR ?= ..

# Marker file to track if C++ library is built
CPP_LIB := $(CB_MPC_DIR)/build/install/public/lib/libcbmpc.a
CPP_PUBLIC_HEADER := $(CB_MPC_DIR)/build/install/public/include/cbmpc/c_api/common.h

DEMO_CERT_DIR ?= demos/tmp/certs/ecdsa2pc_mtls
DEMO_CERT_PARTIES ?= 2
DEMO_ECDSA2PC_ADDR := 127.0.0.1:9440
DEMO_EDDSA_MP_CERT_DIR := demos/tmp/certs/eddsa_mp_backup_mtls
DEMO_EDDSA_MP_ADDRS ?= auto
DEMO_ECDSA_MP_AC_CERT_DIR := demos/tmp/certs/ecdsa_mp_ac_mtls
DEMO_ECDSA_MP_AC_ADDRS ?= auto
DEMO_ECDSA_MP_AC_WEB_CERT_DIR := demos/ecdsa_mp_ac_web/tmp/certs
DEMO_ECDSA_MP_AC_WEB_HTTP_ADDRS ?= 127.0.0.1:7080,127.0.0.1:7081,127.0.0.1:7082
DEMO_ECDSA_MP_AC_WEB_TRANSPORT_ADDRS ?= 127.0.0.1:9680,127.0.0.1:9681,127.0.0.1:9682
DEMO_ECDSA_MP_AC_WEB_STATE_DIR ?= demos/ecdsa_mp_ac_web/tmp/state
DEMO_ECDSA_MP_AC_WEB_MODULE_DIR := demos/ecdsa_mp_ac_web
DEMO_TDH2_CERT_DIR := demos/tmp/certs/tdh2_mtls
DEMO_TDH2_ADDRS ?= auto
GOLANGCI_LINT_VERSION ?= v2.11.4
GOLANGCI_LINT_BIN := $(CURDIR)/bin/golangci-lint
GOVULNCHECK_VERSION ?= v1.1.4
REPO_GO_VERSION := $(shell awk '/^go / {print $$2; exit}' go.mod)
GOVULNCHECK_GOTOOLCHAIN ?= go$(REPO_GO_VERSION)+auto

help:
	@echo "Available targets:"
	@echo "  deps         - Initialize the parent cb-mpc vendor submodules"
	@echo "  build-cpp    - Build cb-mpc C++ library (only if needed)"
	@echo "  force-build-cpp - Force rebuild of C++ library"
	@echo "  build        - Build Go wrapper (builds C++ if needed)"
	@echo "  test         - Run Go tests (builds C++ if needed)"
	@echo "  sanitize-go  - Run ASAN/UBSAN coverage for the CGO boundary"
	@echo "  lint-install - Install repo-local golangci-lint $(GOLANGCI_LINT_VERSION)"
	@echo "  lint         - Run the repo-pinned golangci-lint"
	@echo "  verify-mods  - Verify go.mod/go.sum for the root and isolated demo modules"
	@echo "  vulncheck    - Run the repo-pinned govulncheck"
	@echo "  examples     - Run example programs"
	@echo "  demos-help   - Show networked demo workflows"
	@echo "  demo-certs   - Generate demo TLS certificates under $(DEMO_CERT_DIR)"
	@echo "  demo-ecdsa2pc-mtls - Run the local two-process ECDSA-2PC mTLS demo"
	@echo "  demo-eddsa-mp-backup-mtls - Run the local three-process EdDSA-MP backup showcase over mTLS"
	@echo "  demo-ecdsa-mp-ac-mtls - Run the local three-process ECDSA-MP AC showcase over mTLS"
	@echo "  demo-ecdsa-mp-ac-web - Run the local three-party browser-driven ECDSA-MP AC web demo"
	@echo "  demo-tdh2-mtls - Run the local three-process TDH2 showcase over mTLS"
	@echo "  clean        - Clean Go build artifacts"
	@echo "  clean-all    - Clean everything including the parent cb-mpc build"

deps:
	git -C "$(CB_MPC_DIR)" submodule update --init --recursive
	@echo "Parent cb-mpc submodules are initialized"

verify-mods:
	go mod verify
	go -C "$(DEMO_ECDSA_MP_AC_WEB_MODULE_DIR)" mod verify

# Build C++ library + public install if it doesn't exist or is outdated
$(CPP_LIB): $(CPP_PUBLIC_HEADER)

$(CPP_PUBLIC_HEADER):
	@echo "Building C++ library (this may take a few minutes)..."
	bash scripts/build_cpp.sh Release

# Convenience target that checks if rebuild is needed
build-cpp: $(CPP_LIB)
	@echo "✅ C++ library is up to date"

# Force rebuild of C++ library
force-build-cpp:
	@echo "Force rebuilding C++ library..."
	bash scripts/build_cpp.sh Release

# Build Go code (will build C++ if needed)
build: $(CPP_LIB)
	bash scripts/go_with_cpp.sh go build ./...
	bash scripts/go_with_cpp.sh go -C "$(DEMO_ECDSA_MP_AC_WEB_MODULE_DIR)" build ./...

# Run tests (will build C++ if needed)
test: $(CPP_LIB)
	bash scripts/go_with_cpp.sh go test -v -race ./...
	bash scripts/go_with_cpp.sh go -C "$(DEMO_ECDSA_MP_AC_WEB_MODULE_DIR)" test -v -race ./...

sanitize-go:
	bash scripts/sanitize_go.sh

lint-install:
	@mkdir -p "$(dir $(GOLANGCI_LINT_BIN))"
	@if [ ! -x "$(GOLANGCI_LINT_BIN)" ] || ! "$(GOLANGCI_LINT_BIN)" version 2>/dev/null | grep -q "$(GOLANGCI_LINT_VERSION)"; then \
		echo "Installing golangci-lint $(GOLANGCI_LINT_VERSION) to $(GOLANGCI_LINT_BIN)"; \
		tmpdir="$$(mktemp -d)"; \
		trap 'rm -rf "$$tmpdir"' EXIT; \
		curl -sSfL https://golangci-lint.run/install.sh | sh -s -- -b "$$tmpdir" "$(GOLANGCI_LINT_VERSION)" >/dev/null; \
		mv "$$tmpdir/golangci-lint" "$(GOLANGCI_LINT_BIN)"; \
	else \
		echo "✅ golangci-lint $(GOLANGCI_LINT_VERSION) already installed at $(GOLANGCI_LINT_BIN)"; \
	fi

lint: $(CPP_LIB) lint-install
	bash scripts/go_with_cpp.sh "$(GOLANGCI_LINT_BIN)" run ./...
	cd "$(DEMO_ECDSA_MP_AC_WEB_MODULE_DIR)" && bash ../../scripts/go_with_cpp.sh "$(abspath $(GOLANGCI_LINT_BIN))" run ./...

vulncheck: $(CPP_LIB)
	env GOTOOLCHAIN="$(GOVULNCHECK_GOTOOLCHAIN)" bash scripts/go_with_cpp.sh go run golang.org/x/vuln/cmd/govulncheck@$(GOVULNCHECK_VERSION) ./...

examples: $(CPP_LIB)
	@echo "Running ECDSA-2PC example..."
	bash scripts/go_with_cpp.sh go run examples/ecdsa2pc/main.go
	@echo "Running Schnorr-2PC example..."
	bash scripts/go_with_cpp.sh go run examples/schnorr2pc/main.go
	@echo "Running EdDSA-2PC example..."
	bash scripts/go_with_cpp.sh go run examples/eddsa2pc/main.go
	@echo "Running EdDSA-MP backup example..."
	bash scripts/go_with_cpp.sh go run examples/eddsa_mp_backup/main.go
	@echo "Running PVE showcase example..."
	bash scripts/go_with_cpp.sh go run examples/pve_showcase/main.go
	@echo "Running TDH2 example..."
	bash scripts/go_with_cpp.sh go run examples/tdh2/main.go
	@echo "Running PVE-Batch example..."
	bash scripts/go_with_cpp.sh go run examples/pve_batch/main.go

demos-help:
	@echo "Networked reference demos live under demos/ and are not part of 'make examples'."
	@echo "Try:"
	@echo "  make demo-certs"
	@echo "  make demo-ecdsa2pc-mtls"
	@echo "  make demo-eddsa-mp-backup-mtls"
	@echo "  make demo-ecdsa-mp-ac-mtls"
	@echo "  make demo-ecdsa-mp-ac-web"
	@echo "  make demo-tdh2-mtls"
	@echo "Or run the demos manually as documented in demos/README.md."

demo-certs:
	bash scripts/certs/generate_certs.sh "$(DEMO_CERT_PARTIES)" "$(DEMO_CERT_DIR)"

demo-ecdsa2pc-mtls: $(CPP_LIB) demo-certs
	bash scripts/run_demo_ecdsa2pc_mtls.sh "$(DEMO_CERT_DIR)" "$(DEMO_ECDSA2PC_ADDR)"

demo-eddsa-mp-backup-mtls: $(CPP_LIB)
	$(MAKE) demo-certs DEMO_CERT_PARTIES=3 DEMO_CERT_DIR="$(DEMO_EDDSA_MP_CERT_DIR)"
	bash scripts/run_demo_eddsa_mp_backup_mtls.sh "$(DEMO_EDDSA_MP_CERT_DIR)" "$(DEMO_EDDSA_MP_ADDRS)"

demo-ecdsa-mp-ac-mtls: $(CPP_LIB)
	$(MAKE) demo-certs DEMO_CERT_PARTIES=3 DEMO_CERT_DIR="$(DEMO_ECDSA_MP_AC_CERT_DIR)"
	bash scripts/run_demo_ecdsa_mp_ac_mtls.sh "$(DEMO_ECDSA_MP_AC_CERT_DIR)" "$(DEMO_ECDSA_MP_AC_ADDRS)"

demo-ecdsa-mp-ac-web: $(CPP_LIB)
	$(MAKE) demo-certs DEMO_CERT_PARTIES=3 DEMO_CERT_DIR="$(DEMO_ECDSA_MP_AC_WEB_CERT_DIR)"
	bash "$(DEMO_ECDSA_MP_AC_WEB_MODULE_DIR)/run_demo.sh" "$(DEMO_ECDSA_MP_AC_WEB_CERT_DIR)" "$(DEMO_ECDSA_MP_AC_WEB_HTTP_ADDRS)" "$(DEMO_ECDSA_MP_AC_WEB_TRANSPORT_ADDRS)" "$(DEMO_ECDSA_MP_AC_WEB_STATE_DIR)"

demo-tdh2-mtls: $(CPP_LIB)
	$(MAKE) demo-certs DEMO_CERT_PARTIES=3 DEMO_CERT_DIR="$(DEMO_TDH2_CERT_DIR)"
	bash scripts/run_demo_tdh2_mtls.sh "$(DEMO_TDH2_CERT_DIR)" "$(DEMO_TDH2_ADDRS)"

clean:
	go clean ./...
	go -C "$(DEMO_ECDSA_MP_AC_WEB_MODULE_DIR)" clean ./...

clean-all: clean
	$(MAKE) -C "$(CB_MPC_DIR)" clean || true
