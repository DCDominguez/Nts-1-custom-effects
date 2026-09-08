# CHORDGHOST — NTS-1 Harmonic Delay Specification

**Project:** CHORDGHOST  
**Target:** Korg Nu:Tekt NTS-1 digital kit (MkI)  
**Module type:** `delfx`  
**SDK target:** logue SDK API `1.1-0`  
**Status:** Draft v0.1  
**Primary controller:** OXI One MKII

## 1. Product idea

CHORDGHOST is a custom delay whose **wet repeats follow an externally supplied chord progression while the dry signal remains unchanged**.

```text
dry input ───────────────────────────────► dry out
    │
    └► delay ► harmonic retune ► feedback ► wet out
                    ▲
                    │
              current chord
               from OXI
```

The effect should feel like a harmony voice living inside the delay line rather than a normal delay with a pitch shifter after it.

The important behavior is temporal: an echo created under chord A can still be alive when chord B arrives, and subsequent repeats should be pulled into chord B.

## 2. Core behavior

1. The **dry signal is never reharmonized**.
2. Only the **wet/feedback path** is pitch-shifted.
3. The active harmony may change while echoes are decaying.
4. Existing echoes follow the **current** chord, not necessarily the chord present when the source note was played.
5. v0.x targets **monophonic melodic input**.
6. Polyphonic pitch recognition and polyphonic pitch shifting are out of scope for v0.x.

## 3. Verified NTS-1 / SDK constraints

The official Korg logue SDK provides `platform/nutekt-digital/dummy-delfx` as the delay-effect template for the original NTS-1.

For SDK API `1.1-0`, NTS-1 firmware `>= 1.02` is required.

The delay API exposes:

```c
void DELFX_INIT(uint32_t platform, uint32_t api);
void DELFX_PROCESS(float *xn, uint32_t frames);
void DELFX_PARAM(uint8_t index, int32_t value);
```

`DELFX_PROCESS()` processes interleaved stereo audio in place and must support buffers up to 64 frames.

The user delay API exposes:

```text
TIME
DEPTH
SHIFT_DEPTH
```

Existing NTS-1 delay implementations conventionally convert the `DELFX_PARAM()` fixed-point value with `q31_to_f32(value)` to obtain a normalized 0..1 control value.

The effects runtime also exposes:

```c
fx_get_bpm();
fx_get_bpmf();
```

so CHORDGHOST can derive tempo-synced delay time internally.

The NTS-1 MIDI implementation exposes:

```text
CC30 = Delay Time
CC31 = Delay Depth
CC33 = Delay Mix
```

## 4. Control model

### NTS-1 parameters

| NTS-1 parameter | CHORDGHOST v0.1 role |
|---|---|
| TIME | rhythmic division / delay timing |
| DEPTH | externally supplied chord-state code |
| SHIFT_DEPTH | dry/wet mix |

Feedback begins as a fixed internal constant so the first prototype has only one harmonic control variable.

### OXI → NTS-1 chord protocol

OXI sends chord state using **CC31 / Delay Depth**.

CHORDGHOST must not assume the raw `DELFX_PARAM()` integer equals the MIDI CC value. The callback value is treated as a normalized parameter value:

```cpp
const float normalized = q31_to_f32(value);   // expected 0..1
const int chord_code = round(normalized * 127.0f);
```

The exact edge behavior of MIDI CC → NTS-1 parameter → `DELFX_PARAM()` must be confirmed on hardware in milestone M2. Decoding must clamp to `0..127`.

This keeps the protocol compatible with OXI's ordinary 7-bit MIDI CC lanes while respecting the SDK callback representation.

## 5. Chord encoding

Formula:

```text
chord_code = quality_index * 12 + root_pitch_class
```

Pitch classes:

```text
C=0  C#=1  D=2  D#=3  E=4  F=5
F#=6 G=7   G#=8 A=9   A#=10 B=11
```

| Index | Quality | Code range |
|---:|---|---:|
| 0 | Major | 0–11 |
| 1 | Minor | 12–23 |
| 2 | Maj7 | 24–35 |
| 3 | Min7 | 36–47 |
| 4 | Dominant 7 | 48–59 |
| 5 | Sus2 | 60–71 |
| 6 | Sus4 | 72–83 |
| 7 | Diminished | 84–95 |
| 8 | Augmented | 96–107 |
| 9 | Min7b5 | 108–119 |

Special codes:

```text
120 = harmonic bypass / chromatic repeat
121 = hold previous chord
122–127 = reserved
```

Examples:

```text
Cmaj7 = 24
Fmaj7 = 29
Am7   = 45
G7    = 55
```

Each valid chord resolves to a 12-bit pitch-class mask.

Example:

```text
Cmaj7 = C E G B
mask  = bits {0,4,7,11}
```

## 6. Harmonic engine

### Pitch detection

v0.1 targets single-note synth lines, guitar-like sources, and other monophonic material.

Proposed detector pipeline:

1. sum stereo input to mono
2. pre-filter / remove problematic DC or excessive highs
3. maintain a rolling analysis buffer
4. estimate fundamental at a lower control rate than the audio loop
5. convert frequency to nearest MIDI note / pitch class
6. reject low-confidence estimates
7. hold the last stable pitch briefly through note decay

Implementation priority is **stability over responsiveness** because the pitch decision affects delayed audio rather than the dry attack.

