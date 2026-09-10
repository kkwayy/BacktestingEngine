//
// Created by ethan on 09-Sep-26.
//

#include "DataLoader.h"
#include "Bar.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <ctime>

std::vector<Bar> loadCSV(const std::string& filename) {
    std::vector<Bar> data;

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open " << filename << '\n';
        return data;   // return empty on failure
    }

    std::string header;
    std::getline(file, header);

    std::string header2;
    std::getline(file, header2);

    std::string header3;
    std::getline(file, header3);

    std::string line;

    while (std::getline(file, line)) {

        if (line.empty()) {
            continue;                        // skip blank lines
        }

        std::istringstream ss(line);         // wrap the line in a STREAM
        std::string field;
        Bar current ;



        current.timestamp = 0.0;



        getline(ss, field,',');

        int year, month, day;
        std::sscanf(field.c_str(), "%d-%d-%d", &year, &month, &day);

        std::tm tm{};

        tm.tm_year = year -1900;
        tm.tm_mon = month -1;
        tm.tm_mday = day;

        std::time_t t = std::mktime(&tm);

        current.timestamp = t;


        getline(ss, field,',');
        current.open=stod(field);

        getline(ss, field,',');
        current.high=stod(field);

        getline(ss, field,',');
        current.low = stod(field);

        getline(ss, field,',');
        current.close = stod(field);

        getline(ss, field,',');
        current.volume =stod(field);


        if (data.size() > 0) {

            Bar back = data.back() ;

            assert(back.timestamp < current.timestamp);

        }

        data.push_back(current);


    }

    return data;
}
