#pragma once

#if __has_include("vendor/mlTradingArchitecture.h")
#include "vendor/mlTradingArchitecture.h"
#else
#error "Missing necessary vendor, download via github.com/b0owl/ML-Trading-Architecture"
#endif

/// @brief length of the streak of consecutive up-ticks or down-ticks ending at
/// idx, signed by direction (+3 = three up-ticks in a row), capped at `maxLookback`
double tickRunLengthFeatureAt(unsigned int idx, unsigned int maxLookback) {
    const std::string_view contract = activeContract(engine);

    double lastPrice = requestIdx(engine, idx);
    std::optional<unsigned int> cursor = prevMatchingIdx(engine, idx, contract);

    int direction = 0;
    unsigned int runLength = 0;

    for (unsigned int i = 0; i < maxLookback; ++i) {
        if (!cursor) break;

        const double price = requestIdx(engine, *cursor);
        const double diff = lastPrice - price;
        const int step = (diff > 0.0) - (diff < 0.0);

        if (step == 0) break; // flat tick ends the streak

        if (direction == 0) direction = step;
        if (step != direction) break;

        ++runLength;
        lastPrice = price;
        cursor = prevMatchingIdx(engine, *cursor, contract);
    }

    return static_cast<double>(direction) * static_cast<double>(runLength);
}

// use this and not the above
std::vector<double> tickRunLengthFeature(unsigned int idx, const AgentParameter& maxLookback) {
    return {tickRunLengthFeatureAt(idx, static_cast<unsigned int>(maxLookback.value))};
}
