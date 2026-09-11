#include <iostream>
#include <fstream>
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

    std::vector<double> equityCurve = backtester.execute();

    std::cout << equityCurve.size() <<'\n';
    std::cout << "Equity[0]: " << equityCurve[0] << '\n';
    std::cout << "Equity[1]: " << equityCurve[1] << '\n';
    std::cout << "Equity[2]: " << equityCurve[2] << '\n';
    std::cout << "Equity[last]: " << equityCurve.back() << '\n';

    //Equity Curve display

    std::ofstream out("equity_curve.csv");
    out <<"Price\n";
    out.setf(std::ios::fixed);
    out.precision(10);

    for (const auto& p: equityCurve)
        out<<p<<'\n';
    out.close();

    std::cout<<"Wrote equity_curve.csv (" <<equityCurve.size()<<" points). Plot with python plot_equitycurve.py\n";

    return 0;






}