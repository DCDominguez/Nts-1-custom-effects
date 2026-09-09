# CARRIER

CARRIER is a clean-room Human Soon modulation effect for the original Korg Nu:Tekt NTS-1 digital kit.

It explores the broad ring/amplitude-modulation processor class represented in the public logue ecosystem, but uses an independent control model and implementation.

## M1 concept

CARRIER uses a continuous sine carrier and one two-stage intensity macro:

- **FREQUENCY** — sweeps the carrier from sub-audio tremolo territory into audio-rate metallic sidebands.
- **POLARITY** — moves continuously from dry signal to unipolar amplitude modulation, then from amplitude modulation to bipolar ring modulation.

At the ring-modulation end of POLARITY, the left and right carrier phases separate by a bounded amount. This **sideband split** creates stereo width from a mono source without allowing the two channels to become opposite-polarity copies.

## NTS-1 controls

| NTS-1 modulation control | CARRIER function |
|---|---|
| TIME | FREQUENCY |
| DEPTH | POLARITY |

POLARITY behavior:

```text
0%      dry
50%     full unipolar AM
100%    full bipolar ring modulation
```

The effect targets the official original-NTS-1 `modfx` API `1.1-0` and keeps `num_param = 0` as required for custom effects.

## Current status

M1 source implemented. Compilation and physical MkI validation are separate gates. Complete `docs/qa-sheet.md` before marking the unit hardware-valid.
