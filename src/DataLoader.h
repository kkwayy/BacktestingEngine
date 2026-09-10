//
// Created by ethan on 09-Sep-26.
//

#ifndef BACKTESTINGENGINE_DATALOADER_H
#define BACKTESTINGENGINE_DATALOADER_H


#include <vector>
#include <string>
#include "Bar.h"

std::vector<Bar> loadCSV(const std::string& filename);



#endif //BACKTESTINGENGINE_DATALOADER_H