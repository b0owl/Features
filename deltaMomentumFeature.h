#pragma once

#if __has_include("vendor/mlTradingArchitecture.h")
#include "vendor/mlTradingArchitecture.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/ML-Trading-Architecture"
#endif

/// @brief acceleration of orderflow, sums CVD over the recent half of a
/// `horizon`-tick window minus the prior half, positive = flow is speeding up
double deltaMomentumFeatureAt(unsigned int idx, unsigned int horizon) {
    const std::string_view contract = activeContract(engine);
    const unsigned int pointsNeeded = horizon * 2;

    std::vector<double> deltas;
    deltas.reserve(pointsNeeded);

    std::optional<unsigned int> current = idx;
    for (unsigned int i = 0; i < pointsNeeded; ++i) {
        if (!current) break;

        engine.md.setCursor(*current);
        DataWindow row = engine.handler.requestDataWindow(engine.md, 1);
        deltas.push_back(row.deltas.empty() ? 0.0 : row.deltas[0]);

        current = prevMatchingIdx(engine, *current, contract);
    }
    std::reverse(deltas.begin(), deltas.end()); // oldest -> newest

    if (deltas.size() < 2) return 0.0;

    const size_t mid = deltas.size() / 2;
    double priorSum = 0.0;
    double recentSum = 0.0;
    for (size_t i = 0; i < mid; ++i) priorSum += deltas[i];
    for (size_t i = mid; i < deltas.size(); ++i) recentSum += deltas[i];

    return recentSum - priorSum;
}

// use this and not the above
std::vector<double> deltaMomentumFeature(unsigned int idx, const AgentParameter& horizon) {
    return {deltaMomentumFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
