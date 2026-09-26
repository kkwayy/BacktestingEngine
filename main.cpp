#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "src/Bar.h"
#include "src/DataLoader.h"
#include "src/MarketView.h"
#include "src/Backtester.h"
#include "src/BuyandHold.h"
#include "src/Order.h"
#include "src/SMA.h"
#include "src/SweepTask.h"

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


    SMA s2(20,50);

    Backtester backtester2(OHLCV,s2);

    std::vector<double> equityCurve2 = backtester2.execute();

    std::cout << equityCurve2.size() <<'\n';

    std::ofstream out2("equity_curve2.csv");
    out2 <<"Price\n";
    out2.setf(std::ios::fixed);
    out2.precision(10);

    for (const auto& p: equityCurve2)
        out2<<p<<'\n';
    out2.close();

    std::cout<<"Wrote equity_curve.csv (" <<equityCurve2.size()<<" points). Plot with python plot_equitycurve2.py\n";


    std::vector<SweepTask> tasks;
    for (size_t s = 5; s <= 50; s += 5) {
        for (size_t l = s + 10; l <= 200; l += 10) {
            tasks.push_back({s, l, 0.0});
        }
    }

    std::vector<Bar> bigData(500000);
    for (size_t i = 0; i < bigData.size(); i++) {
        bigData[i] = {(int64_t)i, 100.0, 105.0, 95.0, 100.0 + (i % 10), 1000.0};
    }
    std::cout << "Tasks: " << tasks.size() << '\n';

    // Sequential
    auto start1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < tasks.size(); i++) {
        RunSweep(bigData, tasks[i], 0);
    }
    auto end1 = std::chrono::high_resolution_clock::now();
    auto seqTime = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count();
    std::cout << "Sequential: " << seqTime << " ms\n";


    // Reset tasks
    for (auto& t : tasks) t.finalEquity = 0.0;

    // Parallel
    auto start2 = std::chrono::high_resolution_clock::now();
    runParallelSweep(bigData, tasks);
    auto end2 = std::chrono::high_resolution_clock::now();
    auto parTime = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();
    std::cout << "Parallel: " << parTime << " ms\n";
    return 0;







}