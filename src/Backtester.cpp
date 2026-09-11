//
// Created by ethan on 11-Sep-26.
//
#include "Bar.h"
#include "MarketView.h"
#include "Order.h"
#include "Strategy.h"
#include "Backtester.h"
#include "Portfolio.h"

Backtester::Backtester(const std::vector<Bar> &data, Strategy& strategy)
    : view(data), strategy(strategy){
}

std::vector<double> Backtester::execute() {
    size_t end = view.size();

    std::vector<Order> pending_order;


    Portfolio portfolio(100000);


    for (size_t i = 0; i < end; i++) {

        MarketView currentView(view,i+1);
        double execute_price = currentView(i).open;

        // Execute Pending Orders
        if (pending_order.size() > 0){
            for (size_t j = 0; j < pending_order.size();j++) {
                Order pending = pending_order[j];
                portfolio.updatePortfolio(execute_price,pending);
            }
            pending_order.clear();
        }

         double cMtM =portfolio.MtM(currentView(i).close);


        std::vector<Order> order = strategy.onbar(currentView);
        pending_order.insert(pending_order.end(),order.begin(),order.end());

    }

     const std::vector<double>& strategy_equityC = portfolio.write_equityC();

    return strategy_equityC;
}

