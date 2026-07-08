# Coverage settings (Clang/LLVM instrumentation).
COVERAGE_BUILD_DIR ?= build/coverage
COVERAGE_BUILD_TYPE ?= Debug
COVERAGE_CMAKE_ARGS ?= -DCBMPC_PLATFORM_DEP_OUTPUT_DIR=ON
COVERAGE_TEST_LABEL ?= unit
COVERAGE_TEST_NCORES ?= $(TEST_NCORES)
COVERAGE_PROFILE_DIR ?= $(COVERAGE_BUILD_DIR)/profiles
COVERAGE_PROFDATA ?= $(COVERAGE_BUILD_DIR)/coverage.profdata
COVERAGE_TEST_BINARY ?=
COVERAGE_TEST_BINARY_NAME ?= cbmpc_test_unit
COVERAGE_HTML_DIR ?= $(COVERAGE_BUILD_DIR)/html
COVERAGE_IGNORE_REGEX ?= .*/(tests|vendors|_deps)/.*|/usr/.*
LLVM_PROFDATA ?= $(shell command -v llvm-profdata 2>/dev/null || command -v llvm-profdata-20 2>/dev/null || true)
LLVM_COV ?= $(shell command -v llvm-cov 2>/dev/null || command -v llvm-cov-20 2>/dev/null || true)

.PHONY: coverage
coverage:
	@if [ -z "$(LLVM_PROFDATA)" ]; then \
		echo "coverage: llvm-profdata not found; install LLVM or set LLVM_PROFDATA=/path/to/llvm-profdata"; \
		exit 1; \
	fi
	@if [ -z "$(LLVM_COV)" ]; then \
		echo "coverage: llvm-cov not found; install LLVM or set LLVM_COV=/path/to/llvm-cov"; \
		exit 1; \
	fi
	${RUN_CMD} \
	'set -e; \
	src_dir="$$(pwd -P)"; \
	cache_path="$(COVERAGE_BUILD_DIR)/CMakeCache.txt"; \
	if [ -f "$$cache_path" ] && ! grep -Fq "CMAKE_HOME_DIRECTORY:INTERNAL=$$src_dir" "$$cache_path"; then \
		echo "coverage: removing stale build dir '$(COVERAGE_BUILD_DIR)' (source dir mismatch)"; \
		rm -rf "$(COVERAGE_BUILD_DIR)"; \
	fi; \
	rm -rf "$(COVERAGE_PROFILE_DIR)" "$(COVERAGE_PROFDATA)"; \
	mkdir -p "$(COVERAGE_PROFILE_DIR)"; \
	cmake -B "$(COVERAGE_BUILD_DIR)" -DCMAKE_BUILD_TYPE="$(COVERAGE_BUILD_TYPE)" -DBUILD_TESTS=ON -DBUILD_DUDECT=OFF -DCBMPC_TEST_COVERAGE=ON \
	$(COVERAGE_CMAKE_ARGS) && \
	LLVM_PROFILE_FILE="$(abspath $(COVERAGE_PROFILE_DIR))/build-%p-%m.profraw" \
	cmake --build "$(COVERAGE_BUILD_DIR)" -- -j$(CMAKE_NCORES) && \
	rm -f "$(COVERAGE_PROFILE_DIR)"/build-*.profraw'
	${RUN_CMD} \
	'LLVM_PROFILE_FILE="$(abspath $(COVERAGE_PROFILE_DIR))/%p-%m.profraw" \
	ctest --output-on-failure -j$(COVERAGE_TEST_NCORES) --test-dir "$(COVERAGE_BUILD_DIR)" \
	$(if $(COVERAGE_TEST_LABEL),-L "$(COVERAGE_TEST_LABEL)") \
	$(if $(filter),-R $(filter))'
	${RUN_CMD} \
	'"$(LLVM_PROFDATA)" merge -sparse "$(COVERAGE_PROFILE_DIR)"/*.profraw -o "$(COVERAGE_PROFDATA)"'
	${RUN_CMD} \
	'test_binary="$(COVERAGE_TEST_BINARY)"; \
	if [ -z "$$test_binary" ]; then \
		for candidate in "$(COVERAGE_BUILD_DIR)/bin/$(COVERAGE_BUILD_TYPE)/$(COVERAGE_TEST_BINARY_NAME)" "$(COVERAGE_BUILD_DIR)"/bin/"$(COVERAGE_BUILD_TYPE)"/*/"$(COVERAGE_TEST_BINARY_NAME)"; do \
			if [ -f "$$candidate" ]; then test_binary="$$candidate"; break; fi; \
		done; \
	fi; \
	if [ ! -f "$$test_binary" ]; then \
		echo "coverage: test binary not found; set COVERAGE_TEST_BINARY=/path/to/$(COVERAGE_TEST_BINARY_NAME)"; \
		exit 1; \
	fi; \
	"$(LLVM_COV)" report "$$test_binary" --instr-profile="$(COVERAGE_PROFDATA)" --ignore-filename-regex="$(COVERAGE_IGNORE_REGEX)"'

.PHONY: coverage-html
coverage-html: coverage
	${RUN_CMD} \
	'rm -rf "$(COVERAGE_HTML_DIR)" && \
	test_binary="$(COVERAGE_TEST_BINARY)"; \
	if [ -z "$$test_binary" ]; then \
		for candidate in "$(COVERAGE_BUILD_DIR)/bin/$(COVERAGE_BUILD_TYPE)/$(COVERAGE_TEST_BINARY_NAME)" "$(COVERAGE_BUILD_DIR)"/bin/"$(COVERAGE_BUILD_TYPE)"/*/"$(COVERAGE_TEST_BINARY_NAME)"; do \
			if [ -f "$$candidate" ]; then test_binary="$$candidate"; break; fi; \
		done; \
	fi; \
	if [ ! -f "$$test_binary" ]; then \
		echo "coverage: test binary not found; set COVERAGE_TEST_BINARY=/path/to/$(COVERAGE_TEST_BINARY_NAME)"; \
		exit 1; \
	fi; \
	"$(LLVM_COV)" show "$$test_binary" --instr-profile="$(COVERAGE_PROFDATA)" --format=html -o "$(COVERAGE_HTML_DIR)" --ignore-filename-regex="$(COVERAGE_IGNORE_REGEX)" && \
	echo "coverage: HTML report written to $(COVERAGE_HTML_DIR)/index.html"'
