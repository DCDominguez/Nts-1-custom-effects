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

## M1 — Stable four-voice source

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

Test `Voices` values 1–4.

Pass when density increases without a large output-level jump.

### SHAPE

Sweep from minimum to maximum slowly.

Expected path:

```text
sine -> triangle -> saw -> square
```

Listen for:

- clicks
- discontinuous gain jumps
- strong aliasing
- DC-like offsets

### Spread

With `HarmMode = UNISON`, `ALT = 0`, `Drift = 0`, `Chaos = 0`:

- Spread 0% should collapse tuning
- increasing Spread should create stable beating
- maximum Spread should remain centered overall

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

Verify the root remains present and the expected interval character is audible.

### ALT

For each harmonic mode:

- ALT 0 = unison
- ALT 50% = intermediate interval distances
- ALT 100% = full constellation

The transition should be continuous rather than switching suddenly.

## M2 — Drift

With Spread = 0 and Chaos = 0:

- set Drift above 0
- confirm voices move independently
- confirm no obvious shared vibrato dominates
- sweep Motion from minimum to maximum
- listen for excessive seasick modulation at the top end

Run sustained notes for 1–2 minutes to hear long drift cycles.

## M3 — Chaos

At Chaos = 0, repeatedly trigger the same note and record it. Repeats should be effectively deterministic.

Increase Chaos in stages:

```text
25%
50%
75%
100%
```

Check that successive note-ons vary in:

- phase character
- fine tuning
- internal balance

But remain bounded and musically related to the patch.

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

Test worst-case density:

```text
Voices = 4
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
- stable for at least 30 minutes

## Integration test — PARALLAX

After SPECTRA passes independently:

```text
SPECTRA -> NTS-1 host voice -> PARALLAX
```

Check:

- whether four internal pitches remain intelligible after spatial processing
- mono collapse
- excessive comb cancellation
- whether SPECTRA Spread and PARALLAX Divergence duplicate each other too strongly

Document one conservative and one extreme recommended pairing.
