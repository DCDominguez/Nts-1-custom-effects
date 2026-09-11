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

## Engineering status

The full 29-unit suite passed the repository's A-class engineering gate on the original release candidate, and the changed CORE 0.3-1 candidate has now re-passed the exact-build gates:

- Pre-handoff workflow `34628866540`;
- shared production-DSP common gate: **PASS**;
- full project-specific A-class harness run: **PASS**;
- fresh ARM build/package: **PASS**;
- ARM artifact `10275063747`, SHA-256 `22378a8e266a14a0d8bad999154e313fc6fe61e2142eeb827581bef123555503`;
- extracted CORE 0.3-1 binary SHA-256 `1d91d643775e26bb698cd24c0980109561ebfbc18c5811c19c15d067ddaddaee`.

A-class is engineering coverage only. Physical MkI findings remain stronger evidence for actual runtime/tone behavior.

## FIELD

**Version:** `0.1-2` (`LatField`, custom `delfx`)  
**Physical status:** **LOAD PASS / RUNTIME PASS / MUSICAL PASS for the latest reported playing test**.

FIELD remains the preferred delay/spatial stage for the current MkI LATTICE architecture. No new issue was reported during the 2026-09-12 A-class release-candidate physical pass.

Previously confirmed:

- standalone listening behavior: plays as intended;
- Corrosion + FIELD: clean in the tested playing context;
- Albedo + FIELD is not treated as an independent simultaneous user-REVERB + user-DELAY pair on MkI.

Preserve FIELD's current sound unless later physical evidence requires a change.

## CORE

**Implemented candidate:** `0.3-1`  
**Previous physically tested version:** `0.3-0`  
**0.3-1 engineering status:** **A-CLASS / ARM PASS**  
**0.3-1 physical status:** **RETEST REQUIRED**.

Latest physical MkI observation on 0.3-0:

- CORE loads and sounds good;
- at maximum TIME it loops the note using the current freeze/loop behavior;
- intermittent distortion still appears at or near maximum TIME;
- CORE can also distort when paired with another effect, although some combinations/runs are clean.

This refines the earlier blanket high-TIME failure description. CORE is not always unusable at high TIME; instead, extreme TIME and some multi-effect conditions remain intermittently unstable.

### CORE 0.3-1 runtime diagnostic

The scoped runtime profile is implemented and has passed host/build validation:

1. ordinary microloop ceiling reduced from **16 to 10 voices**;
2. during full freeze, ordinary microloop scheduling, reads, phase advancement and repeat/wait state advancement are suspended;
3. history format/length, patterns, pitch rules, loop rules, wet-drive formula, freeze path, guard and limiter are preserved;
4. the A-class harness explicitly checks the ten-voice ceiling and that ordinary microloop state does not advance during freeze.

This is a runtime/workload diagnostic, not proof that CPU overload caused the physical distortion. There is still no measured MkI CPU percentage or ARM cycle telemetry.

See `reports/lattice/2026-09-12_core-0.3-1-runtime-profile.md`.

## SPACE

**Version:** historical `0.3-0` custom `revfx`  
**Physical status:** **FAIL / REDESIGN CANDIDATE for musical quality/combination robustness**.

Latest physical MkI observation:

- loads and produces the intended echo/spatial behavior;
- remains very subtle even at full MIX;
- distorts when run with modulation in the reported test;
- the effect remains audible underneath the distorted sound quality.

SPACE should not receive a simple global gain boost. It needs a rework/revoice that improves standalone identity while maintaining safe local gain/headroom in combinations.

SPACE remains historical and is not part of the preferred CORE → FIELD architecture.

Per current project priority, **SPACE work begins after CORE 0.3-1 is physically retested**.

## ECHO

**Version:** historical `0.3-0` custom `delfx`  
**Physical status:** **LOAD PASS / RUNTIME PASS / MUSICAL PASS for the latest reported playing test**.

No new problem was reported in the 2026-09-12 A-class release-candidate physical pass. ECHO remains historical rather than the preferred LATTICE delay stage.

## System-level physical findings

Most modulation + delay/reverb pairings were reported to work well. The main LATTICE exception remains CORE, which can distort intermittently in combinations.

Do not solve these findings by globally lowering every LATTICE processor. The user requirement is that each effect should stand strongly on its own.

Current doctrine:

- standalone identity must be obvious;
- multi-effect safety comes from correct local gain structure, bounded regenerative paths and efficient runtime behavior;
- FIELD is preserved as the current preferred delay reference;
- CORE keeps its sound/loop identity while the intermittent extreme-TIME/combination distortion is isolated;
- SPACE is next for redesign after the CORE result is understood.

## Next work

1. Physically retest CORE 0.3-1 alone from upper TIME through maximum, preserving the liked loop behavior.
2. Physically retest CORE 0.3-1 + FIELD 0.1-2 at upper/max TIME.
3. If CORE is clean, record/freeze the MkI result. If not, investigate granular playback/runtime structure rather than globally lowering FIELD or CORE.
4. Then rework SPACE for stronger standalone presence plus combination-safe gain structure.
5. Resume suite-wide level/loudness calibration after the CORE → SPACE priority work.

See `reports/lattice/2026-09-12_a-class-rc-lattice-hardware.md` and `reports/lattice/2026-09-12_core-0.3-1-runtime-profile.md`.
