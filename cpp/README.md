# C++ research tools

The lab uses [polymarket-cpp-client](https://github.com/SebastianBoehler/polymarket-cpp-client)
**v3.0.0**, with C++20 and CMake 3.22+. Install libcurl, OpenSSL, zlib, and Git.

## Build and test

Run from the repository root:

```bash
cmake -S cpp -B cpp/build -DCMAKE_BUILD_TYPE=Release
cmake --build cpp/build --parallel 4
ctest --test-dir cpp/build --output-on-failure
```

The default build includes all executables and the offline strategy test.
Release builds use native CPU optimizations; build on the machine where the
binaries will run. CMake checks whether link-time optimization is supported.

## Research scanner

`discovery_mode` fetches public markets and books, evaluates example signals,
and emits JSONL with paper projections. It does not place orders.

```bash
./cpp/build/discovery_mode --help
./cpp/build/discovery_mode --max 50 --iterations 1
./cpp/build/discovery_mode --15m --duration-min 30 \
  --interval-ms 30000 --out data/discovery.jsonl
```

Signals include `binary_arbitrage` and `market_making_candidate`. The calculations
check visible prices and depth, not a complete fee or execution model. Treat
reported edge as a research input, not realized profit.

## Limitless and Opinion

Build `research_venues` with the other C++ targets, or run
`cmake --build cpp/build --target research_venues` after configuration.
The scanner examines one catalogue page, up to the specified limit (1–20).
Opinion orders its sample by 24-hour volume. Both scans skip entries without
distinct YES and NO token IDs.

```bash
mkdir -p data
./cpp/build/research_venues limitless 10 > data/limitless.jsonl
./cpp/build/research_venues opinion 10 > data/opinion.jsonl
```

Each JSONL record identifies the market and collateral and includes both books,
receipt times in milliseconds since the Unix epoch, and per-market quote
metrics when the snapshot has usable prices. Source timestamps appear when the
venue supplies them. Limitless publishes a single YES book. The SDK derives
the NO book by reflecting prices and swapping bids and asks. Opinion fetches
the two token books separately, so their timestamps can differ.

`yes_spread` is YES ask minus YES bid. `bundle_ask` is YES ask plus NO ask.
Both use quote-token units per share. Missing prices or crossed books yield
null metrics. The terminal summary reports how many catalogue entries the
scanner examined, how many it recorded, how many had complete quotes, and the
mean YES spread.
Request and validation errors appear on stderr and cause a nonzero exit.
JSONL can contain records from successful requests in a partial scan.

The scanner uses [limitless-cpp-client](https://github.com/SebastianBoehler/limitless-cpp-client)
and [opinion-cpp-client](https://github.com/SebastianBoehler/opinion-cpp-client)
through their read-only REST methods. The Opinion scanner uses the public
host at `openapi.opinion.trade`.

## Execution experiments

`polymarket_arb`, `arb_test`, `batch_order_smoke`, `ladder_accumulation`,
`avg_in_test`, `order_test`, `order_benchmark`, and `position_smoke` contain
credentialed or execution-related code. `clob_client_test` requests live API data.
These are executable names, not offline CTest registrations.

Several experiments construct orders manually or use older exchange addresses.
They require a separate CLOB V2 migration and validation before live use.
An SDK upgrade or a passing compilation does not establish execution correctness.
See the SDK's [migration guide](https://github.com/SebastianBoehler/polymarket-cpp-client/blob/main/docs/clob-v2-migration.md).

Do not infer dry-run behavior from an executable's name. Inspect its argument
and environment handling before running it. Offline CI runs only the registered
`example_strategies_test` through CTest.

## Updating the SDK

```bash
bun run cpp:update-client
POLYMARKET_CLIENT_GIT_TAG=<tag-or-commit> bun run cpp:update-client
```

The helper clears the cached SDK checkout and rebuilds the research scanner and
main executable. Run the full build and CTest suite when changing the pinned ref.
Use namespaced public headers, for example `<polymarket/market_fetcher.hpp>`.

For contribution rules and compatibility boundaries, see
[CONTRIBUTING.md](../CONTRIBUTING.md) and [docs/maintenance.md](../docs/maintenance.md).
