# Backtesting Engine

An event-driven backtesting engine built from scratch in C++20. It replays historical OHLCV market data through a pluggable strategy interface, producing fills and a marked-to-market equity curve — then goes further into systems-level performance: a cache-friendly Structure-of-Arrays data layout, AVX2 SIMD-vectorised indicators, and multi-threaded parameter sweeps with core pinning.

Built as a deliberate learning progression from a [portfolio optimisation engine](https://github.com/yourusername/portfolio-engine) — the optimiser answers "what should the portfolio look like?", the backtester answers "how would that strategy have actually performed?"

**Headline results**

| Optimisation | Speedup |
|---|---|
| AoS → SoA data layout | 1.5–7.9× per indicator |
| AoS → AVX2 SIMD (end-to-end) | 12–20× per indicator |
| Sequential → parallel parameter sweep | ~12.6× throughput |

Every optimisation is gated by correctness: SIMD results are tested against scalar baselines before any speedup is reported.

---

## Architecture

```
Historical CSV → DataLoader → std::vector<Bar>
                                    │
                        ┌───────────┘
                        ▼
                   Event Loop (bar by bar)
                        │
                        ├── 1. Fill pending orders at current bar's open
                        ├── 2. Mark-to-market at current bar's close
                        └── 3. Strategy receives MarketView → emits Orders
                                    │
                                    ▼
                              Pending orders held for next bar
```

**Key design decisions**

- **Event-driven, not vectorised.** The engine steps through time one bar at a time, structurally preventing look-ahead bias rather than relying on discipline.
- **MarketView as the look-ahead boundary.** The strategy receives a view that physically cannot access data beyond the current timestamp. Accessing future data throws `std::out_of_range` — and a regression test proves it.
- **Fill at next bar's open.** Orders emitted on bar *t* fill at bar *t+1*'s open price. No ambiguity about whether the strategy "knew" the fill price.
- **Strategy interface via abstract base class.** Any strategy inherits from `Strategy` and implements `onbar(const MarketView&)`. The engine is decoupled from any specific trading logic.
- **Independent units parallelise; single computations vectorise.** SIMD speeds up each indicator call; threads run whole backtests side by side. The two never compete.

---

## Components

| File | Purpose |
|------|---------|
| `Bar.h` | OHLCV bar struct with `int64_t` epoch timestamp |
| `DataLoader` | CSV parser → `std::vector<Bar>` with chronological order validation |
| `MarketView` | Const `std::span` view over bar data with bounds-checked access |
| `Strategy` | Abstract base class — pure virtual `onbar` interface |
| `Backtester` | Event loop: fills pending orders, runs strategy, records equity |
| `Portfolio` | Cash, position tracking, mark-to-market, equity curve |
| `Order` | Side (Buy/Sell) and quantity |
| `MarketDataSoA.h` | Structure-of-Arrays layout — one contiguous array per field |
| `Indicators` | SMA, VWAP, rolling volatility in three versions: AoS scalar, SoA scalar, AVX2 SIMD |
| `SweepTask` | Parallel parameter sweep — one pinned thread per (short, long) window pair |
| `benchmarks/bench_main.cpp` | Timing harness comparing AoS, SoA and SIMD across data sizes |

## Strategies included

- **BuyAndHold** — buys one share on the first bar, holds indefinitely. Baseline validation.
- **SMA Crossover** — configurable short/long window moving-average crossover. Buys on bullish cross, sells on bearish cross, with position tracking to prevent stacking.

---

## Performance

### 1. Data-oriented design — AoS → SoA

The original layout stores each bar as one 48-byte struct (Array of Structs). Computing an SMA only needs `close`, but each load pulls the whole bar into cache — 8 useful bytes out of every 48.

The SoA layout stores each field as its own contiguous array. A loop over `closes[]` uses every byte of every 64-byte cache line.

```
AoS:  [t|o|h|l|c|v][t|o|h|l|c|v][t|o|h|l|c|v] ...
SoA:  closes:  [c0|c1|c2|c3|c4|c5|c6|c7] ...
      volumes: [v0|v1|v2|v3|v4|v5|v6|v7] ...
```

### 2. SIMD vectorisation — AVX2

AVX2 registers are 256 bits wide: one instruction operates on 4 doubles at once. SoA makes this possible — `_mm256_loadu_pd` needs 4 *contiguous* doubles, which SoA provides and AoS does not.

- **SMA** — one vector accumulator, horizontal sum, scalar tail for `window % 4` leftovers
- **VWAP** — two accumulators in parallel (Σ close×volume and Σ volume)
- **Rolling volatility** — three vectorised passes (returns, mean, variance) over a stack-allocated returns buffer, eliminating the per-call heap allocation that capped the SoA-only speedup at 1.5×

### Indicator benchmarks

Release build, window = 50, 100,000 calls per measurement, nanoseconds per call:

```
Data Size   | SMA AoS | SMA SoA | SMA SIMD | VWAP AoS | VWAP SoA | VWAP SIMD | Vol AoS | Vol SoA | Vol SIMD
------------|---------|---------|----------|----------|----------|-----------|---------|---------|----------
2,500       |   141   |   26    |    11    |   350    |    56    |    16     |   974   |   700   |    72
10,000      |   133   |   27    |     9    |   338    |    47    |    26     |   972   |   661   |    50
100,000     |   141   |   26    |    11    |   335    |    46    |    16     |   956   |   658   |    58
500,000     |   130   |   27    |    11    |   344    |    63    |    21     |   913   |   678   |    60
1,000,000   |   134   |   26    |    11    |   364    |    52    |    17     |   952   |   660   |    50
```

| Indicator | AoS (ns) | SoA (ns) | SIMD (ns) | AoS → SoA | AoS → SIMD |
|-----------|----------|----------|-----------|-----------|------------|
| SMA | ~134 | ~26 | ~11 | 5.2× | **12×** |
| VWAP | ~346 | ~52 | ~17 | 6.7× | **20×** |
| Rolling vol | ~953 | ~671 | ~56 | 1.4× | **17×** |

Rolling volatility is the instructive case: the layout change alone barely helped because a heap allocation on every call dominated the cost. The bottleneck only became visible by measuring — and removing it (stack buffer) is what unlocked the 17×.

### 3. Thread-level parallelism

A parameter sweep runs many independent backtests over the same read-only data — each (short, long) window pair is its own task. Each thread receives the market data by `const&` (shared, never copied, never written) and writes only to its own `SweepTask` result, so no locks are needed.

**Setup:** 170 SMA-crossover parameter pairs (short window 5–50, long window up to 200), each a full backtest over 500,000 synthetic bars.

| | Sequential | Parallel | Speedup |
|---|---|---|---|
| OS-scheduled threads | 65.3 s | 5.2 s | **12.6×** |
| Threads pinned to cores | 65.3 s | 6.4 s | 10.2× |

All three measured in the same run. Across repeated runs: OS-scheduled 12.6–13.3×, pinned 10.2–10.7×.

**Core pinning made the sweep ~20% slower (18–24% across runs).** Each thread was pinned with `SetThreadAffinityMask` to logical processor `i % hardware_concurrency()`. Three reasons it lost to the OS scheduler here:

1. **Heterogeneous cores.** The Ryzen AI 9 365 has 4 full Zen 5 cores and 6 lower-clocked Zen 5c cores. Static pinning gives slow cores the same number of tasks as fast ones, so the sweep waits on the slowest cores; the scheduler lets fast cores take more work.
2. **No rebalancing.** 170 tasks over 20 logical processors don't divide evenly, and a pinned thread can't move to a core that has gone idle.
3. **SMT siblings.** Pinning can place two busy threads on the two logical processors of one physical core while another physical core has spare capacity.

Pinning pays off when a single latency-critical thread owns a dedicated core. For a throughput-bound batch job with more threads than cores, on hybrid hardware, the OS scheduler wins — and measuring it is what showed that.

An earlier version pinned thread `i` to core `i` directly, which silently failed for every ID ≥ 20 (non-existent processor) and was undefined behaviour for IDs ≥ 64 (`1ULL << 64`). The first "pinning makes no difference" result was therefore not a real test; the numbers above are after the fix.

### Benchmark methodology

- Timed with `std::chrono::high_resolution_clock`, release build (`-O3 -mavx2`)
- Sweep: sequential, parallel and pinned-parallel timed in the same process run; results consistent across repeated runs (~5% variation)
- Indicator timings averaged over 100,000 calls; results fed into a sink so the compiler cannot optimise the work away
- Synthetic data for indicator benchmarks, so memory layout is the only variable
- Multiple data sizes (2.5K → 1M bars) to show results hold beyond cache capacity
- **Hardware:** AMD Ryzen AI 9 365 (Zen 5), Windows

---

## Test suite

Catch2, covering correctness before and after every optimisation:

| Category | What's tested |
|---|---|
| Indicator correctness | SMA (windows 3 and 5), VWAP and rolling volatility against hand-computed values |
| SIMD correctness | SIMD SMA, VWAP and rolling vol each match their scalar counterpart within floating-point tolerance |
| Edge cases | Window larger than data returns 0.0; window equal to data size; window = 1; zero total volume in VWAP (AoS, SoA and SIMD) returns 0.0 rather than NaN |
| Look-ahead regression | `MarketView` throws when a strategy reaches beyond the current bar |
| PnL accounting | A known buy/sell round trip produces the expected profit |

---

## Build

Requires CMake, a C++20 compiler, and a CPU with AVX2. Core pinning uses the Windows API (`windows.h`).

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

AVX2 is enabled in `CMakeLists.txt` via `-mavx2`. Always benchmark in Release — Debug builds inflate timings and hide real differences.

Run the backtester (loads data, runs both strategies, writes equity curves, runs sequential and parallel sweeps):

```bash
./build/BacktestingEngine
```

Run the tests:

```bash
./build/unit_tests
# or, via CTest:
cd build && ctest
```

Run the benchmarks:

```bash
./build/benchmarks
```

## Data

The engine reads OHLCV CSVs from `data/`. A Python script fetches daily bars via `yfinance`:

```bash
pip install yfinance
python src/fetch_prices.py
```

This downloads AAPL, MSFT and SPY daily bars (2015–2025). Equity curves can be plotted with `src/plot_equitycurve.py` (matplotlib).

**Data caveat:** single-ticker daily data from yfinance is subject to survivorship bias — it only includes companies that still exist today. Fine for validating engine mechanics; not a basis for claiming a strategy works.

---

## Roadmap

- **Python strategy interface** — write strategies in Python, run them through the C++ engine (pybind11)
- **Multi-instrument support** — portfolios across many tickers, larger datasets
- **Live data feed** — stream bars from a market data API, turning the backtester into a paper-trading system
- **Latency instrumentation** — per-stage timing of the event loop, ring buffer for incoming bars
- **Cache-miss profiling** — `perf` / `cachegrind` to measure L1 miss reduction directly, not just wall-clock time