#pragma once

#if __has_include("vendor/QModel.h")
#include "vendor/QModel.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/QModel"
#endif

/// @brief where price sits in its own `horizon`-bar high/low range, rescaled
/// to -1..1 (-1 = at the low, +1 = at the high)
double rangePositionFeatureAt(unsigned int idx, unsigned int horizon) {
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

    const double low = *std::min_element(prices.begin(), prices.end());
    const double high = *std::max_element(prices.begin(), prices.end());
    if (high - low < 1e-9) return 0.0;

    const double position = (prices.back() - low) / (high - low);
    return position * 2.0 - 1.0;
}

// use this and not the above
std::vector<double> rangePositionFeature(unsigned int idx, const AgentParameter& horizon) {
    return {rangePositionFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
