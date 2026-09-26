//
// Created by ethan on 23-Sep-26.
//

#include "SweepTask.h"

#include "Backtester.h"
#include  "MarketDataSoA.h"
#include "Indicators.h"
#include "SMA.h"
#include <thread>

#include <iostream>
#include <vector>


void RunSweep(const std::vector<Bar>& data, SweepTask& task) {
    SMA sma(task.shortWindow, task.longWindow);
    Backtester backtester(data, sma);
    std::vector<double> equitycurve = backtester.execute();
    task.finalEquity = equitycurve.back();
}

void runParallelSweep(const std::vector<Bar>& data, std::vector<SweepTask>& tasks) {
    std::vector<std::thread> threads;

    for (size_t i = 0; i < tasks.size(); i++){
        std::thread t(RunSweep,std::ref(data),std::ref(tasks[i]));

        threads.push_back(std::move(t));

    }

    for (std::thread& thr : threads) {
        thr.join();
    }

    for (SweepTask task : tasks) {

    }
};