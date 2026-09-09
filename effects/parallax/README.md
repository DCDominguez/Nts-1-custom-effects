# PARALLAX

PARALLAX is a four-voice spatial delay / swarm field for the original Korg Nu:Tekt NTS-1.

The original M1/M2 tuning sat mostly in short-delay territory and could read too much like chorus. Hardware feedback on M2 confirmed that behavior. **0.2-1 deliberately widens both the temporal and stereo aperture** so the same control can move from tight doubling into clearly separated spatial arrivals.

```text
INPUT
  ├─ voice A: ~12 → 40 ms   / left edge / true micro-pitch voice
  ├─ voice B: ~20 → 160 ms  / right-mid
  ├─ voice C: ~32 → 360 ms  / left-mid
  └─ voice D: ~48 → 620 ms  / right edge
                     ↓
                 stereo sum
                     ↓
                    OUT
```

## Current implementation stage — 0.2-1

- `TIME / CC30` = **SPREAD** — opens the temporal aperture from tight doubling toward ~40 / 160 / 360 / 620 ms arrivals.
- `DEPTH / CC31` = **DIVERGENCE** — widens stereo anchors and retains Voice A's true micro-pitch shift toward about -9 cents.
- `SHIFT+DEPTH / CC33` = **MIX** — dry/wet balance.

At low SPREAD the unit can still function as a doubler/ensemble. At high SPREAD the modulation depth intentionally recedes so the listener hears **distinct time and stereo positions first**, rather than a chorus wash.

The current high-SPREAD stereo path alternates approximately:

`LEFT EDGE → RIGHT MID → LEFT MID → RIGHT EDGE`

This makes the sequence of arrivals trace a spatial path instead of simply thickening the center image.

## Build model

This folder overlays Korg's official `logue-sdk/platform/nutekt-digital/dummy-delfx` target. Korg's current `userdelfx.h` defines `TIME`, `DEPTH`, and the shift-accessible alternative depth parameter; custom delay effects write back into the stereo input/output buffer and are responsible for wet/dry balance.

## Why `delfx`

PARALLAX needs several independently addressed read positions and a wet/dry control, so the NTS-1 delay-effect slot remains the appropriate host. The 0.2-1 delay memory is 32768 samples per channel, giving roughly 683 ms of addressable history at the MkI's 48 kHz processing rate.
