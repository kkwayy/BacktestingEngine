//
// Created by ethan on 11-Sep-26.
//

#ifndef FETCH_PRICES_PY_BUYANDHOLD_H
#define FETCH_PRICES_PY_BUYANDHOLD_H

#include "Strategy.h"

class BuyandHold: public Strategy{
public:
    std::vector<Order> onbar(const MarketView& view) override;

};

#endif //FETCH_PRICES_PY_BUYANDHOLD_H