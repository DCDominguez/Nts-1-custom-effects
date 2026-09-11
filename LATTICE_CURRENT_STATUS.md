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

The full 29-unit suite, including CORE, FIELD, historical ECHO and historical SPACE, passes the repository's A-class engineering gate on candidate `5718d12416a242137892688e240e8ac41be5a0ca`:

- 29/29 shared production-DSP common gate PASS;
- 29/29 project-specific A-class harness PASS;
- 29/29 fresh ARM build/package PASS;
- final consolidated workflow `34613566166`.

A-class is engineering coverage only. Physical MkI findings below remain stronger evidence for actual runtime/tone behavior.

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

**Version:** `0.3-0`  
**Physical status:** **PASS WITH NOTES / RETEST**.

Latest physical MkI observation:

- CORE loads and sounds good;
- at maximum TIME it loops the note using the current freeze/loop behavior;
- intermittent distortion still appears at or near maximum TIME;
- CORE can also distort when paired with another effect, although some combinations/runs are clean.

This refines the earlier blanket high-TIME failure description. CORE is not always unusable at high TIME; instead, extreme TIME and some multi-effect conditions remain intermittently unstable.

The A-class host harness passes and does not reproduce a fatal numerical/state defect. No measured device CPU percentage exists, so runtime/workload remains a hypothesis rather than a proven cause.

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
- SPACE is a redesign candidate;
- CORE keeps its sound/loop identity while the intermittent extreme-TIME/combination distortion is isolated.

## Next work

1. Add reproducible level/loudness instrumentation to the pre-handoff suite so under-level/over-level candidates can be detected before physical handoff.
2. Keep FIELD unchanged unless later hardware evidence requires a change.
3. Rework SPACE for stronger standalone presence plus combination-safe gain structure.
4. Continue CORE maximum-TIME and multi-effect fault isolation without assuming a measured CPU cause.
5. Re-run full A-class host/build gates for every changed candidate before physical MkI handoff.

See `reports/lattice/2026-09-12_a-class-rc-lattice-hardware.md` for the latest detailed physical report.