If confidence is poor, retain the previous stable pitch instead of producing a random target.

### Default voice-leading mode

**VOICE LEAD / AVOID UNISON**

Inputs:

```text
detected source pitch
current chord mask
previous wet target
```

Rules:

1. Generate legal chord tones around the detected note.
2. Prefer the candidate closest to the previous wet target.
3. Avoid unison with the dry note when another reasonable chord tone exists.
4. Limit the initial transposition range to ±12 semitones.
5. Penalize large leaps.

Conceptual score:

```text
score =
    abs(candidate - previous_target)
  + leap_penalty
  + unison_penalty
```

Lowest score wins.

## 7. Delay engine

### Timing

Delay timing is derived from `TIME` plus `fx_get_bpmf()`.

Initial division table:

```text
1/16
1/8
1/8 dotted
1/4
1/4 dotted
1/2
```

Delay-time transitions should be smoothed to avoid clicks and abrupt Doppler jumps unless a later mode intentionally uses them.

### Feedback path

Initial feedback target:

```text
~0.55–0.65
```

Final value is tuned on hardware.

```text
delay read
  ↓
harmonic pitch shift
  ↓
gentle damping
  ↓
soft limiter
  ↓
feedback write
```

The feedback path must remain bounded under all chord and pitch-detection states.

### Stereo

v0.1 may begin with centered or dual-mono harmonic repeats if necessary for CPU stability. Stereo widening and ping-pong behavior are secondary to harmonic correctness.

## 8. Pitch shifter

The first shifter should prioritize predictable CPU cost and musical usefulness over transparent studio-quality transposition.

Candidate architecture:

- two modulated delay read heads
- overlapping windows / crossfade
- pitch ratio selected from a semitone lookup table

Required v0.1 range:

```text
-12 … +12 semitones
```

Avoid `pow()` inside the sample loop; use a precomputed ratio table.

## 9. Failure behavior

CHORDGHOST should fail musically.

If chord data stops arriving:
- hold the last valid chord

If pitch confidence collapses:
- hold the last stable pitch briefly

If pitch remains unknown:
- bypass harmonic shift while keeping the delay alive

If a reserved/invalid chord code arrives:
- preserve the previous valid chord

If feedback approaches instability:
- clamp / soft-limit the feedback state

No invalid state may mute or corrupt the dry path.

## 10. MVP milestones

### M0 — Toolchain
- clone Korg `logue-sdk`
- initialize submodules
- build official `nutekt-digital/dummy-delfx`
- package `.ntkdigunit`
- confirm load on the physical NTS-1

### M1 — Plain synced delay
- SDRAM delay buffer
- BPM-aware timing
- wet/dry control
- bounded feedback
- no pitch shifting

### M2 — OXI chord protocol
- receive chord state through CC31 / DEPTH
- convert normalized callback value back to `0..127`
- decode chord lookup table
- hardware-test all boundary values
- confirm chord changes do not interrupt audio

### M3 — Fixed pitch-shift feedback
- add semitone pitch shifter
- manually force intervals
- validate artifact character and CPU headroom

### M4 — Monophonic pitch detector
- stable pitch estimate
- confidence gate
- note-change hysteresis
- test synth waveforms and MiniBrute material

### M5 — Voice-leading CHORDGHOST
- detector + chord state + target selector
- active echoes follow new chord changes
- avoid-unison behavior
- hardware tuning

### M6 — Performance pass
- profile CPU
- remove unnecessary per-sample math
- verify 64-frame processing
- stress-test feedback stability
- long-session hardware test

## 11. v0.1 acceptance criteria

1. Builds as a valid `.ntkdigunit`.
2. Loads on an original NTS-1 with compatible firmware.
3. OXI CC31 reliably changes active chord state.
4. Dry monophonic input remains unmodified.
5. Wet repeats resolve to a legal tone of the active chord.
6. Changing chords during a tail changes subsequent repeat harmony.
7. Pitch-detection failure does not produce wild interval jumps.
8. Feedback remains stable at maximum supported delay time.
9. Invalid control states never damage the dry path.
10. Real-time CPU load remains reliable on hardware.

## 12. Non-goals for v0.1

- polyphonic chord recognition from incoming audio
- polyphonic pitch shifting
- automatic access to OXI's internal chord data
- custom UI beyond existing NTS-1 controls
- pristine formant-preserving transposition
- reverb
- multiple simultaneous harmony voices

## 13. Future modes

- **UPPER** — nearest legal chord tone above
- **LOWER** — nearest legal chord tone below
- **NEAREST** — nearest legal chord tone, unison allowed
- **VOICE LEAD** — smoothest moving harmony voice
- **ORBIT** — cycle through chord members on successive repeats
- **ARPEGHOST** — each repeat advances through current chord tones
- **CASCADE** — feedback voice becomes the source for the next harmonic decision
- **DEAD CHANNEL** — controlled corruption / skipped repeats
- **EVENT HORIZON** — dynamics-aware harmonic bloom

## 14. Build order

```text
plain synced delay
      ↓
CC chord decoding
      ↓
fixed semitone pitch shift
      ↓
pitch detection
      ↓
voice leading
```

Do not collapse these stages. A working hardware artifact at each stage is more valuable than a large untestable DSP implementation.

## 15. References

- https://github.com/korginc/logue-sdk
- https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital
- https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital/dummy-delfx
