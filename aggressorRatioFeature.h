#pragma once

#if __has_include("vendor/mlTradingArchitecture.h")
#include "vendor/mlTradingArchitecture.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/ML-Trading-Architecture"
#endif

/// @brief net aggressor split over the last `horizon` ticks, +1 = everything
/// traded on the ask (all buying), -1 = everything traded on the bid
double aggressorRatioFeatureAt(unsigned int idx, unsigned int horizon) {
    const std::string_view contract = activeContract(engine);
    const unsigned int pointsNeeded = horizon + 1;

    double sumBuys = 0.0;
    double sumSells = 0.0;

    std::optional<unsigned int> current = idx;
    for (unsigned int i = 0; i < pointsNeeded; ++i) {
        if (!current) break;

        engine.md.setCursor(*current);
        DataWindow row = engine.handler.requestDataWindow(engine.md, 1);
        if (!row.executedBuys.empty()) sumBuys += row.executedBuys[0];
        if (!row.executedSells.empty()) sumSells += row.executedSells[0];

        current = prevMatchingIdx(engine, *current, contract);
    }

    const double total = sumBuys + sumSells;
    if (total < 1e-9) return 0.0;

    return (sumBuys - sumSells) / total;
}

// use this and not the above
std::vector<double> aggressorRatioFeature(unsigned int idx, const AgentParameter& horizon) {
    return {aggressorRatioFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
