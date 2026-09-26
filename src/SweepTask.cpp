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
#include <windows.h>



void RunSweep(const std::vector<Bar>& data, SweepTask& task, size_t coreId) {
    // Pin this thread to a specific core
    SetThreadAffinityMask(GetCurrentThread(), 1ULL << coreId);

    // Now do the work — all of it runs on the pinned core
    SMA sma(task.shortWindow, task.longWindow);
    Backtester backtester(data, sma);
    std::vector<double> equitycurve = backtester.execute();
    task.finalEquity = equitycurve.back();
}

void runParallelSweep(const std::vector<Bar>& data, std::vector<SweepTask>& tasks) {
    std::vector<std::thread> threads;
    size_t numCores = std::thread::hardware_concurrency();
    for (size_t i = 0; i < tasks.size(); i++){

        std::thread t(RunSweep, std::ref(data), std::ref(tasks[i]), i % numCores);

        threads.push_back(std::move(t));

    }

    for (std::thread& thr : threads) {
        thr.join();
    }

    for (SweepTask task : tasks) {

    }
};