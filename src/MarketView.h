//
// Created by ethan on 11-Sep-26.
//

#ifndef FETCH_PRICES_PY_MARKETVIEW_H
#define FETCH_PRICES_PY_MARKETVIEW_H
#include <vector>
#include "Bar.h"
#include <span>
#include <cassert>


class  MarketView {
public:
    MarketView(const std::vector<Bar>& data, const size_t& limit );

    const Bar& operator()(size_t index)  const ;
    size_t size() const;


private:
    size_t limit ;
    std::span< const Bar> view ;

};

#endif //FETCH_PRICES_PY_MARKETVIEW_H