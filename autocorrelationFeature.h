#pragma once

#if __has_include("vendor/QModel.h")
#include "vendor/QModel.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/QModel"
#endif

/// @brief lag-1 autocorrelation of tick-to-tick point returns over `horizon`
/// bars, positive = momentum regime, negative = mean-reverting/choppy regime
double autocorrelationFeatureAt(unsigned int idx, unsigned int horizon) {
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

    if (prices.size() < 3) return 0.0;

    std::vector<double> deltas;
    deltas.reserve(prices.size() - 1);
    for (size_t i = 1; i < prices.size(); ++i) {
        deltas.push_back(toPoints(kMLConfig.tickSize, kMLConfig.tickValue, prices[i] - prices[i - 1]));
    }

    double mean = 0.0;
    for (double d : deltas) mean += d;
    mean /= static_cast<double>(deltas.size());

    double numerator = 0.0;
    for (size_t i = 0; i + 1 < deltas.size(); ++i) {
        numerator += (deltas[i] - mean) * (deltas[i + 1] - mean);
    }

    double denominator = 0.0;
    for (double d : deltas) denominator += (d - mean) * (d - mean);

    if (denominator < 1e-9) return 0.0;

    return numerator / denominator;
}

// use this and not the above
std::vector<double> autocorrelationFeature(unsigned int idx, const AgentParameter& horizon) {
    return {autocorrelationFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
