#pragma once

#if __has_include("vendor/mlTradingArchitecture.h")
#include "vendor/mlTradingArchitecture.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/ML-Trading-Architecture"
#endif

/// @brief stdev of tick-to-tick point returns over the last `horizon` bars,
/// a plain dispersion/vol-regime read
double rollingVolatilityFeatureAt(unsigned int idx, unsigned int horizon) {
    const std::string_view contract = activeContract(engine);
    const unsigned int pointsNeeded = horizon + 1;

    std::vector<double> prices;
    prices.reserve(pointsNeeded);
    std::optional<unsigned int> current = idx;
    for (unsigned int i = 0; i < pointsNeeded; ++i) {
        if (!current) break;
        prices.push_back(requestIdx(engine, *current));
        current = prevMatchingIdx(engine, *current, contract);
    }
    std::reverse(prices.begin(), prices.end());

    if (prices.size() < 2) return 0.0;

    std::vector<double> deltas;
    deltas.reserve(prices.size() - 1);
    for (size_t i = 1; i < prices.size(); ++i) {
        deltas.push_back(toPoints(kMLConfig.tickSize, kMLConfig.tickValue, prices[i] - prices[i - 1]));
    }

    double mean = 0.0;
    for (double d : deltas) mean += d;
    mean /= static_cast<double>(deltas.size());

    double variance = 0.0;
    for (double d : deltas) variance += (d - mean) * (d - mean);
    variance /= static_cast<double>(deltas.size());

    return std::sqrt(variance);
}

// use this and not the above
std::vector<double> rollingVolatilityFeature(unsigned int idx, const AgentParameter& horizon) {
    return {rollingVolatilityFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
