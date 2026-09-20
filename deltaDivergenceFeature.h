#pragma once

#if __has_include("vendor/QModel.h")
#include "vendor/QModel.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/QModel"
#endif

/// @brief flags price/orderflow divergence over the last `horizon` ticks,
/// -1 = price up but flow down (bearish divergence), +1 = price down but flow
/// up (bullish divergence), 0 = price and flow agree (or no clear move)
double deltaDivergenceFeatureAt(unsigned int idx, unsigned int horizon) {
    const std::string_view contract = activeContract(engine);

    const double currentPrice = requestIdx(engine, idx);

    engine.md.setCursor(idx);
    DataWindow currentRow = engine.handler.requestDataWindow(engine.md, 1);
    double cumulativeDelta = currentRow.deltas.empty() ? 0.0 : currentRow.deltas[0];

    std::optional<unsigned int> cursor = idx;
    for (unsigned int i = 0; i < horizon; ++i) {
        cursor = prevMatchingIdx(engine, *cursor, contract);
        if (!cursor) break;

        engine.md.setCursor(*cursor);
        DataWindow row = engine.handler.requestDataWindow(engine.md, 1);
        if (!row.deltas.empty()) cumulativeDelta += row.deltas[0];
    }
    if (!cursor) return 0.0;

    const double pastPrice = requestIdx(engine, *cursor);
    const double priceChange = currentPrice - pastPrice;

    const int priceDirection = (priceChange > 0.0) - (priceChange < 0.0);
    const int flowDirection = (cumulativeDelta > 0.0) - (cumulativeDelta < 0.0);

    if (priceDirection == 0 || flowDirection == 0 || priceDirection == flowDirection) return 0.0;

    return -static_cast<double>(priceDirection);
}

// use this and not the above
std::vector<double> deltaDivergenceFeature(unsigned int idx, const AgentParameter& horizon) {
    return {deltaDivergenceFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
