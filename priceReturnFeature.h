#pragma once

#if __has_include("vendor/QModel.h")
#include "vendor/QModel.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/QModel"
#endif

/// @brief point return from `horizon` bars ago up to idx, positive means price
/// climbed, negative means it fell, plain momentum signal
double priceReturnFeatureAt(unsigned int idx, unsigned int horizon) {
    const std::string_view contract = activeContract(engine);
    const double currentPrice = requestIdx(engine, idx);

    std::optional<unsigned int> cursor = idx;
    for (unsigned int i = 0; i < horizon; ++i) {
        if (!cursor) break;
        cursor = prevMatchingIdx(engine, *cursor, contract);
    }
    if (!cursor) return 0.0;

    const double pastPrice = requestIdx(engine, *cursor);
    return toPoints(kMLConfig.tickSize, kMLConfig.tickValue, currentPrice - pastPrice);
}

// use this and not the above
std::vector<double> priceReturnFeature(unsigned int idx, const AgentParameter& horizon) {
    return {priceReturnFeatureAt(idx, static_cast<unsigned int>(horizon.value))};
}
