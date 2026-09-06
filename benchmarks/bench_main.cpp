#include <benchmark/benchmark.h>
#include <vector>
#include <numeric>

// Dummy backtesting calculation function
double compute_sma(const std::vector<double>& prices) {
    double sum = 0.0;
    for (double price : prices) {
        sum += price;
    }
    return sum / static_cast<double>(prices.size());
}

static void BM_SimpleMovingAverage(benchmark::State& state) {
    std::vector<double> prices(10'000, 150.25);

    for (auto _ : state) {
        double result = compute_sma(prices);
        // Prevents compiler from optimizing away the result
        benchmark::DoNotOptimize(result);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_SimpleMovingAverage);

// Generate main() entry point automatically
BENCHMARK_MAIN();