# PARALLAX — NTS-1 Spatial Chorus Specification

**Project:** PARALLAX  
**Target:** Korg Nu:Tekt NTS-1 digital kit (MkI)  
**Module type:** `delfx`  
**SDK target:** logue SDK API `1.1-0`  
**Status:** Draft v0.1

## 1. Product idea

PARALLAX turns one input into a small spatial ensemble whose wet voices occupy different pitches, times, motions, and stereo locations.

The dry source stays recognizable. The wet field should sound like several related performers rather than one conventional chorus sweep.

```text
                 ┌─ Voice A: delay / pitch / motion / pan ─┐
INPUT ───────────┼─ Voice B: delay / pitch / motion / pan ─┼─► WET SUM
   │             ├─ Voice C: delay / pitch / motion / pan ─┤
   │             └─ Voice D: delay / pitch / motion / pan ─┘
   │                                                         │
   └──────────────────────────── DRY ─────────────────────────┴─► OUT
```

## 2. Defining behavior

1. Four wet voices for v0.x.
2. Every voice has an independent base delay time.
3. Every voice has an independent pitch target.
4. Every voice has its own modulation rate and phase.
5. Every voice has its own stereo anchor.
6. Modulation must be deliberately decorrelated; the voices must not breathe as one LFO.
7. The dry path must survive all internal failures.
8. The effect must move continuously from **tight chorus** through **ensemble** into **audible spatial cluster**.

## 3. Verified SDK basis

The official Korg logue SDK provides a `dummy-delfx` template for the Nu:Tekt NTS-1 digital kit.

For SDK API `1.1-0`, Korg documents NTS-1 firmware `>= 1.02` as the compatibility requirement.

The delay effect API exposes:

```c
void DELFX_INIT(uint32_t platform, uint32_t api);
void DELFX_PROCESS(float *xn, uint32_t frames);
void DELFX_PARAM(uint8_t index, int32_t value);
```

`DELFX_PROCESS()` operates in place on stereo audio frames and must support buffers up to 64 frames.

The delay effect parameter IDs are:

```text
TIME
DEPTH
SHIFT_DEPTH
```

Korg's current unit index also demonstrates that the NTS-1 ecosystem already uses both modulation and delay slots for chorus, unison, pitch-shifting and ensemble-delay designs. This confirms the effect category is not restricted to literal echo behavior.

## 4. Why PARALLAX starts as `delfx`

A chorus can live in `modfx`, but this design is primarily a **multi-tap time field** with later independent pitch shifters.

Reasons to start in `delfx`:

- several long-lived delay taps are central to the design
- the NTS-1 delay API exposes SDRAM-backed buffers through `__sdram`
- the ecosystem includes tempo-synced ensemble delay and pitch/unison units in `delfx`
- the architecture can later be reused by CHORDGHOST

If hardware profiling shows a modulation-slot implementation would materially improve CPU or routing behavior, that becomes a later port rather than a v0.1 dependency.

## 5. Voice model

Each voice is described by:

```cpp
struct Voice {
  float base_delay_ms;
  float target_cents;
  float mod_depth_ms;
  float mod_rate_hz;
  float phase;
  float pan;
  float gain;
};
```

`pan` uses:

```text
-1.0 = hard left
 0.0 = center
+1.0 = hard right
```

### Default constellation

Initial musical target:

| Voice | Pitch target | Base time | Motion | Stereo anchor |
|---|---:|---:|---:|---:|
| A | -9 cents | 8 ms | slow | L75 |
| B | -3 cents | 14 ms | medium-slow | L20 |
| C | +4 cents | 23 ms | slowest | R25 |
| D | +10 cents | 36 ms | medium | R78 |

These are design targets, not frozen release values. Hardware listening tests decide the final constellation.

## 6. Important distinction: chorus modulation vs true detuning

A moving delay read head causes pitch modulation, but it does not create a voice held at a stable `-9 cents` or `+10 cents`.

Therefore PARALLAX has two separate mechanisms:

### Motion

Small time modulation around a voice's base delay:

```text
read_delay = base_delay + LFO * modulation_depth
```

This creates organic chorus movement.

### Pitch

A true time-domain pitch shifter using independently moving/crossfaded read heads or grains.

This creates stable relative pitch offsets.

M1 implements the first mechanism only. M2 introduces the second.

## 7. Control model

The NTS-1 MkI gives the delay unit three effect parameters, so PARALLAX uses macro controls.

### TIME — SPREAD

Controls separation between voice arrival times.

Low SPREAD:

```text
~8 / 11 / 14 / 18 ms
```

High SPREAD:

```text
~8 / 22 / 38 / 55+ ms
```

The shortest voice should remain chorus-like while the longest can cross into audible doubling.

### DEPTH — DIVERGENCE

A macro that increases how independent the wet voices feel.

As DIVERGENCE rises:

- motion depth increases
- modulation rates diverge farther
- stereo anchors move wider
- later, pitch offsets expand from near-unison toward their full constellation values

DIVERGENCE must not simply multiply one shared LFO.

### SHIFT_DEPTH — MIX

