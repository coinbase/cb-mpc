DOCKER_IMAGE_NAME := cb-mpc
RUN_CMD := bash -c
.DEFAULT_GOAL := ghas
BUILD_TYPE ?= Release

# Local install layout (no sudo by default)
CBMPC_INSTALL_ROOT ?= $(CURDIR)/build/install
CBMPC_PREFIX_PUBLIC ?= $(CBMPC_INSTALL_ROOT)/public
CBMPC_PREFIX_FULL ?= $(CBMPC_INSTALL_ROOT)/full
CMAKE_NCORES := $(shell \
	if [ -n "$${ARG_CMAKE_NCORES}" ]; then \
		echo "$${ARG_CMAKE_NCORES}"; \
    elif command -v nproc >/dev/null 2>&1; then \
        nproc; \
    elif [ "$$(uname)" = "Darwin" ]; then \
        sysctl -n hw.ncpu; \
    fi)

TEST_NCORES := $(shell \
	if [ -n "$${ARG_TEST_NCORES}" ]; then \
		echo "$${ARG_TEST_NCORES}"; \
    elif command -v nproc >/dev/null 2>&1; then \
        nproc; \
    elif [ "$$(uname)" = "Darwin" ]; then \
        sysctl -n hw.ncpu; \
    fi)
TEST_REPEAT ?= 1

# clang-format targets
CLANG_FORMAT_PATHS := src tests include include-internal
CLANG_FORMAT_FILE_FILTER := \( -name '*.cpp' -o -name '*.h' \)
CLANG_FORMAT_FIND := find $(CLANG_FORMAT_PATHS) -type f $(CLANG_FORMAT_FILE_FILTER)

# Sanitizer settings (mirrors the CI "sanitizers" workflow job).
# IMPORTANT: CMake caches the *absolute* source dir in CMakeCache.txt.
# If you reuse the same build dir from both host and Docker, you'll hit:
#   "CMakeCache.txt directory ... is different than the directory ... where it was created"
# because the repo path differs (e.g. /Users/... vs /code).
#
# Default to a docker-specific build dir when running inside Docker so the two
# environments can coexist without `make clean`.
_CBMPC_IN_DOCKER := $(shell if [ -f /.dockerenv ]; then echo 1; else echo 0; fi)
SANITIZE_BUILD_DIR ?= $(if $(filter 1,$(_CBMPC_IN_DOCKER)),build/sanitize-docker,build/sanitize)
SANITIZE_BUILD_TYPE ?= Debug
# -fno-sanitize=enum: allows tests that intentionally pass invalid enum values via the C API.
SANITIZE_CFLAGS ?= -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize=enum
SANITIZE_CXXFLAGS ?= -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize=enum
SANITIZE_LDFLAGS ?= -fsanitize=address,undefined
# Avoid cross-build-dir clobbering: the project archives into `${ROOT_DIR}/lib/${CMAKE_BUILD_TYPE}` by default,
# which is shared between host and Docker (and between different Debug variants). For sanitizer builds, enable
# platform-dependent library output dirs so Linux vs macOS (and arm64 vs x86_64) can coexist.
SANITIZE_CMAKE_ARGS ?= -DCBMPC_PLATFORM_DEP_OUTPUT_DIR=ON
SANITIZE_TEST_LABEL ?= unit
SANITIZE_TEST_NCORES ?= 1
SANITIZE_ASAN_OPTIONS ?= detect_leaks=0
SANITIZE_UBSAN_OPTIONS ?= print_stacktrace=1:halt_on_error=1

.PHONY: ghas
ghas: submodules openssl-linux build

.PHONY: submodules
submodules:
	git submodule update --init --recursive

# OpenSSL build targets
# Note: These require write permission to /usr/local/opt
# You can customize the install location by setting CBMPC_OPENSSL_ROOT:
#   export CBMPC_OPENSSL_ROOT=/custom/path
#   or
#   cmake -DCBMPC_OPENSSL_ROOT=/custom/path ...

.PHONY: openssl-linux
openssl-linux:
	@echo "Building custom OpenSSL for Linux..."
	${RUN_CMD} 'bash ./scripts/openssl/build-static-openssl-linux.sh'
	${RUN_CMD} 'mkdir -p /usr/local/lib64'
	${RUN_CMD} 'mkdir -p /usr/local/lib'
	${RUN_CMD} 'mkdir -p /usr/local/include'

