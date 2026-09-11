//
// Created by ethan on 11-Sep-26.
//

#ifndef FETCH_PRICES_PY_BACKTESTER_H
#define FETCH_PRICES_PY_BACKTESTER_H
#include "Bar.h"
#include "MarketView.h"
#include "Order.h"
#include "Strategy.h"


class Backtester {
public:
    Backtester(const std::vector<Bar>& data , Strategy& strategy);
    std::vector<double> execute();
private:
    const std::vector<Bar>& view ;
    Strategy& strategy ;

};





#endif //FETCH_PRICES_PY_BACKTESTER_H