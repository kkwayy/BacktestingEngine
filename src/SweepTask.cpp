//
// Created by ethan on 23-Sep-26.
//

#include "SweepTask.h"
#include "Backtester.h"
#include "SMA.h"

#include <thread>
#include <iostream>
#include <vector>
#include <windows.h>

void RunSweep(const std::vector<Bar>& data, SweepTask& task, size_t coreId, bool pin) {
    // Optionally pin this thread to one logical processor
    if (pin) {
        DWORD_PTR result = SetThreadAffinityMask(GetCurrentThread(), 1ULL << coreId);
        if (result == 0) {
            std::cerr << "Warning: failed to pin thread to core " << coreId << '\n';
        }
    }

    SMA sma(task.shortWindow, task.longWindow);
    Backtester backtester(data, sma);
    std::vector<double> equitycurve = backtester.execute();
    task.finalEquity = equitycurve.back();
}

void runParallelSweep(const std::vector<Bar>& data, std::vector<SweepTask>& tasks, bool pin) {
    std::vector<std::thread> threads;

    size_t numCores = std::thread::hardware_concurrency();
    if (numCores == 0) numCores = 1;   // standard allows 0 if unknown

    for (size_t i = 0; i < tasks.size(); i++) {
        std::thread t(RunSweep, std::ref(data), std::ref(tasks[i]), i % numCores, pin);
        threads.push_back(std::move(t));
    }

    for (std::thread& thr : threads) {
        thr.join();
    }
}