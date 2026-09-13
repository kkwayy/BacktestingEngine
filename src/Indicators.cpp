//
// Created by ethan on 12-Sep-26.
//

#include "Indicators.h"
#include "Order.h"
#include <cmath>


double sma(const MarketView &view, size_t window) {
    if (view.size() < window ) {
        return 0.0;
    }
    size_t widx = view.size();

    double wTotal = 0.0 ;

    for (size_t i = 0; i < window; i++) {
        wTotal += view((widx- i -1 )).close;
    }

    double wMean = wTotal/window;


    return wMean;
}

double vwap(const MarketView &view, size_t window) {
    if (view.size() < window ) {
        return 0.0;
    }

    size_t widx = view.size();

    double product= 0.0 ;
    double totalVol = 0.0 ;


    for (size_t i = 0; i < window; i++) {
        product += (view(widx - i -1 ).close * view(widx - i -1).volume);
        totalVol += view(widx -i  -1 ).volume;
    }

    double vwap = product/totalVol ;

    return vwap;

}

double rollingVol(const MarketView &view, size_t window) {

    if (view.size() < window + 1) {
        return 0.0 ;
    }

    std::vector<double> returns;

    size_t widx = view.size();


    for (size_t i = 0; i < window ; i++) {

        double r = (view(widx  -1 - i).close  - view( widx - 2 - i).close) / ( view(widx -2 -i).close);

        returns.push_back(r);
    }

    size_t l = returns.size();

    double total = 0.0 ;
    double diff = 0.0 ;
    double var = 0.0;
    double vol = 0.0 ;

    for (size_t j = 0 ; j < l ; j++) {
         total += returns[j];
    }

    double mean = total/l ;

    for (size_t z = 0; z < l ; z++ ) {
        double curr = (returns[z]- mean );
        diff += std::pow(curr, 2);
    }

    var = diff / (l -1) ;

    vol = std::sqrt(var);

    return vol;

}


double smaA(const MarketDataSoA &view, size_t window) {
    if (view.closes.size() < window) {
        return 0.0;
    }

    double total = 0.0;
    size_t l = view.closes.size()-1 ;

    for (size_t i = 0; i < window ; i ++) {
        total+= view.closes[l- i];
    }

    double mean = total /window;

    return  mean;

}


double vwapA(const MarketDataSoA &view, size_t window) {
    if (view.timestamps.size()< window ) {
        return 0.0;
    }

    size_t l = view.closes.size()-1 ;

    double product= 0.0 ;
    double totalVol = 0.0 ;

    for (size_t i = 0; i < window; i++) {
        product += view.closes[l - i] * view.volumes[l - i];
        totalVol += view.volumes[l - i];
    }

    double vwap = product/totalVol ;

    return vwap;

}

double rollingVolA(const MarketDataSoA &view, size_t window) {

    if (view.timestamps.size() < window + 1) {
        return 0.0 ;
    }


    std::vector<double> returns;

    size_t widx = view.closes.size();


    for (size_t i = 0; i < window ; i++) {

        double r = (view.closes[widx  -1 - i]  - view.closes[ widx - 2 - i]) / ( view.closes[widx -2 -i]);

        returns.push_back(r);
    }

    size_t l = returns.size();


    double total = 0.0 ;
    double diff = 0.0 ;
    double var = 0.0;
    double vol = 0.0 ;

    for (size_t j = 0 ; j < l ; j++) {
        total += returns[j];
    }

    double mean = total/l ;

    for (size_t z = 0; z < l ; z++ ) {
        double curr = (returns[z]- mean );
        diff += std::pow(curr, 2);
    }

    var = diff / (l -1) ;

    vol = std::sqrt(var);

    return vol;


}

