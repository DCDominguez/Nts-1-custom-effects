# DUST v0.1 Specification

## Purpose

DUST is a deliberately musical digital-degradation processor for the original NTS-1 MkI. It combines sample-rate reduction and bit-depth reduction, then adds a bounded stereo-clock divergence at extreme settings.

The design is independent. Public commercial effects are treated only as broad functional references; no proprietary source, binary, preset, or parameter design is copied.

## Platform

- Device: Korg Nu:Tekt NTS-1 digital kit MkI
- SDK platform: `nutekt-digital`
- Module: `modfx`
- API: `1.1-0`
- Firmware requirement: NTS-1 firmware >= 1.02 for SDK 1.1-0 units

## Controls

### TIME -> RATE

Normalized range 0..1.

The effective hold length is:

```text
hold_samples = 1 + round(RATE^2 * 127)
```

This produces a useful fine-control region near full-bandwidth operation while still reaching heavy reduction at the top of the control.

At the assumed 48 kHz NTS-1 processing rate, 1 sample is full rate and 128 samples is approximately 375 Hz effective update rate. Hardware validation remains authoritative.

### DEPTH -> DAMAGE

Normalized range 0..1.

DAMAGE controls both wet proportion and quantizer depth:

```text
bits = 16 - round(DAMAGE * 12)
```

Range: approximately 16-bit down to 4-bit.

```text
output = dry * (1 - DAMAGE) + degraded * DAMAGE
```

This makes DEPTH=0 a practical bypass state and DEPTH=1 the strongest degradation state.

## Stereo fracture

At high RATE and DAMAGE the right channel receives a slightly different hold length from the left channel:

```text
fracture = max(0, 2*DAMAGE - 1) * RATE
right_hold = left_hold + round(fracture * 7)
```

The offset is always bounded and never affects the dry path. The purpose is controlled alias decorrelation rather than random stereo modulation.

## DSP order

```text
stereo input
   -> independent sample/hold clocks
   -> signed uniform quantizer
   -> DAMAGE dry/wet crossfade
   -> output clamp
```

No dither or synthetic noise is added in v0.1, so digital silence remains silent.

## Parameter smoothing

RATE and DAMAGE targets are smoothed in the audio loop before they affect hold timing, quantizer resolution, and wet mix. This is required to reduce control zippering during knob sweeps.

## Safety

- no feedback paths
- no generated noise at digital silence
- hard finite output clamp to [-1, 1]
- integer hold lengths clamped to safe ranges
- quantizer depth clamped to 4..16 bits

## Milestones

### M0 — build

- compile against current official Korg `dummy-modfx` template
- package `.ntkdigunit`
- inspect text/data/BSS sizes

### M1 — basic reducer

- RATE reduction works smoothly
- DAMAGE reduces quantizer resolution
- DEPTH=0 behaves as dry
- silence remains silent
- stereo channels remain correctly oriented

### M2 — stereo fracture tuning

- mono source develops useful but bounded stereo difference only at strong settings
- no obvious level pumping
- mono collapse remains acceptable

### M3 — optional character extensions

Only after hardware validation consider:

- selectable quantization laws
- deterministic error shaping
- transient-sensitive reduction
- envelope-controlled degradation

Do not add these before M1/M2 pass on physical hardware.
