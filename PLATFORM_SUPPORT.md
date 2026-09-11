# Human Soon logue SDK Platform Expansion

Status date: **2026-09-11**

This document defines how the Human Soon NTS-1 MkI suite can expand to other current Korg logue SDK platforms without weakening the existing MkI validation record.

## Current official Korg platform matrix

Korg's current `logue-sdk` repository lists these supported product targets:

| Platform | SDK | Minimum firmware | CPU | Relevant module types |
|---|---:|---:|---|---|
| prologue | 1.1.0 | 2.00 | Cortex-M4 | osc, modfx, delfx, revfx |
| minilogue xd | 1.1.0 | 2.00 | Cortex-M4 | osc, modfx, delfx, revfx |
| NTS-1 digital kit MkI | 1.1.0 | 1.02 | Cortex-M4 | osc, modfx, delfx, revfx |
| drumlogue | 2.0.0 | 1.0.0 | Cortex-A7 | synth, delfx, revfx, masterfx |
| NTS-1 digital kit MkII | 2.0.0 | 1.0.0 | Cortex-M7 | osc, modfx, delfx, revfx |
| NTS-3 kaoss pad kit | 2.0.0 | 1.0.0 | Cortex-M7 | genericfx |
| microKORG2 | 2.1.0 | 2.0.0 | Cortex-A7 | osc, modfx, delfx, revfx |

Official source: Korg `logue-sdk` README and platform READMEs.

## Compatibility groups

### Group A — first-generation v1.1 platforms

- prologue
- minilogue xd
- NTS-1 MkI

Korg states these are **binary compatible when the SDK version matches**. That makes them the lowest-friction expansion target.

However binary compatibility does **not** mean every current Human Soon implementation is semantically optimal on every product.

Important example: the v1 ModFX API exposes two processing buffers for prologue dual-timbre operation. Korg says both must be handled the same way on prologue; minilogue xd and NTS-1 MkI can safely ignore the second buffer. Several current Human Soon MkI ModFX intentionally ignore the secondary buffer, so they require a prologue-specific adapter/update before we claim proper prologue support.

Therefore:

- NTS-1 MkI remains the reference v1 target;
- minilogue xd is expected to be the easiest additional v1 hardware target;
- prologue needs explicit dual-timbre ModFX verification;
- specialized per-product builds are preferred even where Korg says binaries are compatible.

### Group B — NTS-1 MkII / SDK 2.0

NTS-1 MkII supports the same four conceptual unit categories as MkI, but the runtime/API and binary format are newer.

Official MkII resources currently specify:

- Cortex-M7 / STM32H725;
- dynamic ELF user units;
- `osc`: 16 slots, 48KB RAM load size;
- `modfx`: 16 slots, 16KB RAM load size + **256KB allocatable external memory**;
- `delfx`: 8 slots, 24KB RAM load size + **3MB allocatable external memory**;
- `revfx`: 8 slots, 24KB RAM load size + **3MB allocatable external memory**;
- independent custom Delay and Reverb runtimes.

This is an excellent Human Soon target.

The MkII uses a different unit callback/metadata model (`unit_init`, `unit_render`, `unit_set_param_value`, compiled `header.c`, runtime SDRAM allocation, etc.), so existing MkI binaries do not simply load on it. The DSP algorithms can nevertheless be shared behind a thin platform adapter.

### Group C — NTS-3 kaoss pad kit / SDK 2.0

NTS-3 exposes **generic effects** rather than separate Mod/Delay/Reverb categories.

Official platform characteristics:

- Cortex-M7 / STM32H725;
- 50 user generic-effect slots;
- four identical effect runtimes;
- up to ~32KB unit size / 32KB RAM load;
- **3MB allocatable external memory per runtime**;
- generic effects can implement any kind of effect and Korg explicitly notes they may even act as sound generators;
- platform callbacks include touch events, tempo and 4-PPQN timing.

This makes NTS-3 especially attractive for Human Soon effects because one adapter can map our ModFX, DelFX and RevFX concepts into `genericfx`, and the XY/touch interface can expose performance macros rather than merely copying the NTS-1 knob mapping.

### Group D — microKORG2 / SDK 2.1

Current official SDK support includes microKORG2 firmware >=2.0.0.

It supports:

- osc, modfx, delfx, revfx;
- Cortex-A7;
- 32 user slots for each unit category;
- external memory budgets of 8KB osc, 64KB modfx, 1MB delfx and 1MB revfx.

Its module categories align closely with the Human Soon suite, but it uses the newer dynamic SDK generation and therefore needs platform wrappers/builds rather than MkI binaries.

### Group E — drumlogue / SDK 2.0

drumlogue supports:

- synth;
- delfx;
- revfx;
- masterfx.

