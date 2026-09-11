# Human Soon NTS-1 — Pre-Handoff Test Coverage

This matrix tracks reproducible **engineering coverage** for the original Korg Nu:Tekt NTS-1 digital kit MkI. It is deliberately separate from physical loading/runtime and subjective musical validation.

## Full A-class suite — 2026-09-11

All **29 tracked units** now satisfy the repository's A-class pre-handoff engineering gate for the MkI target.

Final consolidated evidence:

- branch candidate: `field-0.1-0-test` @ `5718d12416a242137892688e240e8ac41be5a0ca`;
- PR #10 merge-under-test: `f1f27fcd9367d1ade6fe9d29c45a16fbfc0a04de`;
- GitHub Actions workflow: **Pre-handoff suite**, run **34613566166**;
- shared production-DSP common gate: **29/29 PASS**;
- project-specific A-class harnesses: **29/29 PASS**;
- fresh ARM compile/package against the current official Korg logue SDK cloned at CI runtime: **29/29 PASS**;
- AddressSanitizer/UndefinedBehaviorSanitizer: no failure in the exercised host paths;
- fresh ARM artifact: `pre-handoff-arm-builds`, artifact `10269557698`, SHA-256 `05c2871b84ce242a8864344d9fd59d96674364ea8404ba0f6fb3d16a37fff4e4`;
- host result artifact: `pre-handoff-suite-results`, artifact `10270192374`, SHA-256 `e79fd20261950a832b5d7d7a95726c67b2c3ae8b47020b0be17a2ad0f25bc042`.

See `reports/testing/2026-09-11_full-a-class-suite.md`.

## What A means here

**A = deep pre-handoff engineering coverage.** The actual production DSP is exercised through deterministic project-specific host tests, shared safety/state tests and a fresh ARM build/package gate.

A does **not** mean:

- the `.ntkdigunit` has loaded on a physical MkI for this exact candidate;
- real-time MkI CPU/deadline margin is proven;
- combinations with other active processors are proven;
- tone or musicality has passed human listening.

Those remain separate hardware/music statuses. See `A_CLASS_REQUIREMENTS.md` and `COMMUNITY_TESTING.md`.

## Current matrix

| Unit | Type | Engineering coverage | Physical/music note |
|---|---|---:|---|
| SPECTRA | osc | **A** | historical MkI pass exists; exact current candidate still follows normal load/listening handoff |
| PARALLAX | delfx | **A** | backburnered musically; engineering harness retained for community regression testing |
| CHORDGHOST | delfx | **A** | historical MkI pass exists |
| DUST | modfx | **A** | historical MkI pass exists |
| CARRIER | modfx | **A** | historical MkI pass exists |
| VECTORFILTER | modfx | **A** | historical MkI pass exists |
| IRONROT | modfx | **A** | backburnered musically; engineering harness retained |
| ATTRACTOR | modfx | **A** | historical MkI pass exists |
| ZEROCROSS | modfx | **A** | backburnered musically; engineering harness retained |
| PHASEWELL | modfx | **A** | historical MkI pass exists |
| ASCENDER | modfx | **A** | historical MkI pass exists |
| HELIX | modfx | **A** | historical MkI pass exists |
| CAPSTAN | modfx | **A** | historical MkI pass exists |
| SIDEBAND | modfx | **A** | historical MkI pass exists |
| FAULTLINE | modfx | **A** | historical MkI pass exists |
| BALLISTIC | delfx | **A** | historical MkI pass exists |
| RAINFALL | delfx | **A** | lifecycle reset fix in current candidate; exact build needs load/listening check |
| SWARMDELAY | delfx | **A** | historical MkI pass exists |
| GLITCHREPEAT | delfx | **A** | lifecycle reset fix in current candidate; exact build needs load/listening check |
| BUCKETLINE | delfx | **A** | historical MkI pass exists |
| LONGMEMORY | delfx | **A** | historical MkI pass exists |
| SHARD | delfx | **A** | historical MkI pass exists |
| ABYSS | revfx | **A** | historical MkI pass exists |
| AUREOLE | revfx | **A** | historical MkI pass exists |
| NEBULA | revfx | **A** | backburnered musically; lifecycle reset fix in current candidate; prior clipping history remains relevant |
| LATTICE CORE | modfx | **A** | **physical FAIL / RETEST at high TIME**; desktop A-class does not clear this |
| LATTICE ECHO | delfx | **A** | historical architecture; not preferred MkI LATTICE system component |
| LATTICE SPACE | revfx | **A** | historical architecture; not preferred MkI LATTICE system component |
| LATTICE FIELD | delfx | **A** | standalone listening PASS reported; CORE pairing remains unresolved |

## What the A-class backfill actually checks

The project-specific layer now covers the defining architecture of each unit rather than merely checking that finite audio appears. Depending on the unit this includes pitch/interval behavior, spectral transfer, nonlinear harmonic behavior, stereo geometry, moving delay heads, circular-buffer wraps, stochastic reproducibility, capture ownership, BPM/division behavior, finite phrase expiry, feedback decay, shimmer/halo delivery, reverb rest state, parameter boundaries and architecture-appropriate long soaks.

FIELD's dedicated suite remains the deepest capture/delivery reference and is executed by the consolidated 29-unit project-specific runner.

## Physical handoff rule

A normal current-candidate handoff should now leave DC/community testers mainly with:

1. **LOAD** — the physical original NTS-1 MkI accepts, selects and produces audio from the unit;
2. **one identified worst-case runtime sanity check** appropriate to that processor family;
3. **MUSICAL** — tone, musicality, identity, movement and playability.

Known physical failures are never erased by an A-class desktop PASS. In particular, LATTICE CORE's high-TIME distortion remains open until a changed candidate passes on the actual MkI.

## Reproducing the engineering gate

From the repository root on the documented Linux/WSL environment:

```bash
python3 tests/pre-handoff/run_suite.py
python3 tests/pre-handoff/run_unit_specific.py
```

Expected result for this candidate:

```text
29/29 common-gate PASS
29/29 project-specific A-class PASS
```

The GitHub Actions **Pre-handoff suite** additionally performs the fresh ARM build/package gate.
