# SPECTRA DSP Architecture

## Why the oscillator is mono

The original NTS-1 user oscillator callback receives an `int32_t *yn` output buffer with **one sample per frame**. SPECTRA therefore creates several internal pitches but sums them before the NTS-1 host voice path.

Do not design per-voice pan inside this unit. That belongs in a downstream effect.

## Audio/control split

Expensive or state-setting work should happen outside the inner sample loop whenever possible.

### Parameter / event rate

Recalculate on parameter changes or NOTE ON:

- active voice count
- interval ratios
- static spread ratios
- chaos phase offsets
- chaos tuning offsets
- chaos level offsets
- drift rate increments

### Buffer rate

Recalculate once per `OSC_CYCLE()`:

- host base phase increment from `params->pitch`
- waveform band-limit index per voice
- current host `shape_lfo`

### Sample rate

Per sample:

- advance four audio phases
- advance active drift phases
- calculate bounded drift cents
- render waveform for active voices
- weighted sum
- normalize / soft clip
- convert float to Q31

## Pitch model

```text
voice_increment =
    host_base_increment
  * harmonic_ratio
  * spread_ratio
  * chaos_ratio
  * drift_ratio(sample)
```

Harmonic, spread, and chaos ratios should be precomputed when their source parameter/event changes.

Drift depth is small enough that a first-order cents-to-ratio approximation is acceptable for M1/M2 testing:

```text
ratio ≈ 1 + cents * ln(2) / 1200
```

If hardware listening reveals beating asymmetry or tuning error, replace this with a higher-accuracy bounded approximation.

## Wave morph

`SHAPE` plus the host `shape_lfo` selects a continuous position across four waveform families:

```text
sine -> triangle -> band-limited saw -> band-limited square
```

The SDK's own oscillator lookup functions should be preferred for sine and band-limited discontinuous waveforms.

## Band-limit selection

The SDK provides seven band-limited saw/square tables associated with note regions around:

```text
36, 48, 60, 72, 84, 108, 127
```

SPECTRA chooses a conservative table using the base note plus the voice's upward interval contribution.

High-register CLUSTER/OCTAVE modes need special attention because the upper voices approach the edge of the useful range sooner than an ordinary unison oscillator.

## Normalization

Each active voice has a base weight. Chaos may perturb it within a small range.

The sum is normalized by total active weight rather than simply dividing by four. This keeps one-voice and four-voice output levels closer together.

Any final soft clipping is a safety stage, not the primary gain strategy.

## Deterministic zero state

At:

```text
Spread = 0
Drift = 0
Chaos = 0
ALT = 0
HarmMode = UNISON
```

all active voices should collapse to deterministic unison behavior. Their phase anchors may still differ by design, but repeated note-ons must reproduce the same result.

This is the reference state for debugging.
