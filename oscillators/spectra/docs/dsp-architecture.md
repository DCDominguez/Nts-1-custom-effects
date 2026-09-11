# SPECTRA DSP Architecture

## Why the oscillator is mono

The original NTS-1 user oscillator callback receives an `int32_t *yn` output buffer with **one sample per frame**. SPECTRA therefore creates several internal pitches but sums them before the NTS-1 host voice path.

Do not design per-voice pan inside this unit. That belongs in a downstream effect.

## MkI density ceiling

The current MkI production architecture is limited to **three internal voices**. The earlier fourth voice was removed after physical listening found that SPECTRA sounded great through three voices but degraded at four.

The three-voice ceiling is therefore a musical/hardware design constraint, not just an optimization.

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

- advance up to three audio phases
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

Harmonic, spread, and chaos ratios are precomputed when their source parameter/event changes.

Drift depth is small enough that a first-order cents-to-ratio approximation is used:

```text
ratio ≈ 1 + cents * ln(2) / 1200
```

## Wave morph

`SHAPE` plus the host `shape_lfo` selects a continuous position across four waveform families:

```text
sine -> triangle -> band-limited saw -> band-limited square
```

The SDK oscillator lookup functions are used for sine and band-limited discontinuous waveforms.

## Band-limit selection

SPECTRA chooses a conservative table using the base note plus the voice's upward interval contribution.

High-register CLUSTER/OCTAVE modes need special attention because upper interval voices approach the edge of the useful range sooner than ordinary unison.

## Normalization

Each active voice has a base weight. Chaos may perturb it within a small range.

The sum is normalized by total active weight rather than a fixed divisor. This keeps one-, two-, and three-voice output levels closer together.

Final soft clipping is a safety stage, not the primary gain strategy.

## Three-voice spread geometry

```text
1 voice :  0
2 voices: -1, +1
3 voices: -1,  0, +1
```

This keeps three-voice unison spread centered around the played pitch.

## Deterministic zero state

At:

```text
Spread = 0
Drift = 0
Chaos = 0
ALT = 0
HarmMode = UNISON
```

all active voices collapse to deterministic unison behavior. Repeated note-ons must reproduce the same result.

This is the reference state for debugging.

## Safety rule

Even if a host/test sends an out-of-range `Voices` parameter, production DSP hard-clamps to three active voices. A fourth voice cannot be re-enabled accidentally through parameter injection.
