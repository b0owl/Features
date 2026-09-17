#pragma once

#if __has_include("vendor/mlTradingArchitecture.h")
#include "vendor/mlTradingArchitecture.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/ML-Trading-Architecture"
#endif

/// @brief ticks per second over the last `horizon` ticks, a read on how fast
/// the tape is moving right now (busy market vs dead market)
double tradeIntensityFeatureAt(unsigned int idx, unsigned int horizon) {
    const std::string_view contract = activeContract(engine);

    engine.md.setCursor(idx);
    engine.handler.requestDataWindow(engine.md, 1);
    if (engine.handler.windowTimestamps.empty()) return 0.0;
    const long long endEpoch = engine.handler.windowTimestamps[0];

    std::optional<unsigned int> cursor = idx;
    for (unsigned int i = 0; i < horizon; ++i) {
        if (!cursor) break;
        cursor = prevMatchingIdx(engine, *cursor, contract);
    }
    if (!cursor) return 0.0;

    engine.md.setCursor(*cursor);
    engine.handler.requestDataWindow(engine.md, 1);
    if (engine.handler.windowTimestamps.empty()) return 0.0;
    const long long startEpoch = engine.handler.windowTimestamps[0];

    const double elapsedSeconds = static_cast<double>(endEpoch - startEpoch);
    if (elapsedSeconds <= 0.0) return 0.0;

    return static_cast<double>(horizon) / elapsedSeconds;
}

// use this and not the above
std::vector<double> tradeIntensityFeature(unsigned int idx, const AgentParameter& horizon) {
    return {tradeIntensityFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
