# Human Soon LATTICE — Current Status

**Date:** 2026-09-12  
**Target:** original Korg Nu:Tekt NTS-1 digital kit MkI  
**Active development branch:** `field-0.1-0-test`  
**Stable/reference branch:** `lattice-suite`

This is the fastest current-state entry point. For chronological evidence, read `reports/lattice/`; for historical reasoning, read `LATTICE_HISTORY.md`.

## Current architecture

Preferred MkI system:

```text
AUDIO IN / OSC
      ↓
LATTICE CORE   [custom MOD]
      ↓
LATTICE FIELD  [custom DELAY]
      ↓
optional Korg built-in REVERB only
      ↓
OUTPUT
```

User DELAY and user REVERB are not treated as a supported independent simultaneous pair on MkI because they share first-generation SDK memory regions.

## CORE

**Version:** `0.3-1`  
**Engineering status:** **A-CLASS / ARM PASS**  
**Physical status:** **PASS for reported focused playing test**  
**CORE + FIELD 0.1-2:** **PASS for reported focused combination test**.

CORE 0.3-1 is the current MkI reference.

Changes from 0.3-0:

1. ordinary microloop ceiling reduced from 16 to 10 voices;
2. full freeze suspends ordinary microloop scheduling, reads, phase advancement and repeat/wait state advancement;
3. history format/length, patterns, pitch rules, loop rules, wet-drive formula, freeze path, guard and limiter were preserved.

The exact candidate passed workflow `34628866540` and then passed the requested original-NTS-1 MkI checks at upper/max TIME both standalone and with FIELD 0.1-2.

This supports keeping the reduced-workload profile but does not prove a measured CPU cause; no MkI cycle/CPU telemetry exists.

See:

- `reports/lattice/2026-09-12_core-0.3-1-runtime-profile.md`
- `reports/lattice/2026-09-12_core-0.3-1-physical-pass.md`

## FIELD

**Version:** `0.1-2` (`LatField`, custom `delfx`)  
**Physical status:** **LOAD / RUNTIME / MUSICAL PASS for reported playing tests**.

FIELD remains the preferred delay/spatial stage for the MkI LATTICE architecture and passed the latest focused pairing with CORE 0.3-1.

Preserve FIELD's current sound unless later physical evidence requires a change.

## SPACE

**Version:** `0.4-1` (`LatSpace`, custom `revfx`)  
**Engineering status:** **A-CLASS / ARM PASS**  
**Physical status:** **PASS WITH NOTE**  
**Current MkI reference:** **0.4-1**.

### Physical result

The prior 0.4-0 build still distorted after modulation, including CORE and other modulation effects. 0.4-1 removed the sample-by-sample output guard, reduced regenerative drive for more headroom, preserved wet presence at readout, and added hidden-clipping counters to the A-class harness.

The exact 0.4-1 binary then received the physical MkI report:

> “MUCH better. works great now. just some minor minor minor distortion on the top but this is great”

Disposition:

- standalone/general musical result: **PASS for the reported playing test**;
- ModFX → SPACE behavior: **PASS WITH NOTE**;
- the broad modulation-combination distortion from 0.4-0 is materially improved;
- a very small amount of distortion remains near the top end of the tested range;
- preserve 0.4-1 rather than immediately reworking the topology again.

Exact binary SHA-256: `56fd5b263920831af7298839d742482b837408795b2f29aa0232b5ef37a80b56`  
Pre-handoff workflow: `34637349268`.

Engineering evidence for 0.4-1:

- shared production-DSP common gate: **PASS**;
- full project-specific A-class harness run: **PASS**;
- CORE 0.3-1 → SPACE hidden-clipping test: **PASS**;
- IRONROT → SPACE hidden-clipping test: **PASS**;
- fresh ARM build/package against a current official Korg logue SDK clone: **PASS**.

The physical improvement is consistent with the 0.4-1 headroom/guard changes being directionally correct, but it does not prove which specific change was causal or establish measured MkI deadline margin.

See:

- `reports/lattice/2026-09-12_space-0.4-1-guard-isolation.md`
- `reports/lattice/2026-09-12_space-0.4-1-physical-pass.md`

## ECHO

**Version:** historical `0.3-0` custom `delfx`  
**Physical status:** **LOAD / RUNTIME / MUSICAL PASS for latest reported playing test**.

ECHO remains historical rather than the preferred LATTICE delay stage.

## Current doctrine

- each LATTICE processor must have obvious standalone musical identity;
- multi-effect safety should come from correct local gain structure and bounded runtime/state behavior, not blanket attenuation;
- CORE 0.3-1 and FIELD 0.1-2 are the current preferred MkI pair;
- SPACE 0.4-1 is now the current MkI SPACE reference and is usable after modulation with only a minor residual top-end artifact reported;
- physical MkI evidence outranks host-model assumptions;
- do not infer support for simultaneous user DELAY + user REVERB on original MkI.

## Next gate

LATTICE CORE, FIELD and SPACE are now good enough to freeze for this MkI pass. Return to the suite-wide calibration backlog: delay/reverb perceived level, subtle ModFX identity, and IRONROT loudness consistency. Revisit SPACE only if the residual top-end distortion becomes musically significant in later use.
