# NTS-1 Custom Effects

Custom DSP experiments for the **Korg Nu:Tekt NTS-1 digital kit (MkI)** using Korg's logue SDK.

The goal is not to recreate conventional pedal effects. This repository is for effects that behave more like compositional tools: effects that understand timing, harmony, gesture, or musical state.

## Effects

### CHORDGHOST

**Status:** specification + scaffold

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
- effect type: `delfx`

The official SDK template is `platform/nutekt-digital/dummy-delfx` in Korg's [`logue-sdk`](https://github.com/korginc/logue-sdk).

## CHORDGHOST development order

1. plain BPM-synced delay
2. OXI chord-state decoding
3. fixed semitone pitch shifting
4. monophonic pitch detection
5. chord-tone selection and voice leading
6. hardware profiling and musical tuning

This deliberately separates delay, control protocol, pitch shifting, pitch detection, and harmonic logic so failures are diagnosable.

## Repository layout

```text
Nts-1-custom-effects/
├── README.md
└── effects/
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

The NTS-1 folder is an **overlay/scaffold** for Korg's official `dummy-delfx` project rather than a vendored copy of the SDK and toolchain.

## References

- [Korg logue SDK](https://github.com/korginc/logue-sdk)
- [Nu:Tekt NTS-1 SDK platform](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital)
- [Official NTS-1 delay template](https://github.com/korginc/logue-sdk/tree/main/platform/nutekt-digital/dummy-delfx)

## License

Original project code is intended for BSD-3-Clause licensing. Any Korg-derived template files retain their original Korg copyright and license notices.
