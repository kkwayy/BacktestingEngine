//
// Created by ethan on 11-Sep-26.
//

#include "MarketView.h"
#include "Bar.h"


MarketView::MarketView(const std::vector<Bar> &data, const size_t &limit)
    : limit(limit), view(data.data(), limit) {

}

size_t MarketView::size() const {
    return view.size();
}

const Bar& MarketView::operator()(size_t index) const{
    assert(index < limit);

    return view[index];


}

