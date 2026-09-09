# DUST

DUST is a clean-room Human Soon modulation effect for the original Korg Nu:Tekt NTS-1 digital kit.

It explores the broad lo-fi processor class represented by public logue effects such as sample-rate and bit-depth reducers, but uses an independent implementation and control design.

## M1 concept

DUST degrades audio in two digital domains:

- **RATE** — lowers the effective sampling rate with sample-and-hold reduction.
- **DAMAGE** — crossfades into the degraded signal while reducing quantization depth.

At more extreme settings, DUST deliberately allows the left and right reduction clocks to diverge slightly. This **stereo fracture** is our own extension: a mono source can develop a narrow moving digital stereo image instead of both channels producing exactly the same alias pattern.

## NTS-1 controls

| NTS-1 modulation control | DUST function |
|---|---|
| TIME | RATE |
| DEPTH | DAMAGE |

The effect uses the official `modfx` API 1.1-0 target for `nutekt-digital` and keeps `num_param = 0`, as required for custom effects.

## Current status

M1 source scaffold. Must compile in CI and pass the hardware QA sheet before any milestone is marked hardware-valid.
