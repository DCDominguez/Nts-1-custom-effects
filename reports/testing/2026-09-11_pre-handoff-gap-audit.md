# Suite pre-handoff gap audit — 2026-09-11

## Purpose

Audit every current Human Soon NTS-1 unit against the new `PRE_HANDOFF_PROTOCOL.md` engineering gate.

Important distinction: this audit does **not** revoke historical physical-MkI validation of older binaries. It answers a narrower question: **which units currently have A-level internal pre-handoff coverage under the new standard?** A changed candidate should reach A before normal musical handoff; B/C candidates are diagnostic-only unless the missing coverage is explicitly hardware-only.

## Result

- Total tracked units: **29**
- A-level deep pre-handoff: **1**
- Below A-level: **28**
- Active units below A-level: **22**
- Historical/backburner units below A-level: **6**

### A-level reference

- `LATTICE FIELD` — **A**

FIELD is currently the only unit that meets the new deep pre-handoff standard: production DSP is exercised deterministically with delivery/state/safety checks in addition to ARM build/package.

## Active units that have NOT passed the new standard

### B — partial pre-handoff

These have meaningful deterministic/unit/protocol coverage, but incomplete production-DSP host coverage:

1. `SPECTRA` (`osc`) — needs production-DSP pitch/parameter/finite-output harness + soak.
2. `CHORDGHOST` (`delfx`) — needs production-DSP clock/division/delivery harness beyond protocol-only coverage.
3. `DUST` (`modfx`) — needs production-DSP input-level, sweep, silence, and finite-output harness.
4. `CARRIER` (`modfx`) — needs production-DSP AM/ring transition, level/DC, and sweep harness.
5. `VECTORFILTER` (`modfx`) — needs production-DSP cutoff/vector/resonance and bounded-output harness.
6. `ATTRACTOR` (`modfx`) — needs deterministic-seed chaotic-state bound/soak harness.
7. `LATTICE CORE` (`modfx`) — needs high-TIME production-DSP runtime/state harness; current physical high-TIME failure remains open and the proposed 0.3-1 runtime profile is still pending approval.

### C — build + QA only

These compile/package and have QA sheets, but still need deterministic production-DSP host harnesses:

8. `PHASEWELL` (`modfx`) — phase/state/resonance/sweep.
9. `ASCENDER` (`modfx`) — barber-pole wrap/state continuity.
10. `HELIX` (`modfx`) — moving-read-head wrap/feedback/sweep.
11. `CAPSTAN` (`modfx`) — deterministic wear/dropout/wow state + long soak.
12. `SIDEBAND` (`modfx`) — shift/divergence/finite/DC/high-frequency sanity.
13. `FAULTLINE` (`modfx`) — instability bound/self-recovery/sweep.
14. `BALLISTIC` (`delfx`) — tap timing/trajectory/feedback decay/wrap.
15. `RAINFALL` (`delfx`) — event delivery/density/retarget/wrap.
16. `SWARMDELAY` (`delfx`) — BPM/division/fallback/divergence/delivery.
17. `GLITCHREPEAT` (`delfx`) — capture/repeat-state/expiry/BPM change.
18. `BUCKETLINE` (`delfx`) — delay wrap/AGE/nonlinear/feedback decay.
19. `LONGMEMORY` (`delfx`) — long-buffer wrap/memory state/parameter-sweep soak.
20. `SHARD` (`delfx`) — pitch reads/grain boundaries/wrap/finite output.
21. `ABYSS` (`revfx`) — impulse tail/feedback decay/DC/rest/long-wrap.
22. `AUREOLE` (`revfx`) — shimmer feedback/impulse tail/rest/DC/long-wrap.

## Historical/backburner units that also have NOT passed the new standard

These do not need immediate backfill unless deliberately reopened, but they must reach A before a normal redesigned handoff:

23. `PARALLAX` — H/B; if reopened, production-DSP delay-wrap/sweep/stereo-delivery harness.
24. `IRONROT` — H/B; if reopened, nonlinear-character-vs-loudness/DC/hot-input harness.
25. `ZEROCROSS` — H/C; if reopened, through-zero sweep/feedback-bound/wrap harness.
26. `NEBULA` — H/C; if reopened, clipping reproduction/grain-tail delivery/headroom harness.
27. `LATTICE ECHO` — H/B; historical standalone unit; deepen only if reused independently.
28. `LATTICE SPACE` — H/B; historical standalone unit; deepen only if reused independently.

## Existing build status vs new standard

The original consolidated 25-unit build matrix records a successful ARM compile/package run for all 25 legacy suite units. That remains useful Layer-A evidence, but it is **not sufficient** for the new pre-handoff gate. The missing work is primarily Layers B–F: production-DSP safety/state tests, deterministic input corpus, control sweeps, delivery assertions, and soak/abuse coverage.

Historical hardware passes also remain valid for those historical binaries. The new protocol changes what must happen **before future changed builds are handed back to DC**, not the historical record.

## Recommended backfill order

1. `LATTICE CORE` — active known hardware failure; highest priority.
2. Active B-level units: `SPECTRA`, `CHORDGHOST`, `DUST`, `CARRIER`, `VECTORFILTER`, `ATTRACTOR`.
3. Active C-level ModFX: `PHASEWELL`, `ASCENDER`, `HELIX`, `CAPSTAN`, `SIDEBAND`, `FAULTLINE`.
4. Active C-level DelFX: `BALLISTIC`, `RAINFALL`, `SWARMDELAY`, `GLITCHREPEAT`, `BUCKETLINE`, `LONGMEMORY`, `SHARD`.
5. Active C-level RevFX: `ABYSS`, `AUREOLE`.
6. Historical/backburner units only when reopened.

## Handoff rule after this audit

Until A-level coverage exists, a newly changed B/C build is **DIAGNOSTIC**, not a normal musical handoff.

Normal handoff target remains:

> **Internal engineering gate: PASS. Remaining physical checks: load/select, one identified worst-case hardware sanity check, and tone/musicality.**
