# CARRIER Test Plan

CARRIER must be validated on the physical original Korg Nu:Tekt NTS-1 digital kit before feature expansion.

## M0 — load / pass-through

Test with a sustained sine or triangle source first.

Pass when:

- unit loads into a modulation FX user slot
- selecting the effect does not lock or blast
- `TIME = 0`, `DEPTH = 0` is effectively dry
- left/right orientation remains correct
- silence remains silent

## M1 — FREQUENCY / TIME

Use `DEPTH = 50%` first so the carrier is easy to hear as AM/tremolo.

Test TIME at:

```text
0%
5%
10%
20%
35%
50%
65%
80%
100%
```

Listen for:

- smooth sub-audio tremolo at the low end
- continuous acceleration as TIME rises
- transition into audio-rate sidebands
- no obvious phase resets during normal knob motion
- no hard clicks or stuck carrier

The M1 source assumes 48 kHz for its frequency mapping. Record approximate measured or perceived carrier frequencies if the hardware behavior suggests this assumption is wrong.

## M1 — POLARITY / DEPTH

With TIME around a clearly audible low-to-mid rate, test:

```text
DEPTH 0%   = dry
DEPTH 25%  = partial AM
DEPTH 50%  = full unipolar AM
DEPTH 75%  = AM/ring transition
DEPTH 100% = full bipolar ring modulation
```

Pass when:

- the path is continuous with no abrupt mode switch
- 50% does not invert the signal
- 100% crosses through zero and produces classic ring-mod sidebands
- output stays bounded

## M2 — sideband split

Use mono material fed equally to L/R.

At `DEPTH <= 50%`:

- image should remain essentially centered

At `DEPTH > 50%`:

- stereo width should increase progressively
- neither channel should disappear
- image must not flip uncontrollably

Collapse externally to mono and verify:

- no severe recurring cancellation
- full ring remains usable
- level loss is acceptable and stable

## Source matrix

Repeat representative settings using:

- sine
- triangle
- saw
- square
- short pluck
- bass-heavy material
- full stereo material through AUDIO IN

High-frequency saw/square sources are the priority aliasing test at high carrier rates.

## Abuse / recovery

- rapid TIME min/max sweep for 30 seconds
- rapid DEPTH min/max sweep for 30 seconds
- alternate both controls rapidly
- silence for 30 seconds, then transient input
- switch away from CARRIER and back
- verify normal operation resumes without reset or blast

## Stress test

Worst case:

```text
TIME = 100%
DEPTH = 100%
bright input material
```

Run:

- 10 minutes
- 30 minutes

Pass when:

- NTS-1 remains responsive
- no NaN/Inf-like blast
- no permanent mute
- no runaway level
- no audible state corruption after effect switching

## Release decision

Do not add oversampling or additional carrier modes merely because high-frequency aliasing exists. First document where the artifacts become objectionable and whether a lower maximum carrier frequency solves the problem more efficiently on MkI hardware.
