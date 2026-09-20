#pragma once

#if __has_include("vendor/QModel.h")
#include "vendor/QModel.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/QModel"
#endif

/// @brief sqrt of summed squared tick-to-tick point returns over `horizon`
/// bars, realized vol without demeaning (unlike rollingVolatilityFeature)
double realizedVolatilityFeatureAt(unsigned int idx, unsigned int horizon) {
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

    double sumSquared = 0.0;
    for (size_t i = 1; i < prices.size(); ++i) {
        const double d = toPoints(kMLConfig.tickSize, kMLConfig.tickValue, prices[i] - prices[i - 1]);
        sumSquared += d * d;
    }

    return std::sqrt(sumSquared);
}

// use this and not the above
std::vector<double> realizedVolatilityFeature(unsigned int idx, const AgentParameter& horizon) {
    return {realizedVolatilityFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
