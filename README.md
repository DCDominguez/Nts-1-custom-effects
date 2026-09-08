# NTS-1 Custom Effects & Oscillators

Custom DSP experiments for the **Korg Nu:Tekt NTS-1 digital kit (MkI)** using Korg's logue SDK.

The goal is not to recreate conventional pedals or stock synth voices. These projects treat the NTS-1 as a programmable musical system: oscillators and effects that understand pitch relationships, motion, timing, harmony, or controlled instability.

## Oscillators

### SPECTRA

**Status:** specification + first oscillator scaffold

A four-voice swarm oscillator. One incoming pitch becomes an internal ensemble with waveform morphing, detune spread, interval constellations, independent low-rate drift, and bounded per-note mutation.

```text
host pitch
   |
   +-- voice 1: root / detune / drift
   +-- voice 2: interval / detune / drift
   +-- voice 3: interval / detune / drift
   +-- voice 4: interval / detune / drift
             |
          mono sum
             |
       NTS-1 voice path
```

The original NTS-1 user-oscillator API outputs one sample per frame, so SPECTRA is intentionally mono before the host filter/envelope/effects. Independent stereo placement is delegated downstream.

See [`oscillators/spectra/SPEC.md`](oscillators/spectra/SPEC.md).

## Effects

### PARALLAX

**Status:** specification + M1 spatial-field scaffold

A four-voice spatial chorus / swarm doubler. Each wet voice can occupy a different time, pitch, modulation trajectory, and stereo position.

```text
INPUT
  +-- voice A: pitch A / time A / pan A
  +-- voice B: pitch B / time B / pan B
  +-- voice C: pitch C / time C / pan C
  +-- voice D: pitch D / time D / pan D
                     |
                 stereo sum
                     |
                    OUT
```

M1 implements four decorrelated modulated delay taps with independent stereo anchors. True fixed pitch offsets are intentionally deferred until the spatial field is proven on hardware.

See [`effects/parallax/SPEC.md`](effects/parallax/SPEC.md).

### CHORDGHOST

**Status:** specification + scaffold, paused after initial architecture

A harmonic delay whose dry signal remains unchanged while wet repeats follow the current chord progression. The first controller target is the OXI One MKII.

```text
dry input -------------------------------> dry out
    |
    +--> delay --> harmonic retune --> feedback --> wet out
                       ^
                       |
                 current chord
```

See [`effects/chordghost/SPEC.md`](effects/chordghost/SPEC.md).

## Target platform

- Korg Nu:Tekt NTS-1 digital kit, original/MkI
- logue SDK API `1.1-0`
- NTS-1 firmware `>= 1.02`
- current unit types: `osc` and `delfx`

Official templates:

- `platform/nutekt-digital/dummy-osc`
- `platform/nutekt-digital/dummy-delfx`

in Korg's [`logue-sdk`](https://github.com/korginc/logue-sdk).

## Development principle

Build each project in small hardware-testable layers. Do not hide several unproven DSP systems inside one milestone.

For SPECTRA:

1. build/load official `dummy-osc`
2. stable 1–4 voice source + SHAPE morph
3. Spread + interval constellations + ALT morph
4. independent Drift/Motion
5. bounded per-note Chaos
6. high-note alias/CPU tuning
7. SPECTRA + PARALLAX integration

For PARALLAX:

1. four-voice modulated spatial field
2. one true pitch-shift voice
3. two, then four pitch-shift voices if CPU permits
4. tune the default constellation
5. add slow bounded spatial drift
6. extract the pitch engine for reuse in CHORDGHOST

For CHORDGHOST:

1. plain BPM-synced delay
2. OXI chord-state decoding
3. fixed semitone pitch shifting
4. monophonic pitch detection
5. chord-tone selection and voice leading
6. hardware profiling and musical tuning

## Repository layout

```text
Nts-1-custom-effects/
├── README.md
├── LICENSE
├── oscillators/
│   └── spectra/
│       ├── README.md
│       ├── SPEC.md
│       ├── docs/
│       │   ├── dsp-architecture.md
│       │   └── test-plan.md
│       └── nts1/
│           ├── manifest.json
│           ├── project.mk
│           └── src/spectra.cpp
└── effects/
    ├── parallax/
    └── chordghost/
```

The NTS-1 project folders are **overlay/scaffolds** for Korg's official templates rather than vendored copies of the complete SDK/toolchain.

## References

- [Korg logue SDK](https://github.com/korginc/logue-sdk)
- [Nu:Tekt NTS-1 SDK platform](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital)
- [Official NTS-1 oscillator template](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital/dummy-osc)
- [Official NTS-1 delay template](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital/dummy-delfx)
- [Korg oscillator unit index](https://korginc.github.io/logue-sdk/unit-index/osc/)

## License

Original project code is intended for BSD-3-Clause licensing. Any Korg-derived template files retain their original Korg copyright and license notices.
