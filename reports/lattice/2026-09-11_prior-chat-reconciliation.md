# LATTICE prior-chat reconciliation — 2026-09-11

## Purpose

This report audits prior Human Soon Studio conversations against the repository and records development facts that were either only present in chat, only present in narrow per-test reports, or not yet reflected in the durable system summary.

This is a documentation reconciliation only. No DSP source, manifest, binary, control mapping, gain structure, or runtime behavior is changed by this report.

## Repository state found during audit

- Stable/reference branch: `lattice-suite`.
- Active development branch: `field-0.1-0-test`.
- Draft PR #10 targets `lattice-suite` from `field-0.1-0-test`.
- The development branch is materially ahead of `lattice-suite`; this is intentional while FIELD/CORE integration remains under hardware validation.
- `LATTICE_CURRENT_STATUS.md` already contained the strongest current-state summary, but `LATTICE_SYSTEM.md` still contained stale FIELD 0.1-1 / next-milestone wording. That system document was synchronized in the same reconciliation cycle.
- The root `README.md` also did not mention LATTICE at all, despite LATTICE being the active system-development effort. README was updated in the same reconciliation cycle with the current CORE/FIELD status and links to the authoritative LATTICE documents.

## Prior-chat changes verified as already represented somewhere in the repo

### FIELD 0.1-0 implementation

Prior work established a first FIELD implementation on draft PR #10 with:

- two seed/capture ownership model;
- finite scheduled response events;
- shared bloom architecture;
- source-protected scheduling rules;
- dedicated CI and MkI build packaging.

The repo already contains the FIELD source, CI workflow, QA, and 0.1-0 build/CI reports.

### FIELD 0.1-1 hardware problem

Physical testing reported that louder FIELD 0.1-1 echoes did not correctly replenish closely spaced musical input: in the C-D-E test, only the initial C response was reliably heard while later D/E events were missed.

This was already represented in the dedicated C-D-E / note-spacing reports and in draft PR #10's history.

### FIELD 0.1-2 capture revision

The subsequent approved revision changed the seed-admission model so sustained/continuing qualified source material can create CLOCK-spaced captures while protecting the first clear response. Two owned captures remain; busy input may be skipped and lower-priority later phrase events may be retired.

FIELD 0.1-2 is built and packaged; its manifest is `0.1-2`. This is already represented in source, build/CI reports, PR #10, `LATTICE_CURRENT_STATUS.md`, and the FIELD spec amendment.

### FIELD 0.1-2 standalone physical result

Physical MkI report:

> "it's perfect. plays exactly as intended. if i add a mod or a reverb though it becomes distorted."

Repository classification already recorded:

- FIELD 0.1-2 standalone listening behavior: PASS for the reported test;
- combined-effect behavior: FAIL / RETEST until specific pairings are isolated;
- no 30-minute certification inferred.

### Corrosion and Albedo comparison

Physical MkI reports established:

- **Corrosion + FIELD** works cleanly in the tested playing context;
- **Albedo + FIELD** is not an independent simultaneous user REVERB + user DELAY pair;
- turning off Albedo also removed the apparent FIELD delay, consistent with the first-generation user DELAY/user REVERB slot-memory relationship rather than proving a FIELD DSP failure.

This is already recorded in the Sinevibes slot-correction and Corrosion/Albedo reports.

### CORE + FIELD integration failure

Physical testing narrowed the strongest supported-pair distortion to **CORE 0.3-0 + FIELD 0.1-2**. Tested built-in MOD effects did not show the same CORE-specific problem, while Corrosion + FIELD remained clean.

A host-side production-DSP chain probe found substantial guard engagement as CORE TIME increased and confirmed unnecessary ordinary CORE scheduling during settled freeze. The probe did not clip to full scale or prove a hardware CPU cause.

This is already recorded in `2026-09-11_core-field-chain-probe.md`.

### CORE standalone high-TIME failure

