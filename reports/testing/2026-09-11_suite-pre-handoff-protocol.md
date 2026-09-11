# Suite-wide pre-handoff testing protocol adopted — 2026-09-11

## Decision

DC wants the FIELD debugging/testing model applied across the full Human Soon NTS-1 suite so physical handoff is used mostly for:

1. confirming the candidate loads/selects on the original NTS-1 MkI;
2. one short device-specific runtime sanity sweep;
3. tone, musicality, identity and performance feel.

The developer/CI side should own internally reproducible functional defects before handoff.

## Why this changed

FIELD exposed a failure mode that compilation and simple state/activity checks did not catch: captures could occur without the intended musical responses reaching output during real playing. The successful debugging approach used deterministic production-DSP tests, state ownership/expiry checks, delivery assertions, mode/division coverage, wrap checks and hardware feedback reconciliation.

That workflow is now the suite standard rather than a FIELD-only exception.

## Repository changes

Added:

- `PRE_HANDOFF_PROTOCOL.md` — common and unit-type-specific test requirements plus the new hardware handoff contract;
- `PRE_HANDOFF_COVERAGE.md` — per-unit coverage/backfill matrix.

Updated:

- `TESTING.md` — separates the internal engineering gate from the physical/music gate;
- `README.md` — makes the protocol part of the repository development rule.

## Coverage reality at adoption

FIELD is the first unit classified at deep production-DSP pre-handoff coverage. CHORDGHOST and several M2 units have partial test plans/protocol coverage; many historical M1 units still have compile/package + project QA but no equivalent production-DSP host harness.

This protocol does not retroactively invalidate historical hardware passes. It changes the required engineering process for future changed candidates.

## Backfill priority

1. LATTICE CORE because high-TIME hardware distortion is currently open;
2. active M2/M3 candidates;
3. remaining active M1 effects;
4. backburner units only if deliberately reopened.

## Platform boundary

Current official Korg material still lists the original NTS-1 API reference at `1.1-0` and user oscillator/modulation/delay/reverb units loaded through the NTS-1 Sound Librarian. There is no cycle-accurate desktop MkI runtime in this project, so actual user-unit loading and real-time CPU/deadline margin remain hardware-only checks.

## Status

**PROCESS CHANGE: ADOPTED.**

No DSP source or audible behavior was changed by this reporting/protocol commit series.
