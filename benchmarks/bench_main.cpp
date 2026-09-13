#include "../src/Indicators.h"
#include "../src/MarketView.h"
#include "../src/MarketDataSoA.h"
#include "../src/Bar.h"
#include <chrono>
#include <iostream>
#include <vector>
#include <cstdint>

const int ITERS = 100000;
const size_t WINDOW = 50;

std::vector<Bar> generateBarsAoS(size_t n) {
    std::vector<Bar> bars(n);
    for (size_t i = 0; i < n; i++) {
        bars[i] = {(int64_t)i, 100.0, 100.0 + (i % 10), 105.0, 95.0, 1000.0};
    }
    return bars;
}

MarketDataSoA generateBarsSoA(size_t n) {
    MarketDataSoA data;
    data.timestamps.resize(n);
    data.opens.resize(n);
    data.highs.resize(n);
    data.lows.resize(n);
    data.closes.resize(n);
    data.volumes.resize(n);
    for (size_t i = 0; i < n; i++) {
        data.timestamps[i] = (int64_t)i;
        data.opens[i] = 100.0;
        data.highs[i] = 105.0;
        data.lows[i] = 95.0;
        data.closes[i] = 100.0 + (i % 10);
        data.volumes[i] = 1000.0;
    }
    return data;
}

long long timeAoSSMA(const std::vector<Bar>& bars) {
    MarketView view(bars, bars.size());
    double sink = 0.0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; i++) sink += sma(view, WINDOW);
    auto end = std::chrono::high_resolution_clock::now();
    volatile double dummy = sink;
    (void)dummy;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / ITERS;
}

long long timeSoASMA(const MarketDataSoA& data) {
    double sink = 0.0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; i++) sink += smaA(data, WINDOW);
    auto end = std::chrono::high_resolution_clock::now();
    volatile double dummy = sink;
    (void)dummy;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / ITERS;
}

long long timeAoSVWAP(const std::vector<Bar>& bars) {
    MarketView view(bars, bars.size());
    double sink = 0.0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; i++) sink += vwap(view, WINDOW);
    auto end = std::chrono::high_resolution_clock::now();
    volatile double dummy = sink;
    (void)dummy;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / ITERS;
}

long long timeSoAVWAP(const MarketDataSoA& data) {
    double sink = 0.0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; i++) sink += vwapA(data, WINDOW);
    auto end = std::chrono::high_resolution_clock::now();
    volatile double dummy = sink;
    (void)dummy;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / ITERS;
}

long long timeAoSVol(const std::vector<Bar>& bars) {
    MarketView view(bars, bars.size());
    double sink = 0.0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; i++) sink += rollingVol(view, WINDOW);
    auto end = std::chrono::high_resolution_clock::now();
    volatile double dummy = sink;
    (void)dummy;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / ITERS;
}

long long timeSoAVol(const MarketDataSoA& data) {
    double sink = 0.0;
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; i++) sink += rollingVolA(data, WINDOW);
    auto end = std::chrono::high_resolution_clock::now();
    volatile double dummy = sink;
    (void)dummy;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / ITERS;
}

int main() {
    std::vector<size_t> sizes = {2500, 10000, 100000, 500000, 1000000};

    std::cout << "=== AoS vs SoA Benchmark (window=" << WINDOW << ", iters=" << ITERS << ") ===\n\n";

    std::cout << "Data Size   | SMA AoS | SMA SoA | VWAP AoS | VWAP SoA | Vol AoS | Vol SoA\n";
    std::cout << "------------|---------|---------|----------|----------|---------|--------\n";

    for (size_t n : sizes) {
        auto aosData = generateBarsAoS(n);
        auto soaData = generateBarsSoA(n);

        auto smaAos = timeAoSSMA(aosData);
        auto smaSoa = timeSoASMA(soaData);
        auto vwapAos = timeAoSVWAP(aosData);
        auto vwapSoa = timeSoAVWAP(soaData);
        auto volAos = timeAoSVol(aosData);
        auto volSoa = timeSoAVol(soaData);

        printf("%-12zu| %-8lld| %-8lld| %-9lld| %-9lld| %-8lld| %-8lld\n",
               n, smaAos, smaSoa, vwapAos, vwapSoa, volAos, volSoa);
    }

    return 0;
}