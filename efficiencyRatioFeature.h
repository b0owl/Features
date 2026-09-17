#pragma once

#if __has_include("vendor/mlTradingArchitecture.h")
#include "vendor/mlTradingArchitecture.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/ML-Trading-Architecture"
#endif

/// @brief Kaufman efficiency ratio over `horizon` bars, net move divided by
/// the total path length, near 1 = clean trend, near 0 = noisy chop
double efficiencyRatioFeatureAt(unsigned int idx, unsigned int horizon) {
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

    const double netMove = std::abs(prices.back() - prices.front());

    double pathLength = 0.0;
    for (size_t i = 1; i < prices.size(); ++i) {
        pathLength += std::abs(prices[i] - prices[i - 1]);
    }

    if (pathLength < 1e-9) return 0.0;

    return netMove / pathLength;
}

// use this and not the above
std::vector<double> efficiencyRatioFeature(unsigned int idx, const AgentParameter& horizon) {
    return {efficiencyRatioFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
