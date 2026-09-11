//
// Created by ethan on 11-Sep-26.
//
#include "Bar.h"
#include "MarketView.h"
#include "Order.h"
#include "Strategy.h"
#include "Backtester.h"
Backtester::Backtester(const std::vector<Bar> &data, Strategy& strategy)
    : view(data), strategy(strategy){
}

std::vector<Order> Backtester::execute() {
    size_t end = view.size();
    std::vector<Order> orders;

    for (size_t i = 0; i < end; i++) {
        MarketView currentView(view,i+1);
        std::vector<Order> order = strategy.onbar(currentView);

        orders.insert(orders.end(),order.begin(),order.end());

    }

    return orders;
}

