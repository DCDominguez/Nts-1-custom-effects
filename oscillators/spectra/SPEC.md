# SPECTRA — NTS-1 Swarm Oscillator Specification

**Project:** SPECTRA  
**Target:** Korg Nu:Tekt NTS-1 digital kit (MkI)  
**Module:** `osc`  
**SDK:** logue SDK API `1.1-0`  
**Status:** Draft v0.1

## 1. Product idea

SPECTRA turns one played note into a controlled internal ensemble of up to four oscillator voices.

The voices share the same note event but can differ in:

- pitch offset
- static detune
- slow drift
- phase
- level
- waveform state

The design goal is **coherent instability**: the patch should remain identifiable while its internal relationships move.

## 2. Verified NTS-1 oscillator constraints

Korg's original NTS-1 user-oscillator API provides:

```c
void OSC_INIT(uint32_t platform, uint32_t api);
void OSC_CYCLE(const user_osc_param_t *params, int32_t *yn, uint32_t frames);
void OSC_NOTEON(const user_osc_param_t *params);
void OSC_NOTEOFF(const user_osc_param_t *params);
void OSC_PARAM(uint16_t index, uint16_t value);
```

`OSC_CYCLE()` writes one Q31 sample per frame. The oscillator output is therefore **mono** at this stage of the NTS-1 signal path.

The runtime pitch value is provided as:

```text
high byte = MIDI note
low byte  = fine position 0–255 toward the next semitone
```

The API supplies helpers including:

- `osc_w0f_for_note()`
- `osc_sinf()`
- band-limited saw/square lookup functions
- `osc_white()`
- `f32_to_q31()`

The NTS-1 oscillator API is fixed at 48 kHz.

The user oscillator can define up to six edit parameters. In addition, the NTS-1 supplies `SHAPE` and an alternate `ALT` parameter path.

## 3. Signal architecture

```text
host pitch
   |
   +--> V1 phase --> waveform --> level --+
   +--> V2 phase --> waveform --> level --+
   +--> V3 phase --> waveform --> level --+--> normalize --> soft clip --> Q31 out
   +--> V4 phase --> waveform --> level --+
        ^            ^
        |            |
     detune       SHAPE morph
     interval
     drift
     chaos
```

The NTS-1 host filter/envelope/effects remain downstream and are not reimplemented inside SPECTRA.

## 4. Parameter model

### 4.1 Performance controls

#### SHAPE — Wave Morph

Continuous morph:

```text
0.00      0.33       0.66       1.00
SINE ---- TRIANGLE ---- SAW ---- SQUARE
```

The host `shape_lfo` signal is added to the base SHAPE value so the built-in NTS-1 LFO can animate SPECTRA timbre.

#### ALT — Harmonic Amount

`ALT` continuously moves the secondary voices from unison toward the selected `HarmMode` interval constellation.

At ALT = 0:

```text
all active voices = unison swarm
```

At ALT = 1:

```text
voices reach the full interval constellation
```

This makes interval movement a performance control instead of a static patch choice.

### 4.2 User OSC edit parameters

| Parameter | Manifest range | Internal meaning |
|---|---:|---|
| `Voices` | 0–3 typeless | display 1–4; active voices = value + 1 |
| `Spread` | 0–100% | static detune width, max target ±20 cents |
| `Drift` | 0–100% | slow independent pitch deviation, max target about ±8 cents |
| `HarmMode` | 0–7 typeless | selects one of eight interval constellations |
| `Motion` | 0–100% | scales drift rates |
| `Chaos` | 0–100% | per-note phase, tuning and level mutation |

## 5. Interval constellations

The most important additional interval is placed on voice 2 so two-voice operation is already useful:

```text
1 UNISON :  0,  0,   0,   0
2 FIFTH  :  0, +7, +12, -12
3 OCTAVE :  0,+12, -12, +24
4 MAJOR  :  0, +7,  +4, +12
5 MINOR  :  0, +7,  +3, +12
6 SUS    :  0, +7,  +5, +12
7 QUARTAL:  0, +5, +10, +15
8 CLUSTER:  0, +1,  +7, +13
```

`ALT` scales these interval distances continuously from zero to the listed values.

Example with MAJOR selected:

```text
ALT 0.00: 0,   0, 0,  0
ALT 0.50: 0,+3.5,+2,+6
ALT 1.00: 0,  +7,+4,+12
```

Frequency ratios are recalculated when `ALT` or `HarmMode` changes, not inside every sample calculation.

## 6. Detune and drift

### Static spread

