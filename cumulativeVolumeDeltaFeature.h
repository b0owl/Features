#pragma once

#if __has_include("vendor/mlTradingArchitecture.h")
#include "vendor/mlTradingArchitecture.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/ML-Trading-Architecture"
#endif

/// @brief sum of orderflow delta (executedBuys - executedSells) over the last
/// `horizon` ticks, raw CVD, positive means buyers are in control
double cumulativeVolumeDeltaFeatureAt(unsigned int idx, unsigned int horizon) {
    const std::string_view contract = activeContract(engine);
    const unsigned int pointsNeeded = horizon + 1;

    double sumDelta = 0.0;
    std::optional<unsigned int> current = idx;
    for (unsigned int i = 0; i < pointsNeeded; ++i) {
        if (!current) break;

        engine.md.setCursor(*current);
        DataWindow row = engine.handler.requestDataWindow(engine.md, 1);
        if (!row.deltas.empty()) sumDelta += row.deltas[0];

        current = prevMatchingIdx(engine, *current, contract);
    }

    return sumDelta;
}

// use this and not the above
std::vector<double> cumulativeVolumeDeltaFeature(unsigned int idx, const AgentParameter& horizon) {
    return {cumulativeVolumeDeltaFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
