# Human Soon NTS-1 — Pre-Handoff Testing Protocol

Target: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**  
SDK target: **logue SDK API 1.1-0**

## Purpose

FIELD demonstrated a better development loop: reproduce functional failures internally, exercise the real production DSP under deterministic test inputs, and only hand a build to DC after the engineering questions have been reduced as far as possible without the physical NTS-1.

This protocol makes that the default for **every Human Soon oscillator/effect**, including older M1/M2 units when they are next changed.

The intended handoff is now mostly:

1. **device load/smoke check** — the `.ntkdigunit` loads, selects and produces audio on the physical MkI;
2. **brief hardware-runtime sanity** — sweep the real panel controls and confirm no immediate device-specific dropout/crunch/stall appears;
3. **tone and musicality** — DC judges whether the effect actually sounds good, distinctive and useful.

The developer/CI side owns functional correctness before handoff wherever that can be established without the physical MkI.

This does **not** claim that desktop tests can prove MkI real-time CPU margin. Hardware remains the authority for actual loading, host integration and real-time deadline behavior.

---

## Handoff gate

A build must not be handed to DC merely because it compiles.

Before handoff, the unit must have:

- successful ARM build against the current official original-NTS-1 template/API used by the repository;
- valid manifest/module/API/version/name checks;
- deterministic host-side execution of the **actual production DSP source** where technically possible;
- finite output checks: no NaN/Inf and no unbounded state growth;
- silence/rest checks;
- low, normal and hot input-level checks where the unit processes input audio;
- parameter minimum/middle/maximum checks;
- rapid parameter sweep checks where the algorithm has moving read heads, feedback, resonance, state transitions or discrete modes;
- long enough host-side soak to expose stuck state, wrap errors, runaway feedback and state leaks;
- unit-specific behavioral assertions derived from its SPEC/README/QA sheet;
- a written record of any known limitation that remains hardware-only or listening-only.

If a deterministic functional failure can be reproduced internally, **fix or explicitly classify it before another hardware handoff**.

---

# 1. Common test layers

## Layer A — build / package / ABI

Required for every unit.

Check:

- expected unit type: `osc`, `modfx`, `delfx` or `revfx`;
- expected platform: `nutekt-digital`;
- API matches the repository's original-NTS-1 target;
- manifest version and display name match the candidate;
- unit packages successfully as `.ntkdigunit`;
- no accidental source/manifest mismatch;
- text/data/BSS sizes are recorded so regressions are visible.

A package PASS is **BUILD ONLY**, not a hardware or musical pass.

## Layer B — numerical and state safety

Run production DSP with deterministic inputs and reject:

- NaN/Inf;
- impossible indexes / buffer wrap violations;
- stuck voices/read heads after expiry;
- uncontrolled DC accumulation where not intentional;
- state that fails to clear on suspend/resume/reset paths;
- runaway feedback/resonance;
- permanent self-oscillation unless explicitly part of the specification;
- unexpected silence caused by internal state corruption.

Use sanitizers in host tests whenever the production source can be compiled under them.

## Layer C — input corpus

For input-processing effects, exercise at least:

- silence;
- single impulse;
- isolated notes/tones with silence between them;
- legato/consecutive tones;
- sustained tone;
- dense/overlapping material;
- low-level input;
- normal-level input;
- hot-but-bounded input;
- stereo-asymmetric input where stereo processing is claimed;
- noise/broadband input where filters, nonlinear stages or feedback may react differently from sine tests.

The exact waveforms are test fixtures, not claims about musical quality.

## Layer D — control surface

For every exposed parameter:

- minimum;
- useful low setting;
- midpoint;
- useful high setting;
- maximum.

Also test:

- rapid sweep minimum→maximum→minimum;
- simultaneous movement of two exposed parameters where interactions matter;
- discrete mode boundaries;
- tempo/division changes for clocked units;
- MIX extremes for delay/reverb units.

Parameter sweeps should not produce crashes, non-finite output, stuck buffers or state corruption. Audible zippering/tone quality is still a listening judgment unless a deterministic discontinuity can be measured.

## Layer E — delivery, not just activity

Internal tests must assert the **audible/functional consequence**, not merely that internal state changed.

Examples:

- a delay capture counter increasing is not enough — scheduled responses must actually reach output;
- a voice allocation is not enough — it must expire and free correctly;
- a reverb feedback network receiving energy is not enough — it must decay toward rest;
- an oscillator voice being active is not enough — it must track the requested pitch and remain finite.

This is the key lesson from FIELD.

## Layer F — soak / abuse

Run extended deterministic processing with:

- repeated control changes;
- repeated note/capture events;
- maximum practical state turnover;
- buffer wrap crossings;
- repeated suspend/resume where supported;
- long tail/feedback decay.

The host soak is designed to catch logic/state defects. It does not certify MkI cycle margin.

---

# 2. Unit-type protocols

## Oscillator (`osc`)

Internally verify:

- note/pitch input maps to the expected fundamental relationship;
- note changes do not leave stale voice state;
- SHAPE/ALT or project-specific parameters reach their intended ranges;
- output remains finite at parameter extremes;
- DC is bounded;
- silence/reset behavior is correct where applicable;
- high-note behavior receives an aliasing/spectral sanity check when the design can create excessive harmonics;
- any stochastic mutation remains bounded and reproducible enough for regression testing when seeded.

**DC handoff focus:** loads/selects, pitch feels correct on the real keyboard/MIDI path, then tone/character/musicality.

## ModFX (`modfx`)

Internally verify:

- stereo in/out behavior;
- all parameter extremes and sweeps;
- modulation phase/state continuity;
- filter/resonance/feedback bounds;
- no output disappearance from invalid internal state;
- low/normal/hot input behavior;
- silence does not create unintended runaway output;
- any stochastic/chaotic state stays bounded;
- suspend/resume clears or preserves state exactly as specified.

