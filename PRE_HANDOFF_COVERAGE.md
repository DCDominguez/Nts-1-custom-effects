# Human Soon NTS-1 — Pre-Handoff Test Coverage

This matrix tracks how much of the new `PRE_HANDOFF_PROTOCOL.md` is actually automated or internally reproducible for each unit.

It is **not** a hardware-validation table. Historical hardware status remains in `TESTING.md` / `HARDWARE_VALIDATION.md`.

## Coverage levels

- **A — deep pre-handoff**: production DSP exercised through deterministic host tests with state/delivery/safety checks plus ARM build/package.
- **B — partial pre-handoff**: meaningful unit/protocol tests or detailed deterministic test plan exist, but production DSP host coverage is incomplete.
- **C — build + QA only**: ARM build/package and project QA exist; deterministic production-DSP host harness still needs backfill.
- **H — historical/retired**: retained for history; deeper tests required before redesign/reactivation handoff.

The target for any actively changed unit is **A before normal hardware handoff**, unless a specific part of the API cannot be represented off-device. Hardware-only gaps must be written into the handoff.

## Current matrix

| Unit | Type | Current coverage | Next pre-handoff backfill |
|---|---|---:|---|
| SPECTRA | osc | B | production-DSP pitch/parameter/finite-output host harness + soak |
| PARALLAX | delfx | H/B | if reopened: production-DSP delay-wrap, sweep, stereo/delivery harness |
| CHORDGHOST | delfx | B | extend protocol test into production-DSP clock/division/delivery harness |
| DUST | modfx | B | production-DSP input-level, sweep, silence and finite-output harness |
| CARRIER | modfx | B | production-DSP AM/ring transition, level/DC and sweep harness |
| VECTORFILTER | modfx | B | production-DSP cutoff/vector/resonance and bounded-output harness |
| IRONROT | modfx | H/B | if reopened: nonlinear-character vs loudness, DC/hot-input harness |
| ATTRACTOR | modfx | B | deterministic-seed chaotic-state bound/soak harness |
| ZEROCROSS | modfx | H/C | if reopened: through-zero sweep, feedback bound and wrap harness |
| PHASEWELL | modfx | C | phase/state/resonance/sweep host harness |
| ASCENDER | modfx | C | barber-pole wrap/state continuity host harness |
| HELIX | modfx | C | moving-read-head wrap/feedback/sweep host harness |
| CAPSTAN | modfx | C | deterministic wear/dropout/wow state and long-soak harness |
| SIDEBAND | modfx | C | shift/divergence/finite/DC/high-frequency sanity harness |
| FAULTLINE | modfx | C | instability-bound/self-recovery/sweep host harness |
| BALLISTIC | delfx | C | tap-timing/trajectory/feedback-decay/wrap host harness |
| RAINFALL | delfx | C | event-delivery/density/retarget/wrap host harness |
| SWARMDELAY | delfx | C | BPM/division/fallback/divergence/delivery host harness |
| GLITCHREPEAT | delfx | C | capture/repeat-state/expiry/BPM-change host harness |
| BUCKETLINE | delfx | C | delay-wrap/AGE/nonlinear/feedback-decay host harness |
| LONGMEMORY | delfx | C | long-buffer wrap, memory-state, parameter-sweep soak harness |
| SHARD | delfx | C | pitch-read/grain-boundary/wrap/finite-output host harness |
| ABYSS | revfx | C | impulse-tail/feedback-decay/DC/rest/long-wrap host harness |
| AUREOLE | revfx | C | shimmer-feedback/impulse-tail/rest/DC/long-wrap host harness |
| NEBULA | revfx | H/C | if reopened: clipping reproduction, grain/tail delivery, headroom harness |
| LATTICE CORE | modfx | B | high-TIME production-DSP runtime/state harness; pending 0.3-1 decision |
| LATTICE ECHO | delfx | H/B | historical; only deepen if reused independently |
| LATTICE SPACE | revfx | H/B | historical; only deepen if reused independently |
| LATTICE FIELD | delfx | **A** | maintain/extend with every behavior change |

## Why FIELD is the reference

FIELD's current workflow tests more than compilation. It exercises production DSP under isolated and busy inputs, checks delivery rather than merely capture activity, covers state ownership/expiry, control/mode/division behavior, buffer wrapping, ABI behavior, bounded output and known integration probes.

That is the model to reproduce across the suite.

## Backfill order

Backfill should follow development value rather than alphabetical order:

1. **currently active / known failure:** LATTICE CORE;
2. **active units likely to receive further M2/M3 work:** SPECTRA, CHORDGHOST, DUST, CARRIER, VECTORFILTER, ATTRACTOR;
3. **active M1 units:** PHASEWELL, ASCENDER, HELIX, CAPSTAN, SIDEBAND, FAULTLINE, BALLISTIC, RAINFALL, SWARMDELAY, GLITCHREPEAT, BUCKETLINE, LONGMEMORY, SHARD, ABYSS, AUREOLE;
4. **backburner units only when deliberately reopened:** PARALLAX, IRONROT, ZEROCROSS, NEBULA;
5. historical LATTICE ECHO/SPACE only if they return to active use.

## Rule for future handoff

A changed candidate at B/C coverage is not automatically blocked from a diagnostic device test, but it must be labeled **DIAGNOSTIC** and the missing internal coverage must be explicit.

A normal musical handoff should aim for **A coverage**, leaving DC mainly with:

- load/select on the physical NTS-1;
- one short hardware-specific worst-case sanity sweep;
- tone, musicality, identity and performance feel.
