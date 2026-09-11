# Full 29-unit A-class suite — 2026-09-11

## Status

**HOST PASS / BUILD PASS — physical MkI and musical validation remain separate.**

This report records the first consolidated Human Soon original-NTS-1 MkI candidate for which all 29 tracked units pass both the shared production-DSP common gate and a project-specific A-class behavioral harness, while the exact candidate also freshly compiles/packages for the original NTS-1 logue SDK target.

## Candidate and CI evidence

- development branch: `field-0.1-0-test`
- branch head under test: `5718d12416a242137892688e240e8ac41be5a0ca`
- PR #10 merge commit checked out by the workflow: `f1f27fcd9367d1ade6fe9d29c45a16fbfc0a04de`
- workflow: **Pre-handoff suite**
- workflow run: `34613566166`
- environment: Ubuntu 22.04 GitHub-hosted runner

Results:

- shared production-DSP common gate: **29/29 PASS**
- project-specific A-class harnesses: **29/29 PASS**
- fresh ARM compile/package: **29/29 PASS**
- sanitizer failures in exercised host paths: **none**

Artifacts:

- `pre-handoff-arm-builds`
  - artifact ID `10269557698`
  - SHA-256 `05c2871b84ce242a8864344d9fd59d96674364ea8404ba0f6fb3d16a37fff4e4`
  - contains 29 fresh `.ntkdigunit` files plus `ARM_BUILD_RESULTS.md`
- `pre-handoff-suite-results`
  - artifact ID `10270192374`
  - SHA-256 `e79fd20261950a832b5d7d7a95726c67b2c3ae8b47020b0be17a2ad0f25bc042`
  - contains shared and project-specific host result reports

The ARM build job cloned the current official Korg logue SDK at CI runtime and built the repository's original-MkI `nutekt-digital` targets. Build success is not physical hardware validation.

## Final project-specific PASS set

The consolidated A-class runner passed:

1. SPECTRA
2. PARALLAX
3. CHORDGHOST
4. DUST
5. CARRIER
6. VECTORFILTER
7. IRONROT
8. ATTRACTOR
9. ZEROCROSS
10. PHASEWELL
11. ASCENDER
12. HELIX
13. CAPSTAN
14. SIDEBAND
15. FAULTLINE
16. BALLISTIC
17. RAINFALL
18. SWARMDELAY
19. GLITCHREPEAT
20. BUCKETLINE
21. LONGMEMORY
22. SHARD
23. ABYSS
24. AUREOLE
25. NEBULA
26. LATTICE CORE
27. LATTICE ECHO
28. LATTICE SPACE
29. LATTICE FIELD

FIELD's existing deep capture/delivery suite is included in that command and additionally confirmed dry identity/stereo, all 32 mode/division lifecycles, wet audibility/rest, sustained-source rearm, deadline/clock-wrap behavior, dense replacement, 32 busy mode/division cases, audible synthetic C-D-E first responses, invalid-state containment and ordinary-input headroom with zero guard hits in the recorded test.

## Production defects found by the A-class campaign

The important outcome was not merely turning tests green. Deep lifecycle/behavioral testing exposed production-state defects that the earlier compile/common-gate workflow did not prove.

During the wider A-class campaign, initialization/lifecycle state was corrected in units including ATTRACTOR, CAPSTAN and FAULTLINE.

The final pass found and fixed additional issues:

### RAINFALL

Fresh initialization reset buffers/counters/RNG but did not fully restore all delay targets, pan state and smoothed controls. This made the supposedly seeded stochastic trajectory depend on prior instance history.

Fix: restore the complete deterministic initial state during `DELFX_INIT` / suspend/resume reset. Current candidate manifest: **0.1-1**.

### GLITCHREPEAT

Two findings were separated:

1. production reset did not restore all loop/control-smoothed state;
2. the first BPM-boundary test was itself invalid because its silent pre-roll advanced the beat counter before measurement.

Fix: restore complete repeat/control state in production reset; rewrite the tempo test so its measurement begins from a true fresh beat counter; add fresh-init reproducibility coverage. Current candidate manifest: **0.1-1**.

### NEBULA

The granular reset restored history/RNG/phases but not every grain-delay and smoothed control value, so a fresh initialization could inherit state from the prior instance.

Fix: restore deterministic grain delays and controls on reset. Current candidate manifest: **0.1-2**.

Tests were not weakened to hide these production lifecycle issues.

## Historical/backburner units

PARALLAX, IRONROT, ZEROCROSS and NEBULA now have A-class engineering harnesses even though they remain backburnered for **musical/design reasons**. A technical A grade does not reactivate them or claim their current musical identity is approved.

LATTICE ECHO and LATTICE SPACE also receive A-class engineering coverage so the public historical corpus remains reproducible, even though the preferred MkI LATTICE system is CORE + FIELD + optional built-in Korg reverb.

## Known hardware-only/open items

A-class does not supersede existing physical evidence.

### LATTICE CORE

The physical original MkI previously distorted around the user's approximate 3 o'clock-and-higher TIME region, and lowering source level did not remove it. The new host A-class harness passes, but that **does not clear the hardware failure**. CORE remains **physical FAIL / RETEST** at high TIME until a changed or revalidated hardware candidate passes.

### CORE + FIELD

The supported CORE + FIELD pairing remains **physical FAIL / RETEST** based on the most recent physical integration test. The 29/29 host result does not prove simultaneous real-time deadline margin on the MkI.

### FIELD

FIELD 0.1-2 has a reported standalone listening PASS and is the current sound-quality reference. Combination validation remains open as documented above.

### All units

Still unproven by desktop A-class alone:

- exact `.ntkdigunit` load/select on the physical MkI for this candidate;
- real-time CPU/callback deadline margin;
- analog I/O behavior;
- arbitrary simultaneous processor combinations;
- subjective tone, musicality, identity and playability.

## Handoff boundary after this milestone

Normal physical testing should now be intentionally short and high-value:

1. load/select each candidate on the original NTS-1 MkI;
2. perform the one identified family/unit worst-case runtime sanity check;
3. judge tone, musicality, identity and playability.

Ordinary deterministic logic, buffer/state, expiry, control-contract and bounded-output faults should be reproduced internally before handoff wherever representable off-device.

## Community reproduction

From the repository root:

```bash
python3 tests/pre-handoff/run_suite.py
python3 tests/pre-handoff/run_unit_specific.py
```

Expected candidate result:

```text
29/29 common-gate PASS
29/29 project-specific A-class PASS
```

The GitHub Actions **Pre-handoff suite** adds the fresh ARM compile/package layer.
