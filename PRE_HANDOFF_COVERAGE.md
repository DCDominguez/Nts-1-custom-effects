# Human Soon NTS-1 — Pre-Handoff Test Coverage

This matrix tracks reproducible **engineering coverage** for the original Korg Nu:Tekt NTS-1 digital kit MkI. It is deliberately separate from physical loading/runtime and subjective musical validation.

## Full A-class suite — 2026-09-11

All **29 tracked units** satisfy the repository's A-class pre-handoff engineering gate for the MkI target.

Final consolidated evidence:

- branch candidate: `field-0.1-0-test` @ `5718d12416a242137892688e240e8ac41be5a0ca`;
- PR #10 merge-under-test: `f1f27fcd9367d1ade6fe9d29c45a16fbfc0a04de`;
- GitHub Actions workflow: **Pre-handoff suite**, run `34613566166`;
- shared production-DSP common gate: **29/29 PASS**;
- project-specific A-class harnesses: **29/29 PASS**;
- fresh ARM compile/package: **29/29 PASS**;
- AddressSanitizer/UndefinedBehaviorSanitizer: no failure in the exercised host paths;
- fresh ARM artifact: `10269557698`, SHA-256 `05c2871b84ce242a8864344d9fd59d96674364ea8404ba0f6fb3d16a37fff4e4`;
- host result artifact: `10270192374`, SHA-256 `e79fd20261950a832b5d7d7a95726c67b2c3ae8b47020b0be17a2ad0f25bc042`.

See `reports/testing/2026-09-11_full-a-class-suite.md`.

## Latest exact-candidate physical pass — 2026-09-12

The A-class release candidate was loaded and played on the physical original NTS-1 MkI.

Tester rule: units not explicitly called out in the physical notes **loaded properly and sounded as intended**.

Physical findings do not change the A engineering grade, but they do change the separate hardware/music status:

- SPECTRA: sounds great through 3 voices; the 4-voice setting degrades the sound. MkI release line is now capped at 3 voices in 0.2-1; exact new binary requires load retest.
- Delay family: functional/musical PASS with family-wide low perceived output level.
- Reverb family except LATTICE SPACE: functional/musical PASS with family-wide low perceived output level.
- ASCENDER / ATTRACTOR / CAPSTAN: functional PASS; effect is too subtle and needs musical audibility tuning.
- IRONROT: sounds good but can become too loud.
- LATTICE CORE: sounds good, but intermittent distortion remains at maximum/extreme TIME and in some multi-effect use.
- LATTICE SPACE: loads/functions, but is very subtle at full MIX and distorts when run with modulation; redesign/revoice candidate.
- All other units: LOAD / RUNTIME / MUSICAL PASS for the reported playing test.

See `reports/testing/2026-09-12_a-class-rc-physical-mki.md`.

## What A means here

**A = deep pre-handoff engineering coverage.** The actual production DSP is exercised through deterministic project-specific host tests, shared safety/state tests and a fresh ARM build/package gate.

A does **not** mean:

- real-time MkI CPU/deadline margin is fully proven;
- combinations with every other active processor are proven;
- tone or musicality has passed every user/environment;
- later DSP changes inherit the same physical result.

Those remain separate hardware/music statuses.

## Current matrix