For CPU-heavy or multi-voice modulation, add a workload proxy and identify worst-case control regions before device handoff.

**DC handoff focus:** loads, brief worst-case control sweep on hardware, then tone/movement/musicality.

## DelFX (`delfx`)

Internally verify:

- impulse-response timing against intended delay/division behavior;
- buffer wrap safety;
- feedback decay or explicit finite lifetime;
- MIX endpoints and dry-path integrity;
- clock/division changes;
- read-head or capture transitions;
- reverse/stutter/granular boundaries where applicable;
- no endless distinct repeats unless infinite behavior is explicitly intended;
- no self-recapture unless explicitly intended;
- all scheduled events actually reach output;
- tail returns to rest when the design is meant to be finite.

FIELD's ownership/deadline/delivery tests are the model for stateful delay processors.

**DC handoff focus:** loads, confirms real NTS-1 clock/panel behavior, then echo feel/tone/rhythm/musicality.

## RevFX (`revfx`)

Internally verify:

- impulse excites the network;
- tail energy grows/decays according to the intended control range;
- bounded feedback at maximum TIME/DEPTH;
- no stuck tail after silence;
- no runaway low-frequency/DC accumulation;
- stereo network remains finite and decorrelated as intended;
- MIX endpoints preserve the intended source/wet relationship;
- parameter sweeps do not corrupt delay-line state;
- long-tail soak crosses all circular-buffer wraps.

**DC handoff focus:** loads, brief maximum-decay hardware sanity, then size/tone/depth/musicality.

---

# 3. Extra protocols for risky architectures

Add explicit tests when a unit uses any of the following.

### Feedback

- maximum feedback state;
- repeated excitation;
- silence after excitation;
- measured decay/rest condition;
- guard/limiter engagement count if present;
- verify the guard is not continuously hiding an upstream gain error.

### Circular delay buffers / moving read heads

- multiple wrap crossings;
- fractional read boundaries;
- minimum and maximum delay lengths;
- rapid time changes;
- no negative/out-of-range indexes.

### Granular / capture / loop ownership

- admission under isolated and dense input;
- slot ownership;
- replacement rules;
- event delivery;
- expiry/TTL;
- rest state;
- no unintended wet self-capture;
- repeated wrap crossings.

### Stochastic / chaotic processors

- deterministic seed for regression tests;
- bounded output/state;
- statistical range sanity over long runs;
- no rare transition to NaN, stuck state or full-scale latch.

### Nonlinear distortion / saturation

- low-level near-linear baseline;
- expected transfer change as intensity rises;
- DC offset check;
- hot-input bound;
- output-level growth distinguished from actual nonlinear-character growth where the parameter is meant to change texture rather than merely loudness.

### Clocked / tempo-synced processors

- multiple BPM values;
- every documented division/mode;
- tempo change during active state;
- invalid/unavailable BPM fallback;
- no missed delivery caused only by scheduling starvation.

---

# 4. What remains for the physical MkI

Internal testing should remove as much debugging as possible, but these cannot be honestly certified off-device:

- Sound Librarian/user-unit loading and selection on the actual MkI;
- actual NTS-1 host callback timing and real-time CPU deadline margin;
- interaction with the physical panel and host clock implementation;
- true coexistence with other simultaneously active NTS-1 processors;
- analog input/output and monitoring behavior;
- subjective tone, musicality, playability and whether the effect is worth using.

Therefore the normal DC handoff becomes a **short hardware acceptance pass**, not a debugging session.

### Normal DC acceptance pass

1. Load/send the user unit and select it.
2. Confirm audio appears and the intended panel controls respond.
3. Perform one brief worst-case sweep identified in the handoff notes.
4. Judge tone, musicality, identity and performance feel.
5. Report any hardware-only anomaly literally.

A longer hardware stress pass is reserved for processors whose architecture or prior history makes real-time stability a specific open risk.

---

# 5. Handoff package requirements

Every candidate handed to DC must include:

- candidate name/version;
- source commit;
- CI/build run;
- `.ntkdigunit` artifact;
- manifest/module/API verification;
- test summary: PASS/FAIL per internal layer;
- worst-case parameter region identified by internal tests/source review;
- known omissions/limitations;
- **exactly what DC still needs to test**.

Do not hand over a vague instruction such as “try everything.”

Preferred final line:

> **Internal engineering gate: PASS. Remaining physical checks: load/select, [specific worst-case hardware sanity], and tone/musicality.**

If the internal engineering gate is not passed, label the build **DIAGNOSTIC** rather than a normal handoff.

---

# 6. Existing-suite adoption rule

This protocol applies immediately to new development.

Older units do not need gratuitous DSP changes merely to add tests. Instead:

- backfill deterministic tests without changing production sound where practical;
- whenever an older unit is reopened for DSP work, its pre-handoff tests become mandatory before the next device build;
- known problematic/backburnered units receive deeper backfill before redesign handoff;
- historical hardware validation remains valid for the historical binaries, but does not substitute for testing a changed candidate.

Coverage is tracked in `PRE_HANDOFF_COVERAGE.md`.

---

## Official platform references checked for this protocol

Korg's current original-NTS-1 materials continue to identify the original NTS-1 as supporting user oscillator, modulation, delay and reverb units through the logue SDK, with user units transferred through the NTS-1 Sound Librarian. The current API reference continues to list original NTS-1 API `1.1-0`.

These platform facts define the integration boundary; they do not provide a cycle-accurate desktop emulator. That is why actual loading and real-time deadline behavior remain physical-MkI checks.
