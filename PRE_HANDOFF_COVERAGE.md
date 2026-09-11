# Human Soon NTS-1 — Pre-Handoff Test Coverage

This matrix tracks how much of the new `PRE_HANDOFF_PROTOCOL.md` is actually automated or internally reproducible for each unit.

It is **not** a hardware-validation table. Historical hardware status remains in `TESTING.md` / `HARDWARE_VALIDATION.md`.

## Suite-wide common-gate retest — 2026-09-11

All **29 tracked units** were retested through the new shared production-DSP common gate and a fresh ARM build/package gate.

Final GitHub Actions workflow: **Pre-handoff suite**, run **34605006118**.

Results:

- host production-DSP common gate: **29/29 PASS**;
- fresh ARM compile/package against the current official Korg logue SDK cloned at CI runtime: **29/29 PASS**;
- sanitizer/common-state checks: PASS for all exercised units;
- production DSP was not changed to obtain the final common-gate pass.

See `reports/testing/2026-09-11_suite-wide-new-standard-retest.md`.

This does **not** automatically promote every unit to A. The common gate is now present across the suite, but A-level coverage still requires sufficiently deep project-specific behavioral assertions for that unit. FIELD remains the reference for that deeper layer.

## Coverage levels

- **A — deep pre-handoff**: production DSP exercised through deterministic host tests with state/delivery/safety checks plus ARM build/package, with project-specific behavior covered deeply enough for normal handoff.
- **B — partial pre-handoff**: shared common gate passes and meaningful unit/protocol tests or detailed deterministic test plan exist, but project-specific production-DSP host coverage is incomplete.
- **C — common gate + QA baseline**: shared production-DSP common gate and ARM build/package pass, and project QA exists, but dedicated project-specific deterministic host assertions still need backfill.
- **H — historical/retired**: retained for history; shared common gate may pass, but deeper tests are required before redesign/reactivation handoff.

The target for any actively changed unit is **A before normal hardware handoff**, unless a specific part of the API cannot be represented off-device. Hardware-only gaps must be written into the handoff.

## Current matrix

All entries below passed the 2026-09-11 shared common gate and fresh ARM build/package unless explicitly stated otherwise.

| Unit | Type | Current coverage | Next pre-handoff backfill |
|---|---|---:|---|
| SPECTRA | osc | B | deepen production-DSP pitch/parameter/finite-output identity assertions + soak |
| PARALLAX | delfx | H/B | if reopened: dedicated delay-wrap, sweep, stereo/delivery identity harness |
| CHORDGHOST | delfx | B | extend protocol test into deeper production-DSP clock/division/delivery assertions |
| DUST | modfx | B | deepen RATE/DAMAGE identity and low/normal/hot level assertions |
| CARRIER | modfx | B | deepen AM/ring transition, level/DC and sweep assertions |
| VECTORFILTER | modfx | B | deepen cutoff/vector/resonance transfer assertions |
| IRONROT | modfx | H/B | if reopened: nonlinear-character vs loudness, DC/hot-input identity harness |
| ATTRACTOR | modfx | B | deterministic-seed chaotic-state range/identity assertions |
| ZEROCROSS | modfx | H/C | if reopened: dedicated through-zero sweep, feedback bound and wrap identity harness |
| PHASEWELL | modfx | C | phase/state/resonance identity harness |
| ASCENDER | modfx | C | barber-pole wrap/state-continuity identity harness |
| HELIX | modfx | C | moving-read-head wrap/feedback identity harness |
| CAPSTAN | modfx | C | deterministic wear/dropout/wow identity + longer soak harness |
| SIDEBAND | modfx | C | shift/divergence/frequency-translation identity harness |
| FAULTLINE | modfx | C | instability-bound/self-recovery identity harness |
| BALLISTIC | delfx | C | tap-timing/trajectory/feedback-decay identity harness |
| RAINFALL | delfx | C | event-delivery/density/retarget identity harness |
| SWARMDELAY | delfx | C | BPM/division/fallback/divergence identity harness |
| GLITCHREPEAT | delfx | C | capture/repeat-state/expiry/BPM-change identity harness |
| BUCKETLINE | delfx | C | delay/AGE/nonlinear/feedback-decay identity harness |
| LONGMEMORY | delfx | C | long-buffer/memory-state/near-loop identity harness |
| SHARD | delfx | C | pitch-read/grain-boundary identity harness |
| ABYSS | revfx | C | impulse-tail/decay/space identity harness |
| AUREOLE | revfx | C | shimmer-feedback/halo/tail identity harness |
| NEBULA | revfx | H/C | if reopened: clipping reproduction, grain/tail delivery, headroom identity harness |
| LATTICE CORE | modfx | B | high-TIME production-DSP workload/state harness; physical high-TIME issue remains open |
| LATTICE ECHO | delfx | H/B | historical; only deepen if reused independently |
| LATTICE SPACE | revfx | H/B | historical; only deepen if reused independently |
| LATTICE FIELD | delfx | **A** | maintain/extend with every behavior change |

## Why FIELD is still the reference

FIELD's workflow tests more than the shared suite common gate. It exercises production DSP under isolated and busy inputs, checks delivery rather than merely capture activity, covers state ownership/expiry, control/mode/division behavior, buffer wrapping, ABI behavior, bounded output and known integration probes.

The shared 2026-09-11 suite runner now gives every tracked unit a real production-DSP safety/delivery baseline, but FIELD remains the model for project-specific depth.

## Backfill order

Backfill should follow development value rather than alphabetical order:

1. **currently active / known failure:** LATTICE CORE;
2. **active units likely to receive further M2/M3 work:** SPECTRA, CHORDGHOST, DUST, CARRIER, VECTORFILTER, ATTRACTOR;
3. **active M1 units:** PHASEWELL, ASCENDER, HELIX, CAPSTAN, SIDEBAND, FAULTLINE, BALLISTIC, RAINFALL, SWARMDELAY, GLITCHREPEAT, BUCKETLINE, LONGMEMORY, SHARD, ABYSS, AUREOLE;
4. **backburner units only when deliberately reopened:** PARALLAX, IRONROT, ZEROCROSS, NEBULA;
5. historical LATTICE ECHO/SPACE only if they return to active use.

## Rule for future handoff

A changed candidate at B/C coverage is not automatically blocked from a diagnostic device test, but it must be labeled **DIAGNOSTIC** and the missing project-specific internal coverage must be explicit.

A normal musical handoff should aim for **A coverage**, leaving DC mainly with:

- load/select on the physical NTS-1;
- one short hardware-specific worst-case sanity sweep;
- tone, musicality, identity and performance feel.