| Unit | Type | Engineering coverage | Latest physical/music note |
|---|---|---:|---|
| SPECTRA | osc | **A** | prior RC: 1–3 voices sound great; 4 voices musical FAIL. 0.2-1 caps at 3 and needs exact-build load retest |
| PARALLAX | delfx | **A** | latest RC: load/runtime/music PASS; musically backburnered by design history |
| CHORDGHOST | delfx | **A** | latest RC: load/runtime/music PASS; delay family level retune pending |
| DUST | modfx | **A** | latest RC: load/runtime/music PASS |
| CARRIER | modfx | **A** | latest RC: load/runtime/music PASS |
| VECTORFILTER | modfx | **A** | latest RC: load/runtime/music PASS |
| IRONROT | modfx | **A** | sounds great; can become too loud; retune level |
| ATTRACTOR | modfx | **A** | functional PASS; barely noticeable; increase audible identity |
| ZEROCROSS | modfx | **A** | latest RC: load/runtime/music PASS; musically backburnered by design history |
| PHASEWELL | modfx | **A** | latest RC: load/runtime/music PASS |
| ASCENDER | modfx | **A** | functional PASS; barely noticeable; increase audible identity |
| HELIX | modfx | **A** | latest RC: load/runtime/music PASS |
| CAPSTAN | modfx | **A** | functional PASS; barely noticeable; increase audible identity |
| SIDEBAND | modfx | **A** | latest RC: load/runtime/music PASS |
| FAULTLINE | modfx | **A** | latest RC: load/runtime/music PASS |
| BALLISTIC | delfx | **A** | latest RC: functional/music PASS; delay family level retune pending |
| RAINFALL | delfx | **A** | latest RC: functional/music PASS; delay family level retune pending |
| SWARMDELAY | delfx | **A** | latest RC: functional/music PASS; delay family level retune pending |
| GLITCHREPEAT | delfx | **A** | latest RC: functional/music PASS; delay family level retune pending |
| BUCKETLINE | delfx | **A** | latest RC: functional/music PASS; delay family level retune pending |
| LONGMEMORY | delfx | **A** | latest RC: functional/music PASS; delay family level retune pending |
| SHARD | delfx | **A** | latest RC: functional/music PASS; delay family level retune pending |
| ABYSS | revfx | **A** | latest RC: functional/music PASS; reverb family level retune pending |
| AUREOLE | revfx | **A** | latest RC: functional/music PASS; reverb family level retune pending |
| NEBULA | revfx | **A** | latest RC: functional/music PASS; family level retune pending; earlier backburner history retained |
| LATTICE CORE | modfx | **A** | sounds great; intermittent extreme-TIME / combination distortion; PASS WITH NOTES / RETEST |
| LATTICE ECHO | delfx | **A** | latest RC: load/runtime/music PASS; historical architecture |
| LATTICE SPACE | revfx | **A** | load/basic function PASS; too subtle + distortion with modulation; FAIL / REDESIGN CANDIDATE |
| LATTICE FIELD | delfx | **A** | latest RC: load/runtime/music PASS; preferred LATTICE delay stage |

## What the A-class backfill checks

The project-specific layer covers the defining architecture of each unit rather than merely checking that finite audio appears. Depending on the unit this includes pitch/interval behavior, spectral transfer, nonlinear harmonic behavior, stereo geometry, moving delay heads, circular-buffer wraps, stochastic reproducibility, capture ownership, BPM/division behavior, finite phrase expiry, feedback decay, shimmer/halo delivery, reverb rest state, parameter boundaries and architecture-appropriate long soaks.

FIELD's dedicated suite remains the deepest capture/delivery reference and is executed by the consolidated 29-unit project-specific runner.

## New gap exposed by the physical pass

A-class functional coverage did its job, but the latest MkI test exposed a distinct calibration gap: **useful perceived level / standalone effect prominence**.

The next testing layer should add reproducible level/loudness proxy measurements so obvious under-level/over-level candidates are found before handoff. This must supplement, not replace, musical listening judgment.

Tracked in issue #11.

## Physical handoff rule

A normal current-candidate handoff should leave DC/community testers mainly with:

1. **LOAD** — the physical original NTS-1 MkI accepts, selects and produces audio from the unit;
2. **one identified worst-case runtime sanity check** appropriate to that processor family;
3. **MUSICAL** — tone, musicality, identity, movement and playability.

Known physical failures are never erased by an A-class desktop PASS.

## Reproducing the engineering gate

From the repository root on the documented Linux/WSL environment:

```bash
python3 tests/pre-handoff/run_suite.py
python3 tests/pre-handoff/run_unit_specific.py
```

Expected result for the A-class candidate:

```text
29/29 common-gate PASS
29/29 project-specific A-class PASS
```

The GitHub Actions **Pre-handoff suite** additionally performs the fresh ARM build/package gate.