.PHONY: openssl-macos
openssl-macos:
	@echo "Building custom OpenSSL for macOS (x86_64)..."
	@echo "Note: Requires sudo permission for /usr/local/opt"
	${RUN_CMD} 'bash ./scripts/openssl/build-static-openssl-macos.sh'

.PHONY: openssl-macos-m1
openssl-macos-m1:
	@echo "Building custom OpenSSL for macOS (ARM64)..."
	@echo "Note: Requires sudo permission for /usr/local/opt"
	${RUN_CMD} 'bash ./scripts/openssl/build-static-openssl-macos-m1.sh'

.PHONY: openssl
openssl:
	@if [ "$$(uname)" = "Darwin" ]; then \
		if [ "$$(uname -m)" = "arm64" ]; then \
			echo "Detected macOS ARM64"; \
			$(MAKE) openssl-macos-m1; \
		else \
			echo "Detected macOS x86_64"; \
			$(MAKE) openssl-macos; \
		fi; \
	elif [ "$$(uname)" = "Linux" ]; then \
		echo "Detected Linux"; \
		$(MAKE) openssl-linux; \
	else \
		echo "Unsupported platform: $$(uname)"; \
		exit 1; \
	fi

.PHONY: docker-run
docker-run:
	@echo "To run inside docker, you can run do the following"
	@echo "make image"
	@echo "docker run -it --rm -v $(shell pwd):/code -t ${DOCKER_IMAGE_NAME} bash -c 'make XXX'"

.PHONY: image
image:
	docker build -t ${DOCKER_IMAGE_NAME} .

.PHONY: lint-fix
lint-fix:
	$(CLANG_FORMAT_FIND) -exec clang-format -i {} +

.PHONY: lint
lint:
	@output="$$( $(CLANG_FORMAT_FIND) -exec clang-format -n {} + 2>&1 )"; \
	if [ -n "$$output" ]; then \
		echo "$$output"; \
		exit 1; \
	fi

.PHONY: build
build: BUILD_TYPE = Release# (Release/Debug/RelWithDebInfo)
build:
	${RUN_CMD} 'cmake -B build/$(BUILD_TYPE) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DBUILD_TESTS=ON && \
	cmake --build build/$(BUILD_TYPE) -- -j$(CMAKE_NCORES)'

.PHONY: build-no-test
build-no-test: BUILD_TYPE = Release# (Release/Debug/RelWithDebInfo)
build-no-test:
	${RUN_CMD} \
	'cmake -B build/$(BUILD_TYPE) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DBUILD_TESTS=OFF && \
	cmake --build build/$(BUILD_TYPE) -- -j$(CMAKE_NCORES)'

.PHONY: build-with-dudect
build-with-dudect: BUILD_TYPE = Release# (Release/Debug/RelWithDebInfo)
build-with-dudect:
	${RUN_CMD} 'cmake -B build/$(BUILD_TYPE) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DBUILD_TESTS=ON -DBUILD_DUDECT=ON && \
	cmake --build build/$(BUILD_TYPE) -- -j$(CMAKE_NCORES)'

.PHONY: test # e.g. make test filter=ED25519
test: BUILD_TYPE = Release# (Release/Debug/RelWithDebInfo)
test: TEST_LABEL =unit|integration# (unit|integration)
test:
	$(MAKE) build BUILD_TYPE=$(BUILD_TYPE)
	${RUN_CMD} \
	'ctest --output-on-failure --repeat until-fail:$(TEST_REPEAT) -j$(TEST_NCORES) --test-dir build/$(BUILD_TYPE) \
	$(if $(TEST_LABEL),-L "$(TEST_LABEL)") \
	$(if $(filter),-R $(filter))'

.PHONY: dev
dev:
	$(MAKE) test BUILD_TYPE=Debug TEST_LABEL=integration

.PHONY: full-test
full-test:
	$(MAKE) test BUILD_TYPE=Debug TEST_LABEL=unit

.PHONY: dudect
dudect:
	$(MAKE) build-with-dudect BUILD_TYPE=Release
	${RUN_CMD} \
	'ctest --output-on-failure --repeat until-fail:$(TEST_REPEAT) -j1 --test-dir build/Release \
	-L "dudect" \
	-E DUDECT_VT \
	$(if $(filter),-R $(filter))'