Physical testing then established that CORE alone begins distorting around the user's approximate **3 o'clock and higher** TIME region. Lowering input level did **not** remove the distortion.

This weakens a simple input-headroom diagnosis. Current source cost and physical behavior make runtime/workload the leading hypothesis, but there is no measured CPU percentage and dense granular playback behavior remains an alternative contributor.

This is already recorded in the high-TIME reports.

### Proposed CORE 0.3-1 runtime diagnostic

Prior work proposed, but did not implement, a scoped CORE 0.3-1 diagnostic profile:

1. cap ordinary microloop playback at 10 active voices instead of 16;
2. preserve gain, patterns, history, loop rules, and FIELD 0.1-2;
3. suspend ordinary microloop scheduling/processing during full freeze;
4. add automated checks for the new ceiling and freeze work suspension;
5. retest CORE alone at high TIME, then CORE + FIELD.

This remains **pending approval** and must not be described as implemented.

## Gaps found and corrected in this audit

### 1. `LATTICE_SYSTEM.md` was stale

It still said FIELD 0.1-1 was the current candidate and instructed the next step to build/test 0.1-1, despite the repo already containing FIELD 0.1-2 and later hardware results.

Correction: `LATTICE_SYSTEM.md` was updated to:

- make `LATTICE_CURRENT_STATUS.md` the first current-state reference;
- identify FIELD 0.1-2 as the current implementation and standalone sonic reference;
- include the Corrosion/Albedo slot findings;
- include CORE high-TIME failure and low-input persistence;
- include CORE + FIELD FAIL / RETEST status;
- identify the proposed CORE 0.3-1 profile as pending approval, not implemented.

### 2. Root `README.md` did not expose active LATTICE work

The root README described the older 25-unit M1 suite and backburner state correctly, but it had no LATTICE section and therefore gave a stale picture of current development priorities.

Correction: README now includes:

- preferred CORE → FIELD → optional built-in Korg reverb architecture;
- CORE 0.3-0 high-TIME hardware failure;
- FIELD 0.1-2 standalone listening PASS and open validation status;
- Corrosion + FIELD and Albedo + FIELD findings;
- CORE + FIELD FAIL / RETEST status;
- proposed CORE 0.3-1 as design-only / pending approval;
- ordered links to current LATTICE status, system, history, FIELD spec, reporting protocol and reports.

### 3. Durable historical documents still contain older status language

`LATTICE_HISTORY.md` and portions of `LATTICE_FIELD_SPEC.md` were written before the latest FIELD implementation/hardware cycle and contain historical status lines such as FIELD being at specification stage or 0.1-1 candidate status.

These older statements are historical context, not current authority. Until those large historical documents receive a dedicated rewrite, current truth is explicitly ordered as:

1. `LATTICE_CURRENT_STATUS.md`
2. `LATTICE_SYSTEM.md`
3. root `README.md`
4. latest dated reports under `reports/lattice/`
5. historical sections of `LATTICE_HISTORY.md` / `LATTICE_FIELD_SPEC.md`

The top-level 0.1-2 amendments in the FIELD spec remain authoritative over older body text where they conflict.

## Branch / PR note

The latest FIELD development is intentionally not merged into `lattice-suite` yet. Draft PR #10 remains the integration boundary while hardware validation is open. This audit does **not** merge or force-update the stable/reference branch.

## Current authoritative state after reconciliation

- CORE: `0.3-0`; high-TIME physical FAIL / RETEST.
- FIELD: `0.1-2`; standalone listening PASS for reported test; overall validation OPEN.
- Corrosion + FIELD: reported PASS for tested playing context.
- Albedo + FIELD: unsupported user REVERB + user DELAY simultaneous arrangement.
- CORE + FIELD: FAIL / RETEST.
- Proposed CORE `0.3-1` runtime profile: DESIGN ONLY / pending approval.
- No 30-minute CORE + FIELD hardware stability pass exists yet.
- Compile/CI/host tests remain subordinate to physical MkI evidence.
