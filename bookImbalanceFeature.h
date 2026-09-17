#pragma once

#if __has_include("vendor/mlTradingArchitecture.h")
#include "vendor/mlTradingArchitecture.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/ML-Trading-Architecture"
#endif

/// @brief resting bid/ask size imbalance at idx, +1 = book stacked on the bid,
/// -1 = stacked on the ask, single tick snapshot so no horizon needed
double bookImbalanceFeatureAt(unsigned int idx) {
    engine.md.setCursor(idx);
    DataWindow row = engine.handler.requestDataWindow(engine.md, 1);
    if (row.restingBids.empty() || row.restingAsks.empty()) return 0.0;

    const double bid = std::max(row.restingBids[0], 0.0);
    const double ask = std::max(row.restingAsks[0], 0.0);
    const double total = bid + ask;
    if (total < 1e-9) return 0.0;

    return (bid - ask) / total;
}

// use this and not the above
std::vector<double> bookImbalanceFeature(unsigned int idx) {
    return {bookImbalanceFeatureAt(idx)};
}
