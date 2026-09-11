# Community test release / A-class definition — 2026-09-11

## Reason

The suite-wide common engineering retest reached 29/29 PASS, but only LATTICE FIELD currently has the deeper project-specific deterministic coverage classified as A-level. The next goal is to make the testing system understandable and reproducible by community contributors rather than keeping the process dependent on private conversation context.

## Added documentation

- `A_CLASS_REQUIREMENTS.md`
  - defines the exact engineering gates required for A-class;
  - separates shared common-gate coverage from project-specific behavioral depth;
  - requires control-contract, architecture-specific stress, lifecycle/soak, reproducibility, and known-limitations handoff evidence;
  - explicitly separates engineering coverage from physical/music validation.

- `COMMUNITY_TESTING.md`
  - gives local Linux/WSL and GitHub Actions entry points;
  - explains what the host runner proves and does not prove;
  - documents how contributors can add project-specific tests;
  - defines report labels and physical-testing discipline.

- `community/HARDWARE_TEST_REPORT.md`
  - standardizes original-NTS-1 MkI community reports;
  - records exact candidate, routing, simultaneous MOD/DELAY/REVERB processors, controls, reproduction steps, and literal symptoms;
  - keeps observation separate from inference.

## A-class path

All 29 tracked units already have the shared production-DSP common gate and fresh ARM build/package gate from workflow run `34605006118`.

The major remaining work for suite-wide A-class is project-specific deterministic testing:

- prove each unit's documented musical/control behavior reaches output;
- test architecture-specific edge cases and state lifetime;
- add appropriately justified soak/wrap/decay coverage;
- retain public deterministic fixtures and commands in the repository.

FIELD remains the depth reference.

## Validation semantics

A-class remains a **pre-handoff engineering coverage grade**. It does not claim physical MkI loading, cycle margin, coexistence with other effects, or subjective tone/musicality.

Community hardware reports use separate LOAD / RUNTIME / MUSICAL statuses.

## Production DSP

No production DSP was changed in this documentation/community-test release.
