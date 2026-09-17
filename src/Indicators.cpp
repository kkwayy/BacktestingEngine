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


double vwapSIMD(const MarketDataSoA &view, size_t window) {
    // Numerator -> sum of (closes * volumes)
    // Deomninator -> sum of volumes

    if (view.timestamps.size()< window ) {
        return 0.0;
    }

    size_t l = view.closes.size();


    // --- Setup ---
    // Start index: sum the last 'window' elements from the closes array
    size_t start = l - window;

    // SIMD processes 4 doubles per iteration — find how many full groups of 4 fit
    size_t simdEnd = (window / 4) * 4;

    // AVX2 accumulator: 256-bit register holding 4 running partial sums, initialised to zero
    __m256d num = _mm256_setzero_pd(); // Numerator
    __m256d den = _mm256_setzero_pd(); // Denominator


    // --- SIMD loop: process 4 elements per iteration ---
    for (size_t i = 0; i < simdEnd; i += 4) {
        // Load 4 contiguous doubles from closes[] and volumes into a 256-bit register
        __m256d chunk = _mm256_loadu_pd(&view.closes[start + i]);
        __m256d chunk2 = _mm256_loadu_pd(&view.volumes[start + i]);

        // Multiply the chunks together
        __m256d prod = _mm256_mul_pd(chunk,chunk2);


        // Add the numerator
        num = _mm256_add_pd(num,prod );

        // add to the denominator

        den = _mm256_add_pd(den,chunk2);
    }


    // --- Horizontal sum: reduce 4 partial sums to one total ---
    double temp[4];
    _mm256_storeu_pd(temp, num);  // Store the 4 partial sums back to memory -> Numerator
    double total = temp[0] + temp[1] + temp[2] + temp[3];

    double temp2[4];
    _mm256_storeu_pd(temp2, den);  // Store the 4 partial sums back to memory -> Denominator
    double total2 = temp2[0] + temp2[1] + temp2[2] + temp2[3];

    // --- Tail: handle remaining elements that don't fill a full SIMD register ---
    for (size_t j = simdEnd; j < window; j++) {
        total +=  ( view.closes[j + start] * view.volumes[j + start]) ;
        total2+= view.volumes[j + start];
    }


    double vwap = total / total2;

    return vwap;


}

double rollingVolSIMD(const MarketDataSoA &view, size_t window) {

    if (view.timestamps.size() < window + 1) {
        return 0.0 ;
    }

    size_t widx = view.closes.size();

    // --- Setup ---
    // Start index: sum the last 'window' elements from the closes array
    size_t start = widx - window - 1;

    // SIMD processes 4 doubles per iteration — find how many full groups of 4 fit
    size_t simdEnd = (window / 4) * 4;

    // AVX2 accumulator: 256-bit register holding 4 running partial sums, initialised to zero



    //  Compute Returns

    double returns[50];  // stack buffer, no heap allocation



    // --- SIMD loop: process 4 elements per iteration --- -> Computing Returns
    for (size_t i = 0; i < simdEnd; i += 4) {
        __m256d sub = _mm256_setzero_pd(); // Subtraction
        __m256d res = _mm256_setzero_pd(); // Result Chunk

        // Load 4 contiguous doubles from closes[] and volumes into a 256-bit register
        __m256d chunk = _mm256_loadu_pd(&view.closes[start + i]);
        __m256d chunk2 = _mm256_loadu_pd(&view.closes[start + i + 1]);

        // Subtract the chunks together
        sub = _mm256_sub_pd(chunk2,chunk);

        // produce the results
        res = _mm256_div_pd(sub,chunk );

        // add to the returns vector
        _mm256_storeu_pd(&returns[i],res);
    }

    for (size_t j = simdEnd; j < window; j++) {
        returns[j] = (view.closes[start + j + 1] - view.closes[start + j]) / view.closes[start + j];
    }

    /// Mean of Returns
    ///
    // --- Setup ---
    // Start index: sum the last 'window' elements from the closes array


    // SIMD processes 4 doubles per iteration — find how many full groups of 4 fit
    size_t simdEnd2 = (window / 4) * 4;

    // AVX2 accumulator: 256-bit register holding 4 running partial sums, initialised to zero
    __m256d acc = _mm256_setzero_pd();

    // --- SIMD loop: process 4 elements per iteration ---
    for (size_t i = 0; i < simdEnd2; i += 4) {
        // Load 4 contiguous doubles from closes[] into a 256-bit register
        __m256d chunk = _mm256_loadu_pd(&returns[i]);
        // Add all 4 to the accumulator in a single instruction
        acc = _mm256_add_pd(acc, chunk);
    }

    // --- Horizontal sum: reduce 4 partial sums to one total ---
    double temp[4];
    _mm256_storeu_pd(temp, acc);  // Store the 4 partial sums back to memory
    double total = temp[0] + temp[1] + temp[2] + temp[3];

    // --- Tail: handle remaining elements that don't fill a full SIMD register ---
    for (size_t j = simdEnd2; j < window; j++) {
        total += returns[j];
    }

    double mean = total / window;

    // Variance
    __m256d meanVec = _mm256_set1_pd(mean);
    __m256d var = _mm256_setzero_pd(); // variance register


    // --- SIMD loop: process 4 elements per iteration ---
    for (size_t i = 0; i < simdEnd2; i += 4) {

        // Load 4 contiguous doubles from closes[] into a 256-bit register
        __m256d chunk = _mm256_loadu_pd(&returns[i]);

        __m256d sub = _mm256_sub_pd(chunk,meanVec);

        __m256d res = _mm256_mul_pd(sub,sub);

        var = _mm256_add_pd(var, res);

    }

    // --- Horizontal sum: reduce 4 partial sums to one total ---
    double temp3[4];
    _mm256_storeu_pd(temp3, var);  // Store the 4 partial sums back to memory
    double total2 = temp3[0] + temp3[1] + temp3[2] + temp3[3];

    // --- Tail: handle remaining elements that don't fill a full SIMD register ---
    for (size_t j = simdEnd2; j < window; j++) {
        total2 += ((returns[j] - mean) * (returns[j] - mean) );
    }

    double variance = total2/(window -1);

    return std::sqrt(variance);

}

