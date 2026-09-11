//
// Created by ethan on 11-Sep-26.
//

#ifndef FETCH_PRICES_PY_STRATEGY_H
#define FETCH_PRICES_PY_STRATEGY_H
#include <vector>
#include "MarketView.h"
#include "Order.h"


class Strategy {
public:
    virtual std::vector<Order> onbar(const MarketView& view ) = 0;

    virtual ~Strategy() = default;
};
#endif //FETCH_PRICES_PY_STRATEGY_H