Conventional dry/wet balance.

The release build should preserve useful dry level through most of the range; exact curve is a hardware-tuning decision.

## 8. Stereo architecture

The voices are not merely hard-panned copies.

Each voice has:

1. a static stereo anchor
2. optional slow local movement around that anchor
3. a bounded pan range so voices do not repeatedly cross the entire image

Example long-term behavior:

```text
A: L80 ↔ L55
B: L30 ↔ R05
C: R15 ↔ R45
D: R60 ↔ R90
```

This produces independent spatial drift rather than a single left/right sweep.

For M1, the anchor is fixed and decorrelated time modulation supplies motion. Slow pan drift is deferred until the basic field is proven.

## 9. Modulation architecture

v0.1 uses four independent phases and non-identical rates.

Initial rate targets:

```text
A: 0.17 Hz
B: 0.23 Hz
C: 0.11 Hz
D: 0.31 Hz
```

All rates remain deliberately slow.

Later experiments may replace one oscillator with smoothed pseudo-random modulation, but deterministic LFOs come first because they are easier to debug and profile.

## 10. Pitch-shifter architecture

### M2 target

A lightweight time-domain shifter, initially tested on **one wet voice**.

Preferred design:

- two moving read heads / grains
- fixed window length
- crossfade windows 180° out of phase
- linear interpolation first
- semitone/cents ratio from lookup or precomputed coefficient

The shifter must be tested independently before four simultaneous instances are enabled.

### CPU escalation plan

```text
1 shifted voice
      ↓
2 shifted voices
      ↓
4 shifted voices
```

If four independent shifters exceed safe CPU headroom, fallback options are:

- two true shifted voices + two conventional chorus voices
- shared shifter pairs
- reduced interpolation/window complexity

The musical identity matters more than forcing an arbitrary voice count.

## 11. M1 implementation

M1 is a four-tap spatial ensemble:

```text
stereo input
    ↓
stereo circular buffer
    ↓
4 fractionally-read delay taps
    ↓
independent low-rate modulation
    ↓
per-voice stereo pan
    ↓
normalized wet sum
    ↓
dry/wet output
```

There is no feedback in M1. This keeps the first field stable and makes the individual voices easy to hear.

## 12. Failure and safety behavior

- invalid parameter state → retain last valid state
- read position outside the ring → wrap safely
- non-finite or runaway internal value → dry signal must remain valid
- excessive wet summation → normalize/soft-limit wet field
- mix at zero → effectively dry bypass
- suspend/resume → no stale blast from uninitialized memory

No failure should mute or corrupt the dry path.

## 13. Milestones

### M0 — Toolchain
- prove official `dummy-delfx` build/package/load on NTS-1

### M1 — Spatial field
- four delay taps
- fractional reads
- independent LFO phase/rate
- fixed stereo anchors
- SPREAD/DIVERGENCE/MIX macros
- no true pitch shifter

### M2 — One true pitch voice
- dual-head pitch shift
- cents control
- artifact and CPU testing

### M3 — Multi-pitch field
- two, then four shifted voices
- independent target cents
- preserve separate timings and pans

### M4 — Constellation tuning
- tune default microtonal ensemble
- compare 2/3/4 voice variants
- add optional interval constellation if CPU permits

### M5 — Spatial motion
- slow bounded pan drift
- optional smoothed-random motion on one voice
- verify mono compatibility

### M6 — Reusable engine
- separate multi-head pitch code into reusable component for CHORDGHOST

## 14. Acceptance criteria for first useful PARALLAX build

1. It builds as a valid `.ntkdigunit` for original NTS-1.
2. Four wet voices have clearly different arrival times.
3. The four voices do not modulate in lockstep.
4. The stereo field remains stable and intentional at maximum DIVERGENCE.
5. SPREAD moves smoothly from tight chorus to audible ensemble/doubling.
6. MIX behaves predictably from dry to wet.
7. A sustained synth note does not produce clicks at modulation extrema.
8. The dry signal remains intact if wet processing misbehaves.
9. Continuous playback is stable for at least 30 minutes.
10. Before release, at least two wet voices must demonstrate genuine fixed pitch offsets rather than only vibrato-style delay modulation.

## 15. Future constellations

### STRING

```text
-7c / -3c / +4c / +8c
```

Tight and lush.

### ORBIT

```text
-12c / +7c / -5c / +11c
```

More unstable and separated.

### FIFTHS

```text
0 st / +7 st / +12 st / -5 st
```

Turns the effect toward spatial harmonizer territory.

### BROKEN CHOIR

```text
-17c / +4 st / +7 st / +12 st
```

Intentionally synthetic and dislocated.

These are deferred until the microtonal core is stable.

## 16. References

- Korg logue SDK: https://github.com/korginc/logue-sdk
- NTS-1 MkI platform: https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital
- Official delay template: https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital/dummy-delfx
- Korg unit index — modulation FX: https://korginc.github.io/logue-sdk/unit-index/modfx/
- Korg unit index — delay FX: https://korginc.github.io/logue-sdk/unit-index/delfx/
