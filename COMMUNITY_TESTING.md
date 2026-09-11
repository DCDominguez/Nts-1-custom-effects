# Community Testing Guide

This repository welcomes reproducible engineering tests and physical Korg logue SDK hardware reports.

The current production reference is the **original NTS-1 MkI**. Cross-platform work is intentionally gated until the MkI suite is mature; planned targets include minilogue xd, prologue, NTS-1 MkII, NTS-3, microKORG2 and drumlogue. See `PLATFORM_SUPPORT.md`.

The goal is to separate three questions cleanly:

1. **Does the production DSP behave correctly under deterministic host tests?**
2. **Does the candidate build/package correctly for its stated logue SDK target?**
3. **Does it load, run and sound good on that real hardware product?**

These are related, but they are not the same result.

---

## Current A-class reference candidate

As of 2026-09-11, all **29 tracked original-NTS-1 MkI units** pass the repository's shared production-DSP common gate and project-specific A-class engineering harnesses, and all 29 freshly compile/package in the consolidated ARM job.

Reference evidence:

- branch candidate: `field-0.1-0-test` @ `5718d12416a242137892688e240e8ac41be5a0ca`;
- workflow: **Pre-handoff suite**, run `34613566166`;
- common host gate: **29/29 PASS**;
- project-specific A-class gate: **29/29 PASS**;
- ARM build/package: **29/29 PASS**.

See `PRE_HANDOFF_COVERAGE.md` and `reports/testing/2026-09-11_full-a-class-suite.md`.

This is an **engineering** result. It does not convert known physical failures into passes. In particular, LATTICE CORE's high-TIME physical distortion and CORE + FIELD integration failure remain open until physical retest.

---

## Current reference target

- Korg Nu:Tekt NTS-1 digital kit, original / MkI
- repository reference API target: logue SDK `1.1-0`
- reference unit types: `osc`, `modfx`, `delfx`, `revfx`

The repository's CI clones the current official Korg logue SDK at runtime for build/package gates.

Cross-platform ports must identify their product/API separately and do not inherit hardware validation from MkI.

---

## Easiest way to contribute

There are three useful contribution paths.

### 1. Run the host engineering gates

Known CI environment: Ubuntu 22.04.

Local Linux/WSL requirements:

- Python 3
- `g++` with AddressSanitizer and UndefinedBehaviorSanitizer support

From the repository root:

```bash
python3 tests/pre-handoff/run_suite.py
python3 tests/pre-handoff/run_unit_specific.py
```

Expected current result:

```text
29/29 common-gate PASS
29/29 project-specific A-class PASS
```

The shared runner and project-specific runner compile/execute the actual production DSP source using host stubs. FIELD's deeper capture/delivery suite is included in the 29-unit project-specific command.

These are not cycle-accurate emulators for the NTS-1.

If you get a failure, report:

- operating system/distribution;
- compiler version (`g++ --version`);
- commit SHA;
- complete failing unit name;
- complete test output / sanitizer diagnostic;
- whether the failure reproduces on a clean checkout.

Do not modify production DSP merely to make a generic test pass until the test expectation has been checked against the unit's intended architecture. The A-class campaign has already found both genuine production lifecycle bugs and invalid generic-test assumptions; those must be distinguished.

### 2. Run the GitHub Actions pre-handoff suite

On a fork, enable Actions and run the **Pre-handoff suite** workflow manually, or open a pull request that touches the watched test/effect/oscillator paths.

The workflow has two independent jobs:

- `host-production-dsp`
- `arm-build-package`

The host job runs both the 29-unit common gate and the 29-unit project-specific A-class gate. The ARM job clones the current official Korg logue SDK and freshly builds/packages all tracked units.

A successful workflow establishes those tested host/build layers for that commit. It does **not** prove physical hardware loading, CPU deadline margin or musical quality.

### 3. Test on physical logue SDK hardware

Hardware reports are especially valuable because several facts cannot be certified by the desktop harness:

- actual user-unit loading and selection;
- real-time callback/deadline behavior;
- interaction with other active processors/runtimes;
- panel, touch and clock behavior;
- analog I/O behavior;
- tone and musicality.

