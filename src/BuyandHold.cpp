//
// Created by ethan on 11-Sep-26.
//

#include "Strategy.h"
#include "BuyandHold.h"


std::vector<Order> BuyandHold::onbar(const MarketView &view) {

    if (view.size() == 1) {
        Order order;
        order.side=Side::Buy;
        order.quantity=1.0;
        std::vector<Order> res ;
        res.push_back(order);

        return res;
    }
    else{
        return std::vector<Order>{};
    }
}

