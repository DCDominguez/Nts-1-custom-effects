# Community Testing Guide

This repository welcomes reproducible engineering tests and physical Korg logue SDK hardware reports.

The current production reference is the **original NTS-1 MkI**, while support for minilogue xd, prologue, NTS-1 MkII, NTS-3, microKORG2 and drumlogue is being added in stages. See `PLATFORM_SUPPORT.md`.

The goal is to separate three questions cleanly:

1. **Does the production DSP behave correctly under deterministic host tests?**
2. **Does the candidate build/package correctly for its stated logue SDK target?**
3. **Does it load, run and sound good on that real hardware product?**

These are related, but they are not the same result.

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

### 1. Run the shared host engineering gate

Known CI environment: Ubuntu 22.04.

Local Linux/WSL requirements:

- Python 3
- `g++` with AddressSanitizer and UndefinedBehaviorSanitizer support

From the repository root:

```bash
python3 tests/pre-handoff/run_suite.py
```

Expected current result:

```text
29/29 common-gate PASS
```

Project-specific A-class harnesses can be run with:

```bash
python3 tests/pre-handoff/run_unit_specific.py
```

The shared runner compiles and executes the actual production DSP source using host stubs. It checks common numerical/state/delivery behavior. It is not a cycle-accurate emulator for any Korg product.

If you get a failure, report:

- operating system/distribution;
- compiler version (`g++ --version`);
- commit SHA;
- complete failing unit name;
- complete test output / sanitizer diagnostic;
- whether the failure reproduces on a clean checkout.

Do not modify production DSP merely to make a generic test pass until the test expectation has been checked against the unit's intended architecture.

### 2. Run the GitHub Actions pre-handoff suite

On a fork, enable Actions and run the **Pre-handoff suite** workflow manually, or open a pull request that touches the watched test/effect/oscillator paths.

The current workflow has two independent jobs:

- `host-production-dsp`
- `arm-build-package`

The host job now also runs any checked-in project-specific A-class harnesses.

A successful workflow establishes the tested host/build layers for that commit. It does **not** prove physical hardware loading, CPU deadline margin or musical quality.

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

## How to help move units to A-class

See `A_CLASS_REQUIREMENTS.md`.

The suite-wide common gate is already implemented. Most units still need **project-specific tests** that prove the behavior claimed by their QA/spec rather than merely proving that audio is finite.

Good community contributions include:

- a deterministic test for one documented control relationship;
- a regression test for one known bug;
- a long-run wrap/feedback/expiry test;
- a pitch/timing measurement for an oscillator or clocked delay;
- a state-ownership/delivery test for a capture processor;
- a DC/headroom/decay test for a nonlinear or feedback processor.

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

For stochastic/chaotic designs, use a fixed test seed or assert bounded/statistical behavior rather than exact waveform identity.

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
