# Human Soon NTS-1 Backburner

Target: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**

This file records experiments that remain preserved in the repository but are **not active development targets**.

## 2026-09-09 — retired / backburnered

The following four effects are retired from the active M2–M4 roadmap:

| Effect | Slot | Last useful state | Reason for backburner |
|---|---|---|---|
| **PARALLAX** | `delfx` | M2 test builds | The intended idea is a delay whose trails move through stereo position and time. Current implementations read too strongly as chorus/ensemble because the moving stereo taps and short-time behavior dominate the perception. |
| **IRONROT** | `modfx` | M2 test builds | Current nonlinear architecture does not establish a sufficiently distinctive effect identity; on hardware it was perceived mainly as tone/gain change rather than a compelling corrosion process. |
| **ZEROCROSS** | `modfx` | M1 hardware-validated | The through-zero implementation works, but its audible identity remains too close to a conventional flanger to justify further development in the current suite. |
| **NEBULA** | `revfx` | M1 hardware-validated; 0.1-1 corrective test build exists | The current architecture does not yet communicate an audible granular-cloud identity strongly enough and was also reported to clip in the earlier build. It does not currently feel like the intended particle-to-reverb concept. |

## What backburner means

- Keep source, docs, binaries, CI history and hardware-validation history.
- Do **not** delete the projects.
- Do **not** advance them to later milestones while they are backburnered.
- Do **not** treat unvalidated corrective/test builds as release candidates.
- The frozen M1 hardware-validation record remains historically valid for the exact M1 sources that were tested.
- Any future redesign starts a new hardware QA gate.

## Design lesson

Future Human Soon effects should begin from a **strong audible behavior or interaction model**, not from an abstract DSP category alone.

A preferred approach is to study the high-level interaction idea of a familiar effect system, reduce that idea to something the original NTS-1 MkI can realistically execute, and then build an independent Human Soon interpretation around it.

Commercial products may be used only as **high-level functional references**. The project will not copy proprietary source code, firmware, presets, UI text, algorithms disclosed only through reverse engineering, or trademarked branding.

Example direction under consideration: study the broad workflow and musical behavior that makes the Hologram Electronics Microcosm compelling, identify a small core interaction that can fit the NTS-1 MkI, then design an original clean-room effect around that core rather than attempting a clone.

## Resume gate

A backburnered project should only return to active development if we can state, before coding:

1. the audible behavior in one sentence;
2. why it is meaningfully different from the rest of the Human Soon suite;
3. which NTS-1 controls map naturally to that behavior;
4. what the minimum viable MkI implementation is; and
5. what a listener should hear within the first few seconds that proves the identity works.
