//
// Created by ethan on 11-Sep-26.
//

#include "Portfolio.h"


Portfolio::Portfolio(double cash)
    : cash(cash), position(0){

}

void Portfolio::updatePortfolio(double fill_price, const Order& order){
    Side side = order.side;
    double quantity = order.quantity;

    if (side == Side::Buy){
        cash -= (fill_price * quantity);
        position += quantity;
    }

    else {
        cash += (fill_price * quantity);
        position -= quantity;

    }

}

double Portfolio::MtM(double fill_price) {

    double c_Mtm = cash + (position * fill_price);
    equityC.push_back(c_Mtm);
    return c_Mtm;

}

const std::vector<double> &Portfolio::write_equityC() const {
    return equityC;
}

