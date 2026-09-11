# SPECTRA Test Plan

SPECTRA should be proven in layers on the physical original NTS-1.

## M0 — Toolchain

Pass when:

- Korg `logue-sdk` clones successfully
- submodules initialize
- official `platform/nutekt-digital/dummy-osc` builds
- `make install` creates an `.ntkdigunit`
- the stock unit loads on the physical NTS-1

Do not debug SPECTRA before the stock oscillator template passes.

## Current hardware rule — three voices maximum

The previous 0.2-0 build exposed four internal voices. Physical MkI listening found SPECTRA excellent through three voices but degraded at four. Version 0.2-1 therefore hard-caps the MkI build at **1–3 voices**.

Testing must now confirm both:

- the `Voices` control exposes only 1–3;
- out-of-range parameter values cannot reactivate a fourth voice.

## Stable three-voice source

### Test setup

Use the NTS-1 with:

- filter mostly open
- resonance low
- minimal modulation FX
- delay/reverb off initially
- simple host envelope

Monitor at a conservative level.

### Pitch tracking

Test chromatically over at least:

```text
C2 -> C6
```

Confirm:

- single-voice mode tracks the keyboard/ribbon/MIDI pitch
- fine pitch transitions do not produce obvious stepping
- no voice becomes stuck at a previous pitch

### Voices

Test `Voices` values 1–3.

Pass when density increases without a large output-level jump and three voices retain the physically approved musical quality.

### SHAPE

Sweep from minimum to maximum slowly.

Expected path:

```text
sine -> triangle -> saw -> square
```

Listen for clicks, discontinuous gain jumps, strong aliasing, and DC-like offsets.

### Spread

With `HarmMode = UNISON`, `ALT = 0`, `Drift = 0`, `Chaos = 0`:

- Spread 0% should collapse tuning
- increasing Spread should create stable beating
- maximum Spread should remain centered overall
- three-voice geometry should remain approximately `-width / center / +width`

### Harmonic modes

Set ALT to maximum and test all eight modes:

```text
UNISON
FIFTH
OCTAVE
MAJOR
MINOR
SUS
QUARTAL
CLUSTER
```

Verify the root remains present and the expected three-voice interval character is audible.

### ALT

For each harmonic mode:

- ALT 0 = unison
- ALT 50% = intermediate interval distances
- ALT 100% = full three-voice constellation

The transition should be continuous rather than switching suddenly.

## Drift

With Spread = 0 and Chaos = 0:

- set Drift above 0
- confirm voices move independently
- confirm no obvious shared vibrato dominates
- sweep Motion from minimum to maximum
- listen for excessive seasick modulation at the top end

Run sustained notes for 1–2 minutes to hear long drift cycles.

## Chaos

At Chaos = 0, repeatedly trigger the same note and record it. Repeats should be effectively deterministic.

Increase Chaos in stages:

```text
25%
50%
75%
100%
```

Check that successive note-ons vary in phase character, fine tuning, and internal balance while remaining bounded and musically related to the patch.

## Host LFO interaction

Set the NTS-1 LFO target to shape and verify `shape_lfo` moves the SPECTRA waveform morph without corrupting pitch or causing parameter jumps.

## High-note alias test

Use SAW/SQUARE regions of SHAPE and test high notes, especially with upward interval modes.

Priority modes:

```text
OCTAVE
QUARTAL
CLUSTER
```

If aliasing becomes objectionable, reduce upper-voice contribution or choose a more conservative band-limit table at high notes.

## Level/stability test

Test worst-case approved density:

```text
Voices = 3
Spread = 100%
Drift = 100%
HarmMode = CLUSTER or OCTAVE
ALT = 100%
Motion = 100%
Chaos = 100%
```

Pass when:

- no runaway level
- no NaN/Inf-like blasts
- no oscillator lockup
- no persistent DC offset
- three-voice sound remains musically intact

## A-class deterministic host gate

`tests/pre-handoff/unit_specific/test_spectra.cpp` must prove:

- single-voice pitch tracking
- SHAPE harmonic change
- three-voice Spread delivery
- root/fifth/octave delivery in the reference harmonic mode
- Drift and Motion behavior
- Chaos deterministic/randomized contracts
- finite/bounded output
- hard three-voice ceiling even when an out-of-range voice parameter is injected

## Physical 0.2-1 regression gate

The new binary must be loaded on the original NTS-1 MkI and checked at Voices 1, 2 and 3.

Pass when:

- unit loads/selects normally;
- Voices cannot reach 4;
- 1–3 retain the tone/musicality approved in the prior hardware pass;
- no new level, aliasing, or note-transition defect is introduced by the cap.

See `reports/testing/2026-09-12_spectra-three-voice-cap.md`.
