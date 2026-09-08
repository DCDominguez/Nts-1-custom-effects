# PARALLAX DSP Architecture

## Signal path

```text
stereo input
   │
   ├────────────────────────────────────────────── dry
   │
   ▼
stereo circular buffer
   │
   ├─ tap A ─ motion ─ [pitch M2] ─ pan A ─┐
   ├─ tap B ─ motion ─ [pitch M3] ─ pan B ─┤
   ├─ tap C ─ motion ─ [pitch M3] ─ pan C ─┤─ normalized wet sum
   └─ tap D ─ motion ─ [pitch M3] ─ pan D ─┘
                                                  │
                              dry ────────────────┴─ output
```

## M1 voice constants

The first hardware build is intentionally conservative. It proves the field before implementing true pitch shifting.

Nominal voice constellation:

| Voice | Base delay | LFO rate | Max motion | Pan |
|---|---:|---:|---:|---:|
| A | 8 ms | 0.17 Hz | 1.2 ms | -0.75 |
| B | 14 ms | 0.23 Hz | 1.7 ms | -0.20 |
| C | 23 ms | 0.11 Hz | 2.1 ms | +0.25 |
| D | 36 ms | 0.31 Hz | 2.6 ms | +0.78 |

The exact values are listening-test targets, not API requirements.

## Delay memory

M1 uses one circular buffer per input channel and four fractional read positions. It does **not** allocate one full delay buffer per voice.

This is important: the voice is a read head, not a duplicate recording.

```text
                 write head
                     ▼
[────────────────────●──────────────]
   ▲       ▲       ▲        ▲
 voice D voice C voice B  voice A
```

Each tap reads behind the shared write head by its own delay amount.

## Fractional reads

Voice modulation moves read positions by fractions of a sample, so the first build uses linear interpolation:

```text
y = a + frac * (b - a)
```

Higher-order interpolation is deferred until hardware testing demonstrates that linear interpolation is audibly limiting.

## LFOs

All voices use independent phase accumulators.

```text
phase += rate / sample_rate
phase = wrap(phase)
mod = sin(2π * phase)
```

The Korg effects runtime provides sine lookup helpers, so the implementation can avoid the standard-library `sin()` inside the audio loop.

The key design rule is **decorrelation**:

- different frequencies
- different starting phases
- different modulation depths

No master LFO fans out to all four voices.

## SPREAD macro

SPREAD changes the base timing geometry rather than adding the same offset to every tap.

Conceptually:

```text
voice_delay = minimum_delay + constellation_offset * spread_scale
```

This preserves the ordering of voices while expanding the field.

## DIVERGENCE macro

DIVERGENCE affects several dimensions at once:

```text
motion_depth = base_motion * divergence
pan          = base_pan * stereo_width(divergence)
```

When true pitch shifting arrives:

```text
pitch_cents = target_cents * divergence
```

The macro should sound like the ensemble separating into more distinct performers, not merely increasing modulation depth.

## Stereo panning

M1 uses a lightweight constant-power approximation only if CPU cost is acceptable; otherwise simple linear gains are acceptable for the prototype.

A voice's pan anchor is static in M1.

M5 can add bounded pan drift around the anchor. Whole-field autopanning is explicitly not the goal.

## Wet normalization

Four taps summed at unity can overload quickly. The prototype scales voice gain before summing and clamps/soft-limits only as a safety net.

Normal operation should not depend on the limiter.

## M2 pitch shifter

A stable pitch offset cannot be produced by simply wobbling one read head. M2 therefore introduces a separate pitch-shift primitive.

Candidate algorithm:

```text
circular audio history
       │
       ├─ read head 1 ─ window 1 ─┐
       └─ read head 2 ─ window 2 ─┤─ sum
                                  │
                   heads move at pitch ratio
```

The two windows are offset so one head fades out while the other resets/fades in.

Pitch ratio:

```text
ratio = 2^(cents / 1200)
```

For the embedded implementation, ratios should be precomputed or calculated when parameters change rather than per audio sample.

## CPU rule

Do not assume four true pitch shifters fit.

Profile in this order:

```text
M1: 4 ordinary modulated taps
M2: + 1 pitch shifter
M3a: + 2 pitch shifters
M3b: + 4 pitch shifters only if safe
```

A musically strong `2 shifted + 2 chorus` design is an acceptable release architecture if four shifters compromise stability.

## Reuse in CHORDGHOST

The pitch-shift primitive should ultimately have no PARALLAX-specific harmony logic.

Ideal boundary:

```cpp
PitchVoice.process(input_history, semitone_or_cent_offset, ...)
```

PARALLAX supplies fixed constellation offsets. CHORDGHOST can later supply chord-derived offsets.
