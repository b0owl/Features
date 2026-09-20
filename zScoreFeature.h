#pragma once

#if __has_include("vendor/QModel.h")
#include "vendor/QModel.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/QModel"
#endif

/// @brief how many stdevs the current price sits from its rolling mean over
/// `horizon` bars, classic mean-reversion read (far from 0 = stretched)
double zScoreFeatureAt(unsigned int idx, unsigned int horizon) {
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

    double mean = 0.0;
    for (double p : prices) mean += p;
    mean /= static_cast<double>(prices.size());

    double variance = 0.0;
    for (double p : prices) variance += (p - mean) * (p - mean);
    variance /= static_cast<double>(prices.size());

    const double stdev = std::sqrt(variance);
    if (stdev < 1e-9) return 0.0;

    return (prices.back() - mean) / stdev;
}

// use this and not the above
std::vector<double> zScoreFeature(unsigned int idx, const AgentParameter& horizon) {
    return {zScoreFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
