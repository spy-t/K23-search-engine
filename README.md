# K23 SEARCH

## [ΓΙΑ ΤΟΥΣ ΕΞΕΤΑΣΤΕΣ](./docs/REPORTS.md)

## Dependencies

- meson
- ninja
- gcovr (only if you need test coverage)
- For profiling:
	-	[flamegraph-rs](https://github.com/flamegraph-rs/flamegraph)
	-	linux-perf

## Meson how-to

First you need to initialize meson using

```bash
meson build
```

If you change something to the `meson.build` file run

```bash
ninja -C build reconfigure
# Or if that doesn't work
meson setup --wipe build/
```

To change configuration options use `meson configure -Doption=value build`.
The available built-in options are [here](https://mesonbuild.com/Builtin-options.html)

### Compiling

```bash
meson compile -C build
```

### Testing

```bash
meson test -C build # Run all tests
meson test vector_test -C build # Run a specific test
meson test -C build --print-errorlogs # Run all tests with stdout/err output
meson test -C build --gdb # Debug the tests
```

### Profiling

To profile an executable and generate a flamegraph run
```bash
flamegraph -o flamegraph.svg ./path/to/executable
$BROWSER flamegraph.svg
```

### Extra

To compile with `asan` support use
```bash
meson configure -Db_sanitize=address
```

To generate the coverage reports you need `gcovr` in your `$PATH`
```bash
meson configure -Db_coverage=true build # Make sure you have run this first
ninja -C build coverage
```

To format the project use

```bash
ninja -C build clang-format
```

To lint the project and scan for common mistakes run

```bash
ninja -C build scan-build
```

## Micro benchmarks


| threshold   | order    | time   |
|-------------|----------|--------|
| 0           | random   | 2s     |
| 0           | min      | 2s     |
| 0           | max      | 2s     |
| ----------- | -------- | ------ |
| 1           | random   | 8s     |
| 1           | min      | 7s     |
| 1           | max      | 7s     |
| ----------- | -------- | ------ |
| 2           | random   | 17s    |
| 2           | min      | 14s    |
| 2           | max      | 13s    |
| ----------- | -------- | ------ |
| 3           | random   | 27s    |
| 3           | min      | 22s    |
| 3           | max      | 21s    |

So the winner by a very small margin is max ordering (may matter for very large thresholds)