.PHONY: sanitize
sanitize:
	${RUN_CMD} \
	'set -e; \
	src_dir="$$(pwd -P)"; \
	cache_path="$(SANITIZE_BUILD_DIR)/CMakeCache.txt"; \
	if [ -f "$$cache_path" ] && ! grep -Fq "CMAKE_HOME_DIRECTORY:INTERNAL=$$src_dir" "$$cache_path"; then \
		echo "sanitize: removing stale build dir '$(SANITIZE_BUILD_DIR)' (source dir mismatch)"; \
		rm -rf "$(SANITIZE_BUILD_DIR)"; \
	fi; \
	cmake -B "$(SANITIZE_BUILD_DIR)" -DCMAKE_BUILD_TYPE="$(SANITIZE_BUILD_TYPE)" -DBUILD_TESTS=ON -DBUILD_DUDECT=OFF \
	$(SANITIZE_CMAKE_ARGS) \
	-DCMAKE_C_FLAGS="$(SANITIZE_CFLAGS)" -DCMAKE_CXX_FLAGS="$(SANITIZE_CXXFLAGS)" \
	-DCMAKE_EXE_LINKER_FLAGS="$(SANITIZE_LDFLAGS)" -DCMAKE_SHARED_LINKER_FLAGS="$(SANITIZE_LDFLAGS)" && \
	cmake --build "$(SANITIZE_BUILD_DIR)" -- -j$(CMAKE_NCORES)'
	${RUN_CMD} \
	'ASAN_OPTIONS="$(SANITIZE_ASAN_OPTIONS)" UBSAN_OPTIONS="$(SANITIZE_UBSAN_OPTIONS)" \
	ctest --output-on-failure --repeat until-fail:$(TEST_REPEAT) -j$(SANITIZE_TEST_NCORES) --test-dir "$(SANITIZE_BUILD_DIR)" \
	-L "$(SANITIZE_TEST_LABEL)" \
	$(if $(filter),-R $(filter))'

.PHONY: sanitize-docker
sanitize-docker:
	$(MAKE) submodules
	$(MAKE) image
	docker run --rm -v $(shell pwd):/code -t ${DOCKER_IMAGE_NAME} bash -c 'make sanitize'

.PHONY: clean
clean:
	${RUN_CMD} 'rm -rf build'
	${RUN_CMD} 'rm -rf lib'

### Install the C++ library to local (this is necessary for demo and benchmark)
.PHONY: install
install:
	$(MAKE) build-no-test BUILD_TYPE=$(BUILD_TYPE)
	${RUN_CMD} 'scripts/install.sh --mode public --build-type $(BUILD_TYPE) --prefix "$(CBMPC_PREFIX_PUBLIC)"'

.PHONY: install-full
install-full:
	$(MAKE) build-no-test BUILD_TYPE=$(BUILD_TYPE)
	${RUN_CMD} 'scripts/install.sh --mode full --build-type $(BUILD_TYPE) --prefix "$(CBMPC_PREFIX_FULL)"'

.PHONY: install-all
install-all:
	$(MAKE) build-no-test BUILD_TYPE=$(BUILD_TYPE)
	${RUN_CMD} 'scripts/install.sh --mode public --build-type $(BUILD_TYPE) --prefix "$(CBMPC_PREFIX_PUBLIC)"'
	${RUN_CMD} 'scripts/install.sh --mode full --build-type $(BUILD_TYPE) --prefix "$(CBMPC_PREFIX_FULL)"'

.PHONY: uninstall
uninstall:
	${RUN_CMD} 'rm -rf "$(CBMPC_INSTALL_ROOT)"'

### Demos
.PHONY: demo
demo: install-all
	${RUN_CMD} 'CBMPC_PREFIX_PUBLIC="$(CBMPC_PREFIX_PUBLIC)" CBMPC_PREFIX_FULL="$(CBMPC_PREFIX_FULL)" BUILD_TYPE="$(BUILD_TYPE)" bash scripts/run-demos.sh --run-all'

.PHONY: demos
demos: demo

.PHONY: clean-demos
clean-demos:
	${RUN_CMD} 'bash ./scripts/run-demos.sh --clean'

### Benchmarks
include tools/benchmark/benchmark.makefile

.PHONY: benchmark-build
benchmark-build: install-full

.PHONY: bench
bench:
	$(MAKE) benchmark-build
	$(MAKE) benchmark-run unit=us

.PHONY: clean-bench
clean-bench:
	$(MAKE) bench-clean

.PHONY: sanity-check
sanity-check:
	set -e
	$(MAKE) build
	sudo $(MAKE) install-full
	docker run -it --rm -v $(shell pwd):/code -t ${DOCKER_IMAGE_NAME} bash -c 'make lint'
	$(MAKE) demos
	$(MAKE) test
	$(MAKE) benchmark-build
	$(MAKE) dudect filter=NON_EXISTING_TEST

