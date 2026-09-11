//
// Created by ethan on 12-Sep-26.
//

#include "SMA.h"

SMA::SMA(size_t shortW, size_t longW)
    : shortW(shortW), longW(longW), in_position(false){
}


std::vector<Order> SMA::onbar(const MarketView &view) {
    /*
     * Short SMA crosses above long SMA → buy, Short SMA crosses below long SMA → sell
     * What "crosses" means: you need to compare two bars. If yesterday short was below long, and today short is above long, that's a bullish crossover.
     *  So you need the current bar's SMAs and the previous bar's SMAs.
     */


    if (view.size() < longW + 1) {
        return std::vector<Order>{};
    }

    size_t c_idx = view.size();

    double cb_shortTotal = 0.0;

    for (size_t i = 0 ; i < shortW ; i++) {
        cb_shortTotal+= view((c_idx -1 - i)).close;
    }

    double cb_shortMean = cb_shortTotal/shortW;


    double cb_longTotal = 0.0;

    for (size_t j = 0; j < longW ; j++) {
        cb_longTotal+= view((c_idx - 1- j)).close;
    }

    double cb_longMean = cb_longTotal / longW;


    double pb_shortTotal = 0.0;

    for (size_t i = 0 ; i < shortW ; i++) {
        pb_shortTotal+= view((c_idx -2 - i)).close;
    }

    double pb_shortMean = pb_shortTotal/shortW;


    double pb_longTotal = 0.0;

    for (size_t j = 0; j < longW ; j++) {
        pb_longTotal+= view((c_idx - 2- j)).close;
    }

    double pb_longMean = pb_longTotal / longW;



    if (pb_shortMean <= pb_longMean && cb_shortMean >cb_longMean && !in_position) {
        Order order;
        order.side=Side::Buy;
        order.quantity=1.0;
        std::vector<Order> res ;
        res.push_back(order);
        in_position = true;

        return res;
    }

    else if (pb_shortMean> pb_longMean && cb_shortMean<= cb_longMean && in_position){
        Order order;
        order.side=Side::Sell;
        order.quantity=1.0;
        std::vector<Order> res ;
        res.push_back(order);
        in_position = false;
        return res;
    }

    else {
        return std::vector<Order>{};
    }




}
