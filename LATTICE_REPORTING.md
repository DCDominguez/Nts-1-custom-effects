# Human Soon LATTICE — Update Reporting Protocol

**Target:** original Korg Nu:Tekt NTS-1 digital kit MkI  
**Branch:** `lattice-suite`

This document defines the mandatory reporting process for every meaningful LATTICE development step.

The goal is simple: **future work must be able to reconstruct not only what changed, but what was intended, what was tested, what physically happened, and what remains uncertain.**

## Mandatory rule

Whenever LATTICE moves forward in a meaningful way, add a repository update report in `reports/lattice/` in the same development cycle.

A meaningful step includes:

- architecture changes;
- DSP implementation changes;
- parameter/control changes;
- gain/headroom changes;
- sound-quality or voicing changes;
- CPU/memory optimizations;
- new builds or version bumps;
- CI/build-system changes;
- physical MkI test results;
- failed experiments or reverted approaches;
- changes to hardware assumptions or SDK interpretation;
- milestone decisions.

Small typo-only documentation edits do not require a report.

## File naming

Use:

`reports/lattice/YYYY-MM-DD_<short-topic>.md`

Examples:

- `reports/lattice/2026-09-10_field-architecture-pivot.md`
- `reports/lattice/2026-09-11_field-0.1-0-build.md`
- `reports/lattice/2026-09-11_field-0.1-0-hardware-test.md`

If several reports are created on the same date, keep distinct short topics rather than overwriting earlier reports.

## Required report contents

Every report should contain the following sections when applicable.

### 1. Status

- date;
- branch;
- relevant version/build;
- commit SHA(s);
- CI/workflow run if applicable;
- physical hardware validation status.

### 2. Why this change happened

Record the problem, hardware observation, sound-design goal, or engineering constraint that caused the work.

Do not rewrite history after the fact. If the reason was uncertain at the time, say so.

### 3. What changed

Record the actual implementation or design changes, including:

- architecture;
- voice/event counts;
- control mappings;
- gain structure;
- buffers/memory layout;
- feedback values;
- scheduling behavior;
- pitch/rhythm/stereo behavior;
- guardrails and failure containment;
- relevant filenames.

Use concrete values where they are known.

### 4. Sound-quality intent

State what the build is supposed to sound like, especially:

- foreground clarity;
- wet audibility;
- transient preservation;
- stereo image;
- BODY/HALO balance;
- pitch quality;
- reverb/bloom density;
- intended decay/lifetime;
- behaviors that are explicitly unwanted.

This section is mandatory for DSP/voicing changes.

### 5. Runtime and headroom intent

State the expected CPU/memory/headroom consequences.

If they are estimates rather than measurements, label them as estimates.

Peak guards/limiters are containment, not proof that gain structure is correct.

### 6. Build/CI result

Record what actually passed in CI and distinguish it from physical hardware validation.

Never write that a unit “works on hardware” solely because it compiled or packaged successfully.

### 7. Physical MkI result

When hardware testing occurs, record only what the tester actually reports.

Useful observations include:

- whether the unit loaded;
- which controls/settings were exercised;
- whether the source remained clear;
- whether echoes/bloom were clearly audible;
- clipping, crunch, dropout, stuck audio, runaway tails, or collapse;
- interaction with CORE/FIELD or built-in effects;
- whether the issue recovered when controls were reduced.

Do not invent exact knob values, telemetry, CPU percentages, or signal levels that were not observed.

### 8. Result classification

Use one of:

- **PASS** — intended behavior physically established;
- **PASS WITH NOTES** — usable but with known limitations;
- **FAIL / RETEST** — identity, stability, or sound-quality gate not met;
- **BUILD ONLY** — compiled/packaged but not yet physically tested;
- **DESIGN ONLY** — architecture/specification change, no build yet.

### 9. What we learned

Record the engineering or sound-design lesson that should survive even if the implementation is later replaced.

### 10. Next step

State the next concrete action and any gates that must remain open.

## Relationship to other project documents

- `LATTICE_SYSTEM.md` = current architecture and doctrine.
- `LATTICE_HISTORY.md` = long-form narrative history and why the project evolved.
- `LATTICE_FIELD_SPEC.md` = current FIELD implementation specification.
- `LATTICE_REPORTING.md` = reporting rules.
- `reports/lattice/` = chronological development and hardware-test record.
- Issue #9 = raw/open physical MkI QA discussion.

When a report establishes a durable architectural lesson, update `LATTICE_HISTORY.md` as well. When it changes the current design target, update `LATTICE_SYSTEM.md` and/or `LATTICE_FIELD_SPEC.md` in the same development cycle.

## Truth hierarchy

When documents disagree, prefer evidence in this order:

1. explicit physical MkI observation;
2. current source code and manifest for implemented behavior;
3. current official Korg documentation/API;
4. current LATTICE spec/system docs;
5. design intent or inference;
6. older reports/history.

Do not silently turn an inference into a hardware fact.
