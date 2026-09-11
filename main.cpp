#include <iostream>

#include <vector>

#include "src/Bar.h"
#include "src/DataLoader.h"
#include "src/MarketView.h"
#include "src/Backtester.h"
#include "src/BuyandHold.h"
#include "src/Order.h"

int main() {
    std::vector<Bar> OHLCV = loadCSV("data/AAPL.csv");
    std::cout << "Open: " << OHLCV[0].open << " Close: " << OHLCV[0].close << '\n';
    std::cout << " Size: " << OHLCV.back().open << " "<< OHLCV.back().close << '\n';
    std::cout << "Dimesnions" << OHLCV.size() << '\n';

    std::cout << "Time:" << OHLCV[0].timestamp << '\n';

    BuyandHold s1;

    Backtester backtester(OHLCV,s1) ;

    std::vector<Order> orders = backtester.execute();

    std::cout << orders.size() <<'\n';











}