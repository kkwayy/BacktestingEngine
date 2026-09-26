//
// Created by ethan on 23-Sep-26.
//

#ifndef FETCH_PRICES_PY_SWEEPTASK_H
#define FETCH_PRICES_PY_SWEEPTASK_H
#include "Bar.h"
#include <iostream>
#include <vector>


struct SweepTask {
    size_t shortWindow;
    size_t longWindow;
    double finalEquity;  // filled by the worker
};
void RunSweep(const std::vector<Bar>& data, SweepTask& task, size_t coreId, bool pin);
void runParallelSweep(const std::vector<Bar>& data, std::vector<SweepTask>& tasks,bool pin);
#endif //FETCH_PRICES_PY_SWEEPTASK_H