# SPECTRA 0.2-1 three-voice cap — 2026-09-12

## Source

Physical original-NTS-1 MkI listening feedback from the A-class release-candidate test.

Tester report:

> SPECTRA works perfectly, but at four voices it starts becoming bad. Limit it to three voices. Overall it sounds great.

## Physical interpretation

- 1–3 voices: **MUSICAL PASS** for the reported playing test.
- 4 voices in the prior 0.2-0 architecture: **MUSICAL FAIL / RETUNE**.
- Overall SPECTRA identity/tone: strongly positive physical report.
- This is a musical/hardware constraint, not a desktop numerical-safety failure.

## Decision

SPECTRA is intentionally reduced from a four-voice to a **three-voice maximum** for the MkI release line.

Version: `0.2-1`.

Changes:

1. `kMaxVoices` reduced from 4 to 3.
2. `Voices` manifest range reduced from `0..3` to `0..2`, corresponding to 1–3 internal voices.
3. Runtime parameter handling hard-clamps values above the published range to three voices.
4. Default voice count reduced from four to three.
5. Four-voice-only spread geometry and fourth-voice drift state removed.
6. Interval constellations retain their first three voices; the previous fourth interval is removed.
7. A-class SPECTRA harness updated to assert the hard three-voice ceiling and three-voice musical contracts.

## Engineering intent

The goal is not to recover the fourth voice through attenuation or compromise the sound that already passed listening. The physical MkI result establishes three voices as the preferred density ceiling. The fourth voice is removed so SPECTRA preserves the character the tester described as sounding great.

## Platform basis

Target remains original NTS-1 MkI `osc`, logue SDK API `1.1-0`. Korg's current official API reference still lists the original NTS-1 at v1.1-0.

## Validation state

The source/manifest/test change reopens the exact-build gate.

Required before release:

- shared pre-handoff common gate PASS;
- SPECTRA project-specific A-class harness PASS;
- fresh ARM build/package PASS;
- physical MkI load/select check of 0.2-1;
- confirm Voices 1, 2 and 3 retain the approved tone/musicality.

The prior hardware report is sufficient to reject four voices, but does not automatically validate the new 0.2-1 binary until it is loaded on-device.
