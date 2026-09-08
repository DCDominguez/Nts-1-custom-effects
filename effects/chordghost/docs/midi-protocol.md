# CHORDGHOST MIDI Protocol

## Purpose

OXI One MKII supplies the current harmonic state to CHORDGHOST through the NTS-1's existing **Delay Depth** control path.

No custom SysEx protocol is required for v0.1.

## NTS-1 CC mapping

```text
CC30 = Delay Time
CC31 = Delay Depth
CC33 = Delay Mix
```

CHORDGHOST repurposes these as:

```text
CC30 = rhythmic delay division
CC31 = chord code
CC33 = wet/dry mix
```

## Important SDK detail

`DELFX_PARAM()` receives an SDK parameter representation, not a raw MIDI byte. Existing NTS-1 delay code converts the callback value using:

```cpp
const float normalized = q31_to_f32(value);
```

CHORDGHOST therefore reconstructs a 7-bit protocol value using:

```cpp
int code = static_cast<int>(normalized * 127.0f + 0.5f);
code = clamp(code, 0, 127);
```

The exact CC boundary mapping must be verified on physical NTS-1 hardware before the protocol is frozen.

## Chord-code format

```text
code = quality_index * 12 + root_pitch_class
```

### Pitch classes

| Note | Value |
|---|---:|
| C | 0 |
| C# | 1 |
| D | 2 |
| D# | 3 |
| E | 4 |
| F | 5 |
| F# | 6 |
| G | 7 |
| G# | 8 |
| A | 9 |
| A# | 10 |
| B | 11 |

### Chord qualities

| Index | Quality | Range |
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

### Reserved values

```text
120 = harmonic bypass / chromatic repeat
121 = hold previous chord
122–127 = reserved
```

## Examples

```text
C major  = 0
A minor  = 21
Cmaj7    = 24
Fmaj7    = 29
Am7      = 45
G7       = 55
F#sus4   = 78
```

## OXI sequencing model

The OXI sequence that carries the progression should have a modulation/CC lane whose step changes align with the harmonic rhythm.

Example:

```text
Progression: Cmaj7 | Am7 | Fmaj7 | G7
CC31:          24  |  45 |  29   | 55
```

CHORDGHOST holds the most recently received valid chord state between changes.

## Failure rules

- `121`: keep the last valid chord explicitly.
- `122–127`: ignore and keep the last valid chord.
- no new CC data: keep the last valid chord.
- invalid internal decode: keep the last valid chord.
- `120`: keep the delay running but disable harmonic quantization.

## Protocol-freeze test

Before declaring this protocol stable, send every CC31 value from `0` through `127` from OXI and record which integer CHORDGHOST reconstructs internally. Boundary errors must be solved before pitch shifting is added.
