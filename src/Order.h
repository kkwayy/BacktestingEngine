//
// Created by ethan on 11-Sep-26.
//

#ifndef FETCH_PRICES_PY_ORDER_H
#define FETCH_PRICES_PY_ORDER_H

enum class Side {
    Buy,
    Sell
};

struct Order {
    double quantity;
    Side side;

};

#endif //FETCH_PRICES_PY_ORDER_H