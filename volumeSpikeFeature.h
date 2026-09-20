#pragma once

#if __has_include("vendor/QModel.h")
#include "vendor/QModel.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/QModel"
#endif

/// @brief current tick's volume vs its rolling average over the last `horizon`
/// ticks, 1.0 = normal, >1 = a spike, <1 = quiet
double volumeSpikeFeatureAt(unsigned int idx, unsigned int horizon) {
    const std::string_view contract = activeContract(engine);
    const unsigned int pointsNeeded = horizon + 1;

    std::vector<double> volumes;
    volumes.reserve(pointsNeeded);

    std::optional<unsigned int> current = idx;
    for (unsigned int i = 0; i < pointsNeeded; ++i) {
        if (!current) break;

        engine.md.setCursor(*current);
        DataWindow row = engine.handler.requestDataWindow(engine.md, 1);
        if (!row.volumes.empty()) volumes.push_back(row.volumes[0]);

        current = prevMatchingIdx(engine, *current, contract);
    }

    if (volumes.size() < 2) return 1.0;

    const double currentVolume = volumes.front(); // idx itself, pushed first
    double avgVolume = 0.0;
    for (double v : volumes) avgVolume += v;
    avgVolume /= static_cast<double>(volumes.size());

    if (avgVolume < 1e-9) return 1.0;

    return currentVolume / avgVolume;
}

// use this and not the above
std::vector<double> volumeSpikeFeature(unsigned int idx, const AgentParameter& horizon) {
    return {volumeSpikeFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
