#pragma once

#if __has_include("vendor/QModel.h")
#include "vendor/QModel.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/QModel"
#endif

/// @brief RSI on a 0-1 scale (0.5 = neutral, no divide by 100/RS dance) over
/// the last `horizon` bars of point gains vs losses
double relativeStrengthFeatureAt(unsigned int idx, unsigned int horizon) {
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

    if (prices.size() < 2) return 0.5;

    double sumGain = 0.0;
    double sumLoss = 0.0;
    for (size_t i = 1; i < prices.size(); ++i) {
        const double d = toPoints(kMLConfig.tickSize, kMLConfig.tickValue, prices[i] - prices[i - 1]);
        if (d > 0.0) sumGain += d;
        else sumLoss += -d;
    }

    const double denom = sumGain + sumLoss;
    if (denom < 1e-9) return 0.5;

    return sumGain / denom;
}

// use this and not the above
std::vector<double> relativeStrengthFeature(unsigned int idx, const AgentParameter& horizon) {
    return {relativeStrengthFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
