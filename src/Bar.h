//
// Created by ethan on 09-Sep-26.
//

#ifndef BACKTESTINGENGINE_BAR_H
#define BACKTESTINGENGINE_BAR_H
#include <cstdint>


struct  Bar {
    int64_t timestamp;
    double open ;
    double close;
    double high ;
    double low ;
    double volume;

};
#endif //BACKTESTINGENGINE_BAR_H