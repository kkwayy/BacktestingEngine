//
// Created by ethan on 13-Sep-26.
//

#ifndef FETCH_PRICES_PY_MARKETDATASOA_H
#define FETCH_PRICES_PY_MARKETDATASOA_H

struct MarketDataSoA {
    std::vector<int64_t> timestamps;
    std::vector<double> opens;
    std::vector<double> highs;
    std::vector<double> lows;
    std::vector<double> closes;
    std::vector<double> volumes;
};


#endif //FETCH_PRICES_PY_MARKETDATASOA_H