# Community Testing Guide

This repository welcomes reproducible engineering tests and physical original-NTS-1 MkI reports.

The goal is to separate three questions cleanly:

1. **Does the production DSP behave correctly under deterministic host tests?**
2. **Does the candidate build/package correctly for the original NTS-1 MkI target?**
3. **Does it load, run and sound good on real hardware?**

These are related, but they are not the same result.

---

## Target platform

- Korg Nu:Tekt NTS-1 digital kit, original / MkI
- repository API target: logue SDK `1.1-0`
- unit types used here: `osc`, `modfx`, `delfx`, `revfx`

The repository's CI clones the current official Korg logue SDK at runtime for the ARM build/package gate.

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

The script compiles and executes the actual production DSP source using host stubs. It checks common numerical/state/delivery behavior. It is not a cycle-accurate NTS-1 emulator.

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

The workflow has two independent jobs:

- `host-production-dsp`
- `arm-build-package`

A successful workflow establishes the shared host gate and fresh ARM compile/package gate for the tested commit.

It does **not** prove physical MkI loading, CPU deadline margin or musical quality.

### 3. Test on a physical original NTS-1 MkI

Hardware reports are especially valuable because several facts cannot be certified by the desktop harness:

- actual user-unit loading and selection;
- real-time callback/deadline behavior on the MkI;
- interaction with other active processors;
- panel/clock behavior;
- analog I/O behavior;
- tone and musicality.

Use `community/HARDWARE_TEST_REPORT.md` when reporting a device result.

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

- identify the exact unit/version/commit or supplied artifact;
- identify the original NTS-1 MkI firmware if known;
- state the input source;
- state which MOD/DELAY/REVERB slots are active;
- state exact control positions or approximate clock-face positions;
- change one variable at a time during fault isolation;
- distinguish what you heard from what you infer caused it;
- do not report a compile/desktop PASS as a hardware PASS.

For combination tests, name every simultaneously active custom or built-in processor.

---

## Suggested report labels

Use one of these labels in prose or issues:

- **HOST PASS / FAIL** — deterministic production-DSP test result
- **ARM BUILD PASS / FAIL** — compile/package result
- **LOAD PASS / FAIL** — physical MkI accepts/selects unit
- **RUNTIME PASS / FAIL** — short real-hardware stability test
- **MUSICAL PASS / PASS WITH NOTES / FAIL** — subjective usefulness/quality
- **RETEST** — candidate changed or failure unresolved

This prevents a successful test at one layer from being mistaken for success at all layers.

---

## Current known platform limitation

For the original NTS-1 MkI, do not treat a user DELAY and user REVERB as a generally supported independent simultaneous pair. The LATTICE documentation records the practical implications for FIELD/Albedo testing.

When reporting combination behavior, say whether each processor is built-in or custom.

---

## Where results go

- Generic suite/test-infrastructure findings: `reports/testing/`
- LATTICE-specific engineering/hardware findings: `reports/lattice/`
- Unit-specific QA: the unit's `docs/` directory or a focused pull request
- Hardware reports from community members: issue/PR using the hardware report template

A reproducible failure is more valuable than a vague "it broke." Include the smallest input/control sequence that reproduces it.
