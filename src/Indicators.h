//
// Created by ethan on 12-Sep-26.
//




#pragma once
#include "MarketView.h"
#include "MarketDataSoA.h"



// AoS
double sma(const MarketView& view, size_t window);
double vwap(const MarketView& view, size_t window);
double rollingVol(const MarketView& view, size_t window);
// Soa
double smaA(const MarketDataSoA& view, size_t window);
double vwapA(const MarketDataSoA& view, size_t window);
double rollingVolA(const MarketDataSoA& view, size_t window);

double smaSIMD(const MarketDataSoA& view, size_t window);
double vwapSIMD(const MarketDataSoA& view, size_t window);
double rollingVolSIMD(const MarketDataSoA& view, size_t window);