It does **not** expose the same oscillator/ModFX model as NTS-1. Korg describes drumlogue synth units as responsible for their own articulation/filtering, and master effects have their own routing/sidechain semantics.

Consequences:

- Human Soon DelFX and RevFX are the most direct candidates;
- SPECTRA would need to become a drumlogue synth, including articulation responsibilities;
- Human Soon ModFX would generally need redesign/adaptation as master effects rather than a mechanical port.

This is a later-phase target.

---

# Recommended architecture

Do not fork the musical algorithm separately for every Korg product.

Target structure:

```text
portable DSP core
      |
      +-- NTS-1 MkI adapter / API 1.1
      +-- minilogue xd adapter / API 1.1
      +-- prologue adapter / API 1.1
      +-- NTS-1 MkII adapter / API 2.0
      +-- NTS-3 genericfx adapter / API 2.0
      +-- microKORG2 adapter / API 2.1
      +-- drumlogue adapter / API 2.0 where conceptually appropriate
```

The existing MkI production source must not be destabilized merely to create this structure.

Migration rule:

1. first give a unit A-class behavioral coverage on MkI;
2. extract or wrap its DSP core with regression comparison against the MkI reference;
3. make the platform adapter responsible for callback shape, parameter metadata, memory allocation and product-specific controls;
4. run the same platform-neutral behavioral assertions against every port where the musical contract is meant to stay the same;
5. add platform-specific assertions for changed controls/routing;
6. build in official Korg templates in CI;
7. physical hardware remains the authority for load/runtime behavior.

## Why A-class comes first

The A-class work is what makes safe multi-platform refactoring practical.

Without behavioral tests, extracting a DSP core risks silently changing the sound. With deterministic output/timing/control assertions, we can prove that the MkII/NTS-3/microKORG2 implementation still behaves like the Human Soon unit rather than merely compiling.

---

# Expansion priority

## Phase 1 — minilogue xd + prologue build audit

Reason: same v1.1 generation and official binary-compatibility family.

Tasks:

- add CI builds against each official template;
- test manifests/unit packaging for each product;
- audit prologue ModFX secondary-buffer handling;
- preserve MkI binaries as the reference output;
- recruit hardware testers for load/tone checks.

## Phase 2 — NTS-1 MkII

Reason: closest conceptual successor, much larger delay/reverb memory, independent custom Delay/Reverb runtimes, and an official Korg web simulator path.

Start with simple A-class units such as DUST/CARRIER, then DelFX/RevFX, then LATTICE.

### LATTICE opportunity on MkII

MkII removes the first-generation shared custom Delay/Reverb restriction. Its separate runtimes mean the architectural target can once again be explored as:

```text
LATTICE CORE  [MOD]
      ↓
LATTICE FIELD [DELAY]
      ↓
LATTICE SPACE [REVERB]
```

This is only a platform capability/design opportunity until built and physically tested; it is not a hardware PASS claim.

## Phase 3 — NTS-3

Port effects to genericfx and design performance-oriented XY mappings.

A compelling LATTICE configuration is possible in principle because NTS-3 provides four generic-effect runtimes with large external memory. CORE/FIELD/SPACE could become separate generic units or a consolidated performance unit. Hardware validation would still be required.

## Phase 4 — microKORG2

Port the portable cores through the v2.1 wrappers. Its four familiar unit categories make most of the suite conceptually transferable.

## Phase 5 — drumlogue

Prioritize DelFX and RevFX first. Treat synth/masterfx versions as product-specific redesigns rather than simple ports.

---

# Testing strategy across platforms

The community test system should report platform separately from musical unit identity.

Example status:

```text
DUST core behavior: A-class
NTS-1 MkI adapter: ARM PASS / hardware PASS
NTS-1 MkII adapter: build PASS / hardware open
NTS-3 adapter: build PASS / hardware open
```

A port does not inherit hardware validation from another platform.

## Official web simulator opportunity

Korg's current SDK includes a `websim` workflow for **NTS-1 MkII and NTS-3**. The official README documents building a normal project with `make wasm` and exercising the processor through a browser/WebAudio graph.

We should add this as an additional pre-hardware test layer for those two platforms. It is still not a substitute for physical timing/loading validation, but it gives the community a Korg-provided interactive DSP test environment in addition to our deterministic harness.

---

# Community hardware coverage

We currently own/target the original NTS-1 MkI as the physical reference. Additional platform support should explicitly seek community owners of:

- minilogue xd;
- prologue;
- NTS-1 MkII;
- NTS-3;
- microKORG2;
- drumlogue.

Community reports must identify exact product, firmware, unit build/commit, slot/module and active processor chain.

No platform is marked hardware validated solely because another logue SDK product passed.
