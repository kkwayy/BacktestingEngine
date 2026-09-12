//
// Created by ethan on 06-Sep-26.
//

#include <catch2/catch_all.hpp>
#include "src/Bar.h"
#include "src/MarketView.h"
#include "src/Indicators.h"
#include "src/Portfolio.h"
#include "src/Order.h"
TEST_CASE("Dummy passing test", "[dummy]") {
    REQUIRE(1 + 1 == 2);
}

TEST_CASE("SMA test") {
    std::vector<Bar> bars;
    Bar bar1{1, 0.0, 10.0, 0.0, 0.0, 100.0};
    Bar bar2{2,0.0, 20.0, 0.0 ,0.0,200.0};
    Bar bar3{3,0.0, 30.0, 0.0, 0.0, 300.0};
    Bar bar4{4,0.0,40.0,0.0,0.0, 150.0};
    Bar bar5{5,0.0,50.0,0.0,0.0 ,250.0};
    bars.push_back(bar1);
    bars.push_back(bar2);
    bars.push_back(bar3);
    bars.push_back(bar4);
    bars.push_back(bar5);
    MarketView view(bars,bars.size());

    double res =sma(view,3);

    REQUIRE(res ==Catch::Approx(40.0));

    double res2 = sma(view,5);
    REQUIRE(res2 == Catch::Approx(30.0));
}
TEST_CASE("VWAP") {
    std::vector<Bar> bars;
    Bar bar1{1, 0.0, 10.0, 0.0, 0.0, 100.0};
    Bar bar2{2,0.0, 20.0, 0.0 ,0.0,200.0};
    Bar bar3{3,0.0, 30.0, 0.0, 0.0, 300.0};
    Bar bar4{4,0.0,40.0,0.0,0.0, 150.0};
    Bar bar5{5,0.0,50.0,0.0,0.0 ,250.0};
    bars.push_back(bar1);
    bars.push_back(bar2);
    bars.push_back(bar3);
    bars.push_back(bar4);
    bars.push_back(bar5);
    MarketView view(bars,bars.size());

    double res = vwap(view,3);

    REQUIRE(res==Catch::Approx(39.2857142));
}

TEST_CASE("Rolling Vol") {
    std::vector<Bar> bars;
    Bar bar1{1, 0.0, 10.0, 0.0, 0.0, 100.0};
    Bar bar2{2,0.0, 20.0, 0.0 ,0.0,200.0};
    Bar bar3{3,0.0, 30.0, 0.0, 0.0, 300.0};
    Bar bar4{4,0.0,40.0,0.0,0.0, 150.0};
    Bar bar5{5,0.0,50.0,0.0,0.0 ,250.0};
    bars.push_back(bar1);
    bars.push_back(bar2);
    bars.push_back(bar3);
    bars.push_back(bar4);
    bars.push_back(bar5);
    MarketView view(bars,bars.size());

    double res1 = rollingVol(view,3);

    REQUIRE(res1 == Catch::Approx(0.12730).epsilon(1e-4));

}

TEST_CASE("Look-Ahead Regression Test") {
    std::vector<Bar> bars;
    Bar bar1{1, 0.0, 10.0, 0.0, 0.0, 100.0};
    Bar bar2{2,0.0, 20.0, 0.0 ,0.0,200.0};
    Bar bar3{3,0.0, 30.0, 0.0, 0.0, 300.0};
    Bar bar4{4,0.0,40.0,0.0,0.0, 150.0};
    Bar bar5{5,0.0,50.0,0.0,0.0 ,250.0};
    bars.push_back(bar1);
    bars.push_back(bar2);
    bars.push_back(bar3);
    bars.push_back(bar4);
    bars.push_back(bar5);
    MarketView view(bars,3);

    REQUIRE(view.size() == 3);
    REQUIRE_THROWS(view(3));

}

TEST_CASE("PnL Accounting Test") {
    Portfolio p1(1000);
    Order order1{1,Side::Buy};
    p1.updatePortfolio(100.0,order1);
    Order order2{1,Side::Sell};
    p1.updatePortfolio(150.0,order2);
    double finalValue = p1.MtM(0.0);
    REQUIRE(finalValue == Catch::Approx(1050.0));






}