#include <iostream>

#include <vector>

#include "src/Bar.h"
#include "src/DataLoader.h"

int main() {
    std::vector<Bar> OHLCV = loadCSV("data/AAPL.csv");
    std::cout << "Open: " << OHLCV[0].open << " Close: " << OHLCV[0].close << '\n';
    std::cout << " Size: " << OHLCV.back().open << " "<< OHLCV.back().close << '\n';
    std::cout << "Dimesnions" << OHLCV.size() << '\n';





}