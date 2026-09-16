//
// Created by ethan on 12-Sep-26.
//

#include "Indicators.h"
#include "Order.h"
#include <cmath>
#include <immintrin.h>


double sma(const MarketView &view, size_t window) {
    if (view.size() < window ) {
        return 0.0;
    }
    size_t widx = view.size();

    double wTotal = 0.0 ;

    for (size_t i = 0; i < window; i++) {
        wTotal += view((widx- i -1 )).close;
    }

    double wMean = wTotal/window;


    return wMean;
}

double vwap(const MarketView &view, size_t window) {
    if (view.size() < window ) {
        return 0.0;
    }

    size_t widx = view.size();

    double product= 0.0 ;
    double totalVol = 0.0 ;


    for (size_t i = 0; i < window; i++) {
        product += (view(widx - i -1 ).close * view(widx - i -1).volume);
        totalVol += view(widx -i  -1 ).volume;
    }

    double vwap = product/totalVol ;

    return vwap;

}

double rollingVol(const MarketView &view, size_t window) {

    if (view.size() < window + 1) {
        return 0.0 ;
    }

    std::vector<double> returns;

    size_t widx = view.size();


    for (size_t i = 0; i < window ; i++) {

        double r = (view(widx  -1 - i).close  - view( widx - 2 - i).close) / ( view(widx -2 -i).close);

        returns.push_back(r);
    }

    size_t l = returns.size();

    double total = 0.0 ;
    double diff = 0.0 ;
    double var = 0.0;
    double vol = 0.0 ;

    for (size_t j = 0 ; j < l ; j++) {
         total += returns[j];
    }

    double mean = total/l ;

    for (size_t z = 0; z < l ; z++ ) {
        double curr = (returns[z]- mean );
        diff += std::pow(curr, 2);
    }

    var = diff / (l -1) ;

    vol = std::sqrt(var);

    return vol;

}


double smaA(const MarketDataSoA &view, size_t window) {
    if (view.closes.size() < window) {
        return 0.0;
    }

    double total = 0.0;
    size_t l = view.closes.size()-1 ;

    for (size_t i = 0; i < window ; i ++) {
        total+= view.closes[l- i];
    }

    double mean = total /window;

    return  mean;

}


double vwapA(const MarketDataSoA &view, size_t window) {
    if (view.timestamps.size()< window ) {
        return 0.0;
    }

    size_t l = view.closes.size()-1 ;

    double product= 0.0 ;
    double totalVol = 0.0 ;

    for (size_t i = 0; i < window; i++) {
        product += view.closes[l - i] * view.volumes[l - i];
        totalVol += view.volumes[l - i];
    }

    double vwap = product/totalVol ;

    return vwap;

}

double rollingVolA(const MarketDataSoA &view, size_t window) {

    if (view.timestamps.size() < window + 1) {
        return 0.0 ;
    }


    std::vector<double> returns;

    size_t widx = view.closes.size();


    for (size_t i = 0; i < window ; i++) {

        double r = (view.closes[widx  -1 - i]  - view.closes[ widx - 2 - i]) / ( view.closes[widx -2 -i]);

        returns.push_back(r);
    }

    size_t l = returns.size();


    double total = 0.0 ;
    double diff = 0.0 ;
    double var = 0.0;
    double vol = 0.0 ;

    for (size_t j = 0 ; j < l ; j++) {
        total += returns[j];
    }

    double mean = total/l ;

    for (size_t z = 0; z < l ; z++ ) {
        double curr = (returns[z]- mean );
        diff += std::pow(curr, 2);
    }

    var = diff / (l -1) ;

    vol = std::sqrt(var);

    return vol;


}

double smaSIMD(const MarketDataSoA &view, size_t window) {
    if (view.closes.size() < window) {
        return 0.0;
    }

    size_t size = view.closes.size();

    // --- Setup ---
    // Start index: sum the last 'window' elements from the closes array
    size_t start = size - window;

    // SIMD processes 4 doubles per iteration — find how many full groups of 4 fit
    size_t simdEnd = (window / 4) * 4;

    // AVX2 accumulator: 256-bit register holding 4 running partial sums, initialised to zero
    __m256d acc = _mm256_setzero_pd();

    // --- SIMD loop: process 4 elements per iteration ---
    for (size_t i = 0; i < simdEnd; i += 4) {
        // Load 4 contiguous doubles from closes[] into a 256-bit register
        __m256d chunk = _mm256_loadu_pd(&view.closes[start + i]);
        // Add all 4 to the accumulator in a single instruction
        acc = _mm256_add_pd(acc, chunk);
    }

    // --- Horizontal sum: reduce 4 partial sums to one total ---
    double temp[4];
    _mm256_storeu_pd(temp, acc);  // Store the 4 partial sums back to memory
    double total = temp[0] + temp[1] + temp[2] + temp[3];

    // --- Tail: handle remaining elements that don't fill a full SIMD register ---
    for (size_t j = simdEnd; j < window; j++) {
        total += view.closes[j + start];
    }

    double mean = total / window;
    return mean;
}