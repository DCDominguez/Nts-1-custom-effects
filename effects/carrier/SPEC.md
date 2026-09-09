# CARRIER v0.1 Specification

## Goal

Create an original ring/amplitude modulation effect for the original Korg Nu:Tekt NTS-1 digital kit that remains musically useful from slow tremolo through audio-rate sideband generation.

CARRIER is not a recreation of any commercial effect. Public products are used only to identify the broad DSP category.

## Platform contract

Target:

- platform: `nutekt-digital`
- module: `modfx`
- API: `1.1-0`
- host controls: `TIME`, `DEPTH`
- manifest custom parameters: `0`

The Korg ModFX callback processes interleaved stereo frames and supports buffers up to 64 frames.

## Control design

### TIME — FREQUENCY

Normalized TIME controls carrier frequency with a strongly curved response so slow modulation gets most of the knob travel.

M1 nominal mapping assumes a 48 kHz audio rate:

```text
frequency = 0.2 Hz + TIME^5 * 4799.8 Hz
```

The exact audible frequency range must be hardware-checked before release. The 48 kHz constant is an implementation assumption, not a value established by the NTS-1 owner manual.

### DEPTH — POLARITY

POLARITY is deliberately staged instead of being a normal wet/dry control.

```text
0.00 -> dry
0.50 -> full unipolar amplitude modulation
1.00 -> full bipolar ring modulation
```

For `DEPTH <= 0.5`, modulation factor morphs from `1` to `(0.5 + 0.5 * carrier)`.

For `DEPTH > 0.5`, modulation factor morphs from `(0.5 + 0.5 * carrier)` to `carrier`.

This keeps the complete control sweep useful rather than dedicating one of the NTS-1's two ModFX parameters to a separate mode switch.

## Sideband split

Above the AM midpoint, the stereo carrier phases separate gradually:

```text
left  = phase - offset
right = phase + offset
```

Maximum offset is 1/12 cycle per side (30 degrees), producing a 60-degree L/R carrier relationship at full ring modulation.

Design goals:

- mono input should develop stereo width at strong ring settings
- channels must never become 180 degrees apart
- mono sum should remain useful
- low-depth AM/tremolo should remain centered

## Parameter smoothing

TIME and DEPTH targets are smoothed sample-by-sample to reduce zippering and hard transitions. Carrier phase is continuous through normal parameter movement.

## M1 exclusions

Not included yet:

- oversampling
- alternate carrier waveforms
- tempo lock
- envelope tracking
- frequency quantization
- feedback
- Hilbert/Bode frequency shifting

Those are different problems and should not be added until M1 passes physical MkI QA.

## Acceptance gate

M1 passes only when the physical original NTS-1 confirms:

- unit loads and selects reliably
- slow TIME values produce smooth tremolo-rate movement
- higher TIME values produce stable audio-rate sidebands
- DEPTH traverses dry -> AM -> ring continuously
- sideband split produces useful stereo width without severe mono collapse
- rapid parameter motion does not lock or blast
- digital silence remains silent
- 30-minute worst-case operation is stable
