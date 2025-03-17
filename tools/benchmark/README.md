# MPC Engine Benchmark Tool

The benchmark tool is based on the [google/benchmark](https://github.com/google/benchmark/), which is a submodule of this repository.

## Usages

### Manuel benchmarking

Before running the benchmarks, make sure necessary submodules are downloaded, one can do `git pull --recurse-submodules` after cloning.

The following commands should invoke the `Makefile` in the root of the repository.

- `make benchmark-build`: generate an executable in `tools/benchmark/build/MPCEngineBM`
- `make benchmark-run unit={ns|us|ms|s} filter=<filter regexp>`: only output the results to the console. Default unit is `ns`.

For more ways to execute `tools/benchmark/build/MPCEngineBM`, see the [google/benchmark user guide](https://github.com/google/benchmark/blob/main/docs/user_guide.md#command-line)

### Continuous benchmarking

We use GitHub action workflows to run the benchmark automatically whenever `master` branch gets updated.
The workflow will:

1. Run the benchmarks automatically
2. Compare results with previous runs to detect performance degradation
3. Commit benchmark results to the `gh-pages` branch
4. Render the results on the repository's GitHub Pages site
