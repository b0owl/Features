#pragma once

#if __has_include("vendor/QModel.h")
#include "vendor/QModel.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/QModel"
#endif

/// @brief distance in points between the current price and the volume-weighted
/// average price over the last `horizon` ticks, positive = trading above vwap
double vwapDeviationFeatureAt(unsigned int idx, unsigned int horizon) {
    const std::string_view contract = activeContract(engine);
    const unsigned int pointsNeeded = horizon + 1;

    double sumPV = 0.0;
    double sumV = 0.0;
    double currentPrice = 0.0;
    bool haveCurrent = false;

    std::optional<unsigned int> current = idx;
    for (unsigned int i = 0; i < pointsNeeded; ++i) {
        if (!current) break;

        engine.md.setCursor(*current);
        DataWindow row = engine.handler.requestDataWindow(engine.md, 1);
        if (!row.prices.empty() && !row.volumes.empty()) {
            const double price = row.prices[0];
            const double volume = std::max(row.volumes[0], 0.0);

            if (!haveCurrent) { currentPrice = price; haveCurrent = true; }

            sumPV += price * volume;
            sumV += volume;
        }

        current = prevMatchingIdx(engine, *current, contract);
    }

    if (sumV < 1e-9) return 0.0;

    const double vwap = sumPV / sumV;
    return toPoints(kMLConfig.tickSize, kMLConfig.tickValue, currentPrice - vwap);
}

// use this and not the above
std::vector<double> vwapDeviationFeature(unsigned int idx, const AgentParameter& horizon) {
    return {vwapDeviationFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
