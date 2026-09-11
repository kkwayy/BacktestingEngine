//
// Created by ethan on 11-Sep-26.
//

#ifndef FETCH_PRICES_PY_PORTFOLIO_H
#define FETCH_PRICES_PY_PORTFOLIO_H
#include <vector>
#include "Order.h"


class Portfolio {
public:
    Portfolio(double cash);
    void updatePortfolio(double fill_price, const Order& order);
    double MtM(double fill_price);
    const std::vector<double>& write_equityC() const;




private:
    double cash;
    double position;
    std::vector<double> equityC;


};


#endif //FETCH_PRICES_PY_PORTFOLIO_H