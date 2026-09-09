# NTS-1 Custom Effects & Oscillators

Custom DSP experiments for the **Korg Nu:Tekt NTS-1 digital kit (MkI)** using Korg's logue SDK.

The goal is not to recreate conventional pedals or stock synth voices. These projects treat the NTS-1 as a programmable musical system: oscillators and effects that understand pitch relationships, motion, timing, harmony, or controlled instability.

## Oscillators

### SPECTRA

**Status:** compiled / ready for physical MkI QA

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

**Status:** compiled M1 / ready for physical MkI QA

A four-voice spatial chorus / swarm doubler. Each wet voice can occupy a different time, pitch, modulation trajectory, and stereo position.

M1 implements four decorrelated modulated delay taps with independent stereo anchors. True fixed pitch offsets are intentionally deferred until the spatial field is proven on hardware.

See [`effects/parallax/SPEC.md`](effects/parallax/SPEC.md).

### CHORDGHOST

**Status:** compiled M1 / ready for physical MkI QA; feature development paused after initial architecture

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

### DUST

**Status:** compiled M1 / ready for physical MkI QA

A clean-room sample-rate and bit-depth reduction design. TIME controls RATE; DEPTH controls DAMAGE. Strong settings introduce a bounded stereo clock fracture so left and right alias structures can decorrelate.

See [`effects/dust/README.md`](effects/dust/README.md).

### CARRIER

**Status:** compiled M1 / ready for physical MkI QA

A clean-room amplitude/ring-modulation design. TIME controls FREQUENCY. DEPTH is a POLARITY macro that traverses dry -> unipolar AM -> bipolar ring modulation. Strong ring settings introduce a bounded stereo carrier-phase split.

See [`effects/carrier/README.md`](effects/carrier/README.md).

### Human Soon Effects Suite

DUST and CARRIER are the first units in a wider clean-room effects roadmap that studies broad DSP categories present in the Korg logue ecosystem while implementing original Human Soon algorithms, mappings, and behaviors.

See [`effects/humansoon-suite/ROADMAP.md`](effects/humansoon-suite/ROADMAP.md).

## Hardware testing

Compilation is not hardware validation. Every unit marked ready for test has a project-specific QA sheet.

See [`TESTING.md`](TESTING.md).

## Target platform

- Korg Nu:Tekt NTS-1 digital kit, original/MkI
- logue SDK API `1.1-0`
- NTS-1 firmware `>= 1.02`
- current unit types: `osc`, `modfx`, and `delfx`

Official templates used as build bases:

- `platform/nutekt-digital/dummy-osc`
- `platform/nutekt-digital/dummy-modfx`
- `platform/nutekt-digital/dummy-delfx`

in Korg's [`logue-sdk`](https://github.com/korginc/logue-sdk).

## Development principle

Build each project in small hardware-testable layers. Do not hide several unproven DSP systems inside one milestone.

Current rule set:

1. implement one bounded milestone
2. compile against Korg's current template in GitHub Actions
3. package `.ntkdigunit`
4. add the unit and QA sheet to the hardware test queue
5. test on the physical original NTS-1
6. only then expand the DSP architecture

## Repository layout

```text
Nts-1-custom-effects/
├── README.md
├── TESTING.md
├── LICENSE
├── oscillators/
│   └── spectra/
└── effects/
    ├── parallax/
    ├── chordghost/
    ├── dust/
    ├── carrier/
    └── humansoon-suite/
        └── ROADMAP.md
```

The NTS-1 project folders are **overlay/scaffolds** for Korg's official templates rather than vendored copies of the complete SDK/toolchain.

## References

- [Korg logue SDK](https://github.com/korginc/logue-sdk)
- [Nu:Tekt NTS-1 SDK platform](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital)
- [Official NTS-1 oscillator template](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital/dummy-osc)
- [Official NTS-1 modulation FX template](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital/dummy-modfx)
- [Official NTS-1 delay template](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital/dummy-delfx)
- [Korg unit index](https://korginc.github.io/logue-sdk/unit-index/)

## License

Original project code is intended for BSD-3-Clause licensing. Any Korg-derived template files retain their original Korg copyright and license notices.
