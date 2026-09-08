# NTS-1 Custom Effects

Custom DSP experiments for the **Korg Nu:Tekt NTS-1 digital kit (MkI)** using Korg's logue SDK.

The goal is not to recreate conventional pedal effects. This repository is for effects that behave more like compositional tools: effects that understand timing, harmony, gesture, or musical state.

## Effects

### PARALLAX

**Status:** specification + M1 spatial-field scaffold

A four-voice spatial chorus / swarm doubler. Each wet voice can occupy a different time, pitch, modulation trajectory, and stereo position.

```text
INPUT
  ├─ voice A: pitch A / time A / pan A
  ├─ voice B: pitch B / time B / pan B
  ├─ voice C: pitch C / time C / pan C
  └─ voice D: pitch D / time D / pan D
                     ↓
                 stereo sum
                     ↓
                    OUT
```

M1 already implements four decorrelated modulated delay taps with independent stereo anchors. True fixed pitch offsets are intentionally deferred to M2 so the spatial field can be proven first.

See [`effects/parallax/SPEC.md`](effects/parallax/SPEC.md).

### CHORDGHOST

**Status:** specification + scaffold, paused after initial architecture

A harmonic delay whose **dry signal remains unchanged while the wet repeats follow the current chord progression**. The first controller target is the OXI One MKII, which sends compact chord-state data to the NTS-1 over MIDI CC.

```text
dry input ───────────────────────────────► dry out
    │
    └► delay ► harmonic retune ► feedback ► wet out
                    ▲
                    │
              current chord
               from OXI
```

The defining behavior is that echoes follow the chord that is active **now**, even if the echo was created under an earlier chord. A decaying delay tail can therefore behave like a moving harmony voice.

See [`effects/chordghost/SPEC.md`](effects/chordghost/SPEC.md).

## Target platform

- Korg Nu:Tekt NTS-1 digital kit, original/MkI
- logue SDK API `1.1-0`
- NTS-1 firmware `>= 1.02`
- current effect target: `delfx`

The official SDK template is `platform/nutekt-digital/dummy-delfx` in Korg's [`logue-sdk`](https://github.com/korginc/logue-sdk).

## Development principle

Build effects in small hardware-testable layers. Do not hide several unproven DSP systems inside one milestone.

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
└── effects/
    ├── parallax/
    │   ├── README.md
    │   ├── SPEC.md
    │   ├── docs/
    │   │   ├── dsp-architecture.md
    │   │   └── test-plan.md
    │   └── nts1/
    │       ├── manifest.json
    │       ├── project.mk
    │       └── src/parallax.cpp
    └── chordghost/
        ├── README.md
        ├── SPEC.md
        ├── docs/
        │   ├── midi-protocol.md
        │   └── test-plan.md
        └── nts1/
            ├── manifest.json
            ├── project.mk
            └── src/
                ├── chordghost.cpp
                └── chord_table.h
```

The NTS-1 folders are **overlay/scaffolds** for Korg's official `dummy-delfx` project rather than vendored copies of the SDK and toolchain.

## References

- [Korg logue SDK](https://github.com/korginc/logue-sdk)
- [Nu:Tekt NTS-1 SDK platform](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital)
- [Official NTS-1 delay template](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital/dummy-delfx)
- [Korg modulation effect unit index](https://korginc.github.io/logue-sdk/unit-index/modfx/)
- [Korg delay effect unit index](https://korginc.github.io/logue-sdk/unit-index/delfx/)

## License

Original project code is intended for BSD-3-Clause licensing. Any Korg-derived template files retain their original Korg copyright and license notices.
