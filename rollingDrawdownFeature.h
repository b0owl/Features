#pragma once

#if __has_include("vendor/QModel.h")
#include "vendor/QModel.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/QModel"
#endif

/// @brief biggest peak-to-trough drop in points over the last `horizon` bars,
/// how much pain would you have sat through holding through this window
double rollingDrawdownFeatureAt(unsigned int idx, unsigned int horizon) {
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

    double peak = prices[0];
    double maxDrawdown = 0.0;
    for (double p : prices) {
        peak = std::max(peak, p);
        maxDrawdown = std::max(maxDrawdown, peak - p);
    }

    return toPoints(kMLConfig.tickSize, kMLConfig.tickValue, maxDrawdown);
}

// use this and not the above
std::vector<double> rollingDrawdownFeature(unsigned int idx, const AgentParameter& horizon) {
    return {rollingDrawdownFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
