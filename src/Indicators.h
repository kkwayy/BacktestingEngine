//
// Created by ethan on 12-Sep-26.
//


#ifndef FETCH_PRICES_PY_INDICATORS_H
#define FETCH_PRICES_PY_INDICATORS_H

#pragma once
#include "MarketView.h"

double sma(const MarketView& view, size_t window);
double vwap(const MarketView& view, size_t window);
double rollingVol(const MarketView& view, size_t window);


#endif //FETCH_PRICES_PY_INDICATORS_H