Spread coefficients depend on the active voice count so one voice remains exactly centered and multi-voice modes remain symmetrical:

```text
1 voice :  0
2 voices: -1, +1
3 voices: -1,  0, +1
4 voices: -1, -0.333, +0.333, +1
```

`Spread` scales these toward a maximum of approximately ±20 cents.

### Drift

Each voice has its own low-rate oscillator:

```text
V1 ~0.11 Hz
V2 ~0.17 Hz
V3 ~0.23 Hz
V4 ~0.31 Hz
```

`Motion` scales the rates while `Drift` scales their pitch depth.

The drift LFOs must not share phase or rate. Correlated motion would collapse the concept back toward ordinary vibrato.

## 7. Chaos

Chaos is sampled primarily on NOTE ON, not regenerated continuously at audio rate.

For each active voice it can perturb:

- phase start
- fine tuning
- amplitude

Bounds for v0.1:

```text
extra tuning: roughly ±4 cents max
level variation: roughly ±12% max
phase: deterministic zero → increasingly random
```

At Chaos = 0, repeated notes should be reproducible.

At Chaos = 100, repeated notes should differ while remaining musically bounded.

## 8. Waveform engine

The first engine morphs four waveform families:

1. sine via `osc_sinf()`
2. triangle generated algebraically
3. band-limited saw via SDK lookup
4. band-limited square via SDK lookup

Band-limit table selection follows played pitch plus the active interval contribution.

The output of all active voices is normalized before conversion to Q31. A final light soft-clip stage may catch peaks but must not be used to hide chronic gain errors.

## 9. Stereo constraint

SPECTRA does **not** contain independent stereo placement.

The original NTS-1 oscillator API writes one sample per frame, so all four voices are summed into a mono oscillator signal before the host voice path.

Stereo is intentionally delegated to downstream processing:

```text
SPECTRA --> NTS-1 filter/envelope --> PARALLAX / modulation / delay / reverb --> stereo out
```

This separation keeps the oscillator API honest and makes PARALLAX a natural companion effect.

## 10. Failure behavior

- invalid `Voices` value → clamp to 1–4
- invalid `HarmMode` → clamp to known mode
- extreme pitch increment → clamp below unstable/Nyquist-adjacent range
- summed voice level → normalize by active voice weights
- invalid parameter callback → ignore safely
- NOTE OFF → no internal free-running envelope; host articulation remains authoritative

No parameter state should produce NaN/Inf output.

## 11. Milestones

### M0 — Toolchain
- build Korg `nutekt-digital/dummy-osc`
- package `.ntkdigunit`
- load it on the physical NTS-1

### M1 — Stable four-voice source
- 1–4 voices
- SHAPE morph
- static Spread
- interval modes
- ALT harmonic amount
- bounded output

### M2 — Independent drift
- per-voice LFOs
- Drift + Motion controls
- host shape-LFO interaction
- verify no correlated pitch motion

### M3 — Chaos
- note-on phase mutation
- note-on tuning mutation
- note-on amplitude mutation
- deterministic zero state

### M4 — Musical tuning
- tune interval modes
- tune spread/drift limits
- tune normalization
- inspect aliasing on high notes

### M5 — SPECTRA + PARALLAX integration
- run SPECTRA into PARALLAX
- identify mono-to-stereo cancellation issues
- define recommended constellations

## 12. v0.1 acceptance criteria

1. Builds as a valid original NTS-1 `osc` unit.
2. Loads on firmware compatible with SDK 1.1-0.
3. Tracks incoming NTS-1 pitch accurately.
4. `Voices` audibly changes density from 1 to 4.
5. `Spread = 0` produces centered tuning.
6. `SHAPE` morphs without clicks or large gain jumps.
7. `ALT = 0` collapses all intervals to unison.
8. `ALT = 100%` reaches the selected interval constellation.
9. Drift voices do not move in lockstep.
10. Chaos = 0 is repeatable; Chaos > 0 varies notes within bounds.
11. High-register notes do not produce unacceptable aliasing.
12. Continuous operation is stable for at least 30 minutes.

## 13. Non-goals for v0.1

- independent stereo placement inside the oscillator
- internal filter clone
- internal ADSR clone
- polyphony beyond what the NTS-1 host voice path provides
- sample playback
- spectral FFT/resynthesis
- formant preservation

## 14. Repository layout

```text
oscillators/spectra/
├── README.md
├── SPEC.md
├── docs/
│   ├── dsp-architecture.md
│   └── test-plan.md
└── nts1/
    ├── manifest.json
    ├── project.mk
    └── src/
        └── spectra.cpp
```
