#pragma once

#if __has_include("vendor/QModel.h")
#include "vendor/QModel.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/QModel"
#endif

/// @brief average absolute tick-to-tick point move over `horizon` bars, a
/// cheap noise/choppiness proxy when you don't have a real spread column
double spreadProxyFeatureAt(unsigned int idx, unsigned int horizon) {
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

    double sumAbs = 0.0;
    for (size_t i = 1; i < prices.size(); ++i) {
        sumAbs += std::abs(toPoints(kMLConfig.tickSize, kMLConfig.tickValue, prices[i] - prices[i - 1]));
    }

    return sumAbs / static_cast<double>(prices.size() - 1);
}

// use this and not the above
std::vector<double> spreadProxyFeature(unsigned int idx, const AgentParameter& horizon) {
    return {spreadProxyFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
