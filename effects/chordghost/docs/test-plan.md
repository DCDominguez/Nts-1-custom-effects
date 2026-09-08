# CHORDGHOST Test Plan

The project is intentionally staged so each subsystem can be proven independently on the physical NTS-1.

## M0 — Toolchain

Pass when:

- Korg `logue-sdk` clones successfully
- submodules initialize
- official `nutekt-digital/dummy-delfx` builds
- `make install` generates an `.ntkdigunit`
- the unit loads into the physical NTS-1

Do not debug CHORDGHOST DSP before the stock template passes.

## M1 — Plain synced delay

### Test signal

Use a short, percussive monophonic synth sound.

### Checks

- dry signal remains audible at minimum mix
- wet signal appears as mix is increased
- feedback does not run away
- all six delay divisions can be selected
- changing TIME does not create hard clicks or unstable jumps
- tempo changes track the NTS-1 BPM
- stereo input does not swap or collapse unexpectedly
- effect remains stable for at least 10 minutes

### Boundary tests

Test at:

- lowest practical BPM
- highest practical BPM
- TIME = minimum
- TIME = maximum
- MIX = 0
- MIX = 0.5
- MIX = 1

## M2 — OXI chord protocol

Before adding pitch shifting, verify the control channel.

### Exhaustive CC test

Send CC31 values:

```text
0, 1, 2 ... 127
```

The decoded value must be monotonic and map back to the intended 7-bit code without skipped or duplicated chord states at the important boundaries.

Specifically inspect:

```text
11/12
23/24
35/36
47/48
59/60
71/72
83/84
95/96
107/108
119/120
120/121
121/122
```

### Progression test

Sequence:

```text
Cmaj7 | Am7 | Fmaj7 | G7
24    | 45  | 29    | 55
```

The internal chord state must update exactly at each harmonic change without interrupting the audio delay.

## M3 — Fixed interval shifter

Before pitch detection, force known shifts:

```text
0
+3
+4
+7
+12
-3
-4
-7
-12 semitones
```

Check:

- CPU stability
- click-free grain/window transitions
- acceptable artifacts on sine, saw, square, and filtered synth tones
- no runaway gain when shifted audio re-enters feedback

## M4 — Pitch detector

Input one note at a time across the intended range.

Test sources:

- sine
- triangle
- saw
- square
- MiniBrute bass patch
- brighter lead patch

Record:

- detected note
- time to stable detection
- octave errors
- false changes during decay
- behavior under vibrato

The detector should prefer holding the last stable pitch over jumping to a low-confidence estimate.

## M5 — Voice leading

Progression:

```text
Cmaj7 | Am7 | Fmaj7 | G7
```

For each source note, verify every wet target belongs to the active chord mask.

Then verify the defining CHORDGHOST behavior:

1. play a note under chord A
2. allow several repeats
3. change to chord B while the tail is active
4. confirm later repeats move into legal tones of chord B

### Avoid-unison test

When a dry note is already a legal chord tone, CHORDGHOST should prefer a different legal tone when the resulting leap is musically reasonable.

## M6 — Stress test

- maximum feedback supported by the release build
- maximum delay time
- rapid chord changes
- rapid TIME changes
- rapid MIX changes
- repeated reserved CC values
- silence followed by sudden transients
- at least 30 minutes continuous operation

Any failure must preserve the dry path.
