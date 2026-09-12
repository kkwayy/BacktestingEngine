# Backtesting Engine

A single-threaded, event-driven backtesting engine built from scratch in C++. The engine replays historical OHLCV market data through a pluggable strategy interface, producing fills and a marked-to-market equity curve.

Built as a deliberate learning progression from a [portfolio optimisation engine](https://github.com/kkway/portfolio-engine) — the optimiser answers "what should the portfolio look like?", the backtester answers "how would that strategy have actually performed?"

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

**Key design decisions:**

- **Event-driven, not vectorised.** The engine steps through time one bar at a time, structurally preventing look-ahead bias rather than relying on discipline.
- **MarketView as the look-ahead boundary.** The strategy receives a view that physically cannot access data beyond the current timestamp. This is enforced via bounds checking — accessing future data throws `std::out_of_range`.
- **Fill at next bar's open.** Orders emitted on bar *t* fill at bar *t+1*'s open price, with configurable slippage and commission. No ambiguity about whether the strategy "knew" the fill price.
- **Strategy interface via abstract base class.** Any strategy inherits from `Strategy` and implements `onbar(const MarketView&)`. The engine is decoupled from any specific trading logic.

## Components

| File | Purpose |
|------|---------|
| `Bar.h` | OHLCV bar struct with `int64_t` epoch timestamp |
| `DataLoader` | CSV parser → `std::vector<Bar>` with chronological order validation |
| `MarketView` | Const view over bar data with index-bounded access |
| `Strategy` | Abstract base class — pure virtual `onbar` interface |
| `Backtester` | Event loop: fills pending orders, runs strategy, records equity |
| `Portfolio` | Cash, position tracking, mark-to-market, equity curve |
| `Order` | Side (Buy/Sell) and quantity |
| `Indicators` | Standalone SMA, VWAP, rolling volatility functions |

## Strategies included

- **BuyAndHold** — buys one share on the first bar, holds indefinitely. Used for baseline validation.
- **SMA Crossover** — configurable short/long window moving-average crossover. Buys on bullish cross, sells on bearish cross, with position tracking to prevent stacking.

## Test suite

Catch2 tests covering:

- **Indicator correctness** — SMA, VWAP, and rolling volatility validated against hand-computed values on known inputs
- **Look-ahead regression** — asserts that `MarketView` cannot access bars beyond the current time boundary
- **PnL accounting** — verifies portfolio cash after a known buy/sell sequence matches expected profit

## Build

Requires CMake and a C++20 compiler.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Run the backtester:

```bash
./build/BacktestingEngine
```

Run tests:

```bash
cd build && ctest
```

## Data

The engine reads OHLCV CSVs from the `data/` directory. A Python script (`src/fetch_prices.py`) fetches daily bars via `yfinance`:

```bash
pip install yfinance
python src/fetch_prices.py
```

This downloads AAPL, MSFT, and SPY daily bars (2015–2025) into `data/`.

## Roadmap

The engine is designed from the start with performance architecture in mind. Planned stages, each gated by correctness:

- **Data-oriented design** — refactor bar storage from Array-of-Structs to Structure-of-Arrays for cache-friendly access patterns
- **SIMD vectorisation** — AVX2 intrinsics for indicator computation, correctness-tested against scalar baselines
- **Thread-level parallelism** — parallel parameter sweeps with independent worker threads and core pinning

Each optimisation is added only after profiling, benchmarked against the previous stage, and validated against the correct scalar implementation.
