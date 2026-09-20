Standalone features that can be fed to an ML model.

## Setup

These headers include the amalgamated single-header build of QModel, not the
multi-file `src/` version. You need to generate it once:

1. In `QModel/`, run `./amalgamate.sh`. This produces a `QModel/` folder
   containing `QModel.h` and the GLFW headers.
2. Copy (or symlink) that `QModel/` folder into this directory as `vendor/`,
   so you end up with `Features/vendor/QModel.h`.

## Including a feature

Just include the file you want and call the wrapper:

```cpp
#include "priceReturnFeature.h"

AgentParameter returnHorizon(10.0, 1.0, 100.0, true);

std::vector<double> features = priceReturnFeature(state.idx, returnHorizon);
```

Multiple features combine by concatenating:

```cpp
#include "priceReturnFeature.h"
#include "cumulativeVolumeDeltaFeature.h"
#include "bookImbalanceFeature.h"

std::vector<double> features = priceReturnFeature(state.idx, returnHorizon);
auto cvd = cumulativeVolumeDeltaFeature(state.idx, cvdHorizon);
auto book = bookImbalanceFeature(state.idx); // no horizon needed

features.insert(features.end(), cvd.begin(), cvd.end());
features.insert(features.end(), book.begin(), book.end());
```

Most features take an `AgentParameter` horizon so the model can tune the
lookback window itself, same as `derivativeHorizon` in `main.cpp`. A couple
(`bookImbalanceFeature`, `tickRunLengthFeature` uses it as a lookback cap
rather than a fixed window) don't need one.

## The features

**priceReturnFeature** - point return from `horizon` bars ago to now. Plain
momentum: positive means price climbed, negative means it fell.

**rollingVolatilityFeature** - stdev of tick-to-tick point returns over
`horizon` bars. A vol-regime read.

**zScoreFeature** - how many stdevs the current price sits from its rolling
mean over `horizon` bars. Mean-reversion signal, big magnitude = stretched.

**relativeStrengthFeature** - RSI rescaled to 0-1 (0.5 = neutral) over
`horizon` bars of gains vs losses.

**cumulativeVolumeDeltaFeature** - sum of orderflow delta
(executedBuys - executedSells) over `horizon` ticks. Raw CVD, positive means
buyers are in control.

**vwapDeviationFeature** - distance in points between the current price and
the volume-weighted average price over `horizon` ticks.

**bookImbalanceFeature** - resting bid/ask size imbalance at a single tick,
+1 stacked on the bid, -1 stacked on the ask. No horizon, it's a snapshot.

**aggressorRatioFeature** - net aggressor split over `horizon` ticks, +1 =
everything traded on the ask (all buying), -1 = everything hit the bid.

**volumeSpikeFeature** - current tick's volume vs its rolling average over
`horizon` ticks. 1.0 = normal, above 1 = a spike, below 1 = quiet.

**tickRunLengthFeature** - length of the streak of consecutive up-ticks or
down-ticks ending at idx, signed by direction, capped at `maxLookback`.

**deltaDivergenceFeature** - flags price/orderflow divergence over `horizon`
ticks. -1 = price up but flow down (bearish divergence), +1 = price down but
flow up (bullish divergence), 0 = they agree.

**tradeIntensityFeature** - ticks per second over the last `horizon` ticks.
How fast the tape's moving right now.

**realizedVolatilityFeature** - sqrt of summed squared tick-to-tick point
returns over `horizon` bars. Realized vol without demeaning, unlike
rollingVolatilityFeature.

**returnSkewnessFeature** - skewness of tick-to-tick point returns over
`horizon` bars. Positive = occasional big up-moves, negative = same but down.

**rollingDrawdownFeature** - biggest peak-to-trough drop in points over
`horizon` bars.

**rangePositionFeature** - where price sits in its own `horizon`-bar
high/low range, rescaled to -1..1 (-1 at the low, +1 at the high).

**autocorrelationFeature** - lag-1 autocorrelation of tick-to-tick point
returns over `horizon` bars. Positive = momentum regime, negative =
mean-reverting/choppy regime.

**efficiencyRatioFeature** - Kaufman efficiency ratio over `horizon` bars,
net move divided by total path length. Near 1 = clean trend, near 0 = noisy
chop.

**deltaMomentumFeature** - acceleration of orderflow: CVD over the recent
half of a `horizon`-tick window minus the prior half. Positive = flow
speeding up.

**spreadProxyFeature** - average absolute tick-to-tick point move over
`horizon` bars. Cheap noise/choppiness proxy when there's no real spread
column.
