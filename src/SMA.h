//
// Created by ethan on 12-Sep-26.
//

#ifndef FETCH_PRICES_PY_SMA_H
#define FETCH_PRICES_PY_SMA_H
#include "Strategy.h"


class SMA : public Strategy{
public:
    std::vector<Order> onbar(const MarketView& view) override;
    SMA(size_t shortW, size_t longW);
private:
    size_t shortW;
    size_t longW;
    bool in_position;
};


#endif //FETCH_PRICES_PY_SMA_H