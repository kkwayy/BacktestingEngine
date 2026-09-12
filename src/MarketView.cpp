//
// Created by ethan on 11-Sep-26.
//

#include "MarketView.h"
#include "Bar.h"
#include <stdexcept>


MarketView::MarketView(const std::vector<Bar> &data, const size_t &limit)
    : limit(limit), view(data.data(), limit) {

}

size_t MarketView::size() const {
    return view.size();
}

const Bar& MarketView::operator()(size_t index) const{
    if (index >= limit) throw std::out_of_range("MarketView: access beyond current time");;

    return view[index];


}

