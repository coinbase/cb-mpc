home:=$(shell pwd)

exe:="build/cbmpc_benchmark"

.PHONY: benchmark-build
benchmark-build:
	${RUN_CMD} 'cd $(home)/tools/benchmark && \
	cmake -Bbuild -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON -DCMAKE_BUILD_TYPE=Release && \
	cmake --build build/ -- -j$(NCORES)'

.PHONY: benchmark-run # e.g. make benchmark-run unit=us filter=ZK
benchmark-run:
	${RUN_CMD} 'cd $(home)/tools/benchmark && \
	$(exe) \
	--benchmark_time_unit=$(unit) \
	--benchmark_filter=$(filter) \
	--benchmark_out=$(bmout) \
	--benchmark_out_format=json'

.PHONY: benchmark-all
benchmark-all:
	${RUN_CMD} 'cd $(home)/tools/benchmark && \
	$(exe) \
	--benchmark_time_unit=ns \
	--benchmark_out=/tmp/all.json \
	--benchmark_out_format=json'

.PHONY: benchmark-update
benchmark-update:
	${RUN_CMD} 'cd $(home)/tools/benchmark && \
	mkdir -p results/$(protocol) && \
	$(exe) \
	--benchmark_time_unit=ns \
	--benchmark_filter=^$(protocol)/ \
	--benchmark_out=$(HOME)/tools/benchmark/results/$(protocol)/result.json \
	--benchmark_out_format=json'

.PHONY: bench-clean
bench-clean:
	${RUN_CMD} 'rm -rf tools/benchmark/build'
