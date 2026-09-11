# Human Soon NTS-1 — A-Class Pre-Handoff Requirements

Target: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**  
Repository SDK target: **logue SDK API 1.1-0**

## What A-class means

**A-class is an engineering-coverage grade, not a claim that a unit sounds good and not a substitute for physical NTS-1 validation.**

A unit reaches A-class when the repository can reproduce and automatically verify the important engineering behavior of its **actual production DSP source** deeply enough that normal device handoff is reduced mainly to:

1. loading/selecting the `.ntkdigunit` on the physical MkI;
2. one short identified hardware-runtime sanity check that cannot be certified off-device;
3. subjective tone, musicality, identity and playability.

A-class therefore sits between "it compiles" and "DC/community hardware-tested it and likes the sound."

---

## Mandatory A-class gates

Every A-class unit must satisfy **all** of the following.

### A1 — Shared production-DSP common gate

The actual production source must pass the suite-wide deterministic host runner under the repository's sanitizer configuration.

Required common coverage:

- silence;
- normal signal delivery;
- bounded/finite output;
- parameter minimum/middle/maximum where applicable;
- repeated parameter sweeps;
- state/reset/rest behavior;
- enough processing to cross relevant buffer/state boundaries;
- no AddressSanitizer/UndefinedBehaviorSanitizer failure in the exercised path.

The shared common gate is necessary but **not sufficient** for A-class.

### A2 — Fresh ARM build/package gate

The exact candidate source must freshly compile and package against the current official original-NTS-1 logue SDK template used by CI.

Required evidence:

- correct unit type (`osc`, `modfx`, `delfx`, `revfx`);
- platform `nutekt-digital`;
- API `1.1-0` for the repository's MkI target;
- manifest name/version consistent with the candidate;
- successful `.ntkdigunit` creation;
- text/data/BSS recorded;
- source commit and CI run recorded.

Build/package PASS is still not physical hardware validation.

### A3 — Project-specific behavioral harness

This is the main work still missing for most units.

Each unit needs a deterministic test that proves the behavior that makes that unit itself, not merely that audio comes out.

The test must use the **production DSP implementation**, not a simplified rewrite of the algorithm.

Examples:

- SPECTRA: pitch relationship, voice-count behavior, spread bounds, harmony-mode interval behavior, SHAPE morph endpoints, bounded drift/chaos.
- DUST: RATE actually changes sample-hold cadence; DAMAGE changes quantization/fracture rather than only level; stereo fracture remains bounded.
- BALLISTIC: tap timing demonstrably transitions across accelerating/even/decelerating trajectory behavior.
- ASCENDER: barber-pole phase relationship and wrap continuity are maintained through long runs.
- AUREOLE: octave/shimmer energy is present in the intended path while the feedback network remains bounded and returns toward rest.

A test that only checks an internal counter/state flag is insufficient when the intended result is audible output. The intended consequence must reach the output path.

### A4 — Control-contract tests

Every documented control must be verified against its intended behavior.

At minimum:

- minimum;
- midpoint;
- maximum;
- important transition boundaries;
- rapid sweep;
- interacting controls where relevant;
- discrete modes/divisions individually exercised;
- MIX endpoints for Delay/Reverb effects;
- BPM fallback and tempo changes for clocked processors.

The test should assert meaningful behavior, not exact sample-for-sample identity when the design intentionally contains stochastic or chaotic variation.

### A5 — Architecture-specific stress tests

Add the relevant gate for every risky subsystem used by the unit.

#### Feedback/reverb

- repeated excitation;
- maximum documented feedback/decay region;
- finite output;
- no runaway DC/low-frequency accumulation;
- decay/rest measurement after input stops;
- long enough run to cross circular-buffer wraps.

#### Circular delay / moving read heads

- minimum/maximum delay;
- repeated wraps;
- fractional read boundaries where used;
- rapid parameter movement;
- no invalid indexes/state corruption.

#### Capture/granular/loop engines

- admission;
- ownership;
- replacement policy;
- event delivery;
- expiry/TTL;
- rest state;
- no unintended wet self-capture;
- wrap crossings.

#### Stochastic/chaotic systems

- deterministic test seed when possible;
- bounded long-run state;
- expected range/distribution sanity;
- no rare NaN/full-scale/stuck-state transition in soak.

#### Nonlinear processors

- low-level baseline;
- hot-but-bounded input;
- DC check;
- parameter changes must demonstrate the intended nonlinear character, not merely output-level growth when the design claim is tonal change.

### A6 — Soak and lifecycle tests

A-class requires a unit-specific soak long enough to exercise the unit's longest meaningful state lifetime.

The soak must include, as applicable:

- repeated state creation/destruction;
- repeated control changes;
- buffer wrap crossings;
- multiple tempo/mode transitions;
- suspend/resume/reset;
- tail expiry;
- high-state-turnover conditions.

The duration should be justified by the architecture rather than using one arbitrary fixed duration for every effect.

### A7 — Reproducibility

A community contributor must be able to reproduce the automated result from the repository without private scripts or undocumented fixtures.

Required repository material:

- test source;
- deterministic fixtures/seed values;
- one documented command or CI workflow;
- expected PASS/FAIL conditions;
- dependencies;
- report path;
- candidate source commit/version.

If a test depends on proprietary material, hidden files or private tooling, it cannot be the sole basis for A-class.

### A8 — Known-limitations handoff note

Every A-class candidate must explicitly state what remains unproven off-device.

Normally this is:

- actual Sound Librarian/device loading and selection;
- MkI real-time CPU/deadline margin;
- interaction with other simultaneously active processors;
- analog I/O behavior;
- tone, musicality and playability.

The handoff must identify one specific worst-case hardware sanity check rather than asking the tester to "try everything."

---

## A-class does not require subjective consensus

A technically A-class unit can still fail the musical gate.

Examples:

- it may be stable but boring;
- it may meet its algorithmic spec but sound harsh;
- the control curve may be technically correct but musically awkward;
- a community tester may dislike its identity.

Those are valid listening results and should be recorded separately from engineering coverage.

---

## Current path to make the whole suite A-class

The shared common gate and fresh ARM build gate already cover all 29 tracked units. The remaining work is principally **A3–A6: deep project-specific assertions and lifecycle/stress coverage**.

Recommended implementation order:

1. LATTICE CORE — known physical high-TIME failure means its specific runtime/state profile is highest priority.
2. SPECTRA, CHORDGHOST, DUST, CARRIER, VECTORFILTER, ATTRACTOR — active units with partial existing protocol work.
3. PHASEWELL, ASCENDER, HELIX, CAPSTAN, SIDEBAND, FAULTLINE.
4. BALLISTIC, RAINFALL, SWARMDELAY, GLITCHREPEAT, BUCKETLINE, LONGMEMORY, SHARD.
5. ABYSS, AUREOLE.
6. PARALLAX, IRONROT, ZEROCROSS, NEBULA only if the goal is literally A-class for historical/backburnered units rather than only active units.
7. Historical LATTICE ECHO/SPACE only if they remain part of the public test corpus.

FIELD is the reference implementation for the depth expected from A-class state/delivery testing.

---

## Two separate badges/statuses

To avoid misleading users, keep these separate:

**Engineering coverage**

- A / B / C / H from `PRE_HANDOFF_COVERAGE.md`.

**Physical/music validation**

- NOT TESTED
- LOAD PASS
- RUNTIME PASS
- MUSICAL PASS
- PASS WITH NOTES
- FAIL / RETEST

An A-class build should never be advertised as physically validated unless a real original NTS-1 MkI report exists for that exact candidate.