Use `community/HARDWARE_TEST_REPORT.md` when reporting a device result and identify the exact product/firmware/build.

---

## How to improve an already A-class unit

A is not a claim that testing is finished forever. Good community contributions include:

- a deterministic regression test for one newly discovered hardware or DSP bug;
- a stronger measurement of a documented control relationship;
- a longer wrap/feedback/expiry test;
- additional low/normal/hot-level cases;
- a pitch/timing measurement that is less brittle than an existing assertion;
- a state-ownership/delivery test for a newly added capture behavior;
- a DC/headroom/decay test for a changed nonlinear or feedback path.

Every behavioral DSP change should extend or revise its tests in the same development cycle.

Prefer small, auditable tests. One test should answer one clear engineering question.

---

## Rules for good test contributions

### Use production DSP

Tests should compile/include the repository's actual production source where technically possible.

Do not create a simplified reimplementation and then treat its success as proof of the shipped algorithm.

### Test output consequences

An internal counter changing is not enough when the specification claims an audible result.

Examples:

- a delay event must actually reach output;
- a reverb must actually produce and then decay a tail;
- an oscillator must actually change pitch/interval as documented;
- a captured voice must actually expire and release ownership.

### Keep deterministic tests deterministic

For stochastic/chaotic designs, use a fixed test seed or assert bounded/statistical behavior rather than accidental waveform history.

Fresh initialization should reproduce documented deterministic seeded behavior where the unit promises that property.

### Do not tune tests around one accidental waveform

Tests should capture the contract of the effect: timing, bounds, state behavior, relative energy, interval relationships, decay, etc.

Avoid brittle sample-for-sample golden files unless exact output identity is genuinely part of the design.

### Separate engineering failures from taste

Examples:

- NaN / stuck state / runaway feedback = engineering failure.
- "the shimmer is too bright" = listening feedback.
- "TIME above 3 o'clock distorts only on physical MkI" = hardware/runtime report.

All are useful, but they should not be conflated.

---

## Physical testing discipline

When testing real hardware:

- identify the exact product, unit/version/commit or supplied artifact;
- identify firmware if known;
- state the input source;
- state all simultaneously active processors/runtimes;
- identify built-in vs custom processors;
- state exact control values or approximate positions;
- change one variable at a time during fault isolation;
- distinguish what you heard from what you infer caused it;
- do not report a compile/desktop PASS as a hardware PASS.

For prologue, include single/dual-timbre context. For NTS-3, include runtime/slot and XY/touch mapping. For combination tests, name every simultaneously active custom or built-in processor.

---

## Suggested report labels

Use one of these labels in prose or issues:

- **HOST PASS / FAIL** — deterministic production-DSP test result
- **BUILD PASS / FAIL** — compile/package result for the named target
- **LOAD PASS / FAIL** — physical product accepts/selects unit
- **RUNTIME PASS / FAIL** — short real-hardware stability test
- **MUSICAL PASS / PASS WITH NOTES / FAIL** — subjective usefulness/quality
- **RETEST** — candidate changed or failure unresolved

This prevents a successful test at one layer from being mistaken for success at all layers.

---

## Platform notes

For the original NTS-1 MkI, do not treat a user DELAY and user REVERB as a generally supported independent simultaneous pair. The LATTICE documentation records the practical implications for FIELD/Albedo testing.

Korg documents first-generation binary compatibility between prologue, minilogue xd and NTS-1 MkI when SDK versions match, but product-specific hardware testing is still required. In particular, prologue ModFX dual-timbre behavior needs explicit coverage.

Korg's current SDK also includes a browser/WebAudio `websim` path for **NTS-1 MkII and NTS-3**. This is useful as an additional development/test layer, but it still does not replace physical hardware validation.

---

## Where results go

- Generic suite/test-infrastructure findings: `reports/testing/`
- LATTICE-specific engineering/hardware findings: `reports/lattice/`
- Unit-specific QA: the unit's `docs/` directory or a focused pull request
- Hardware reports from community members: issue/PR using the hardware report template
- Cross-platform implementation notes: `PLATFORM_SUPPORT.md` and focused platform reports

A reproducible failure is more valuable than a vague "it broke." Include the smallest input/control sequence that reproduces it.
