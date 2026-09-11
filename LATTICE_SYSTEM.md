# Human Soon LATTICE System

Target: **original Korg Nu:Tekt NTS-1 digital kit MkI**.

LATTICE is a generative phrase-and-space system built around one musical behavior:

> **a simple incoming sound should become a deliberate musical response, then memory, then atmosphere — without losing source clarity or collapsing the MkI runtime.**

## Read these first

Before changing LATTICE DSP:

1. **`LATTICE_CURRENT_STATUS.md`** — fastest current-state entry point.
2. **`LATTICE_HISTORY.md`** — project history, failed architectures, and durable engineering lessons.
3. **`LATTICE_FIELD_SPEC.md`** — current FIELD design/implementation guardrails.
4. **`LATTICE_REPORTING.md`** — mandatory reporting protocol.
5. **`reports/lattice/`** — chronological build, CI, diagnosis, and physical MkI reports.
6. **Issue #9** — raw physical MkI QA thread.

`LATTICE_CURRENT_STATUS.md` overrides older status wording in historical sections when they conflict.

---

## Current preferred MkI architecture

```text
AUDIO IN / OSC
      ↓
LATTICE CORE   [custom MOD]
composition + microloops + performance loop/freeze
      ↓
LATTICE FIELD  [custom DELAY]
finite clocked phrase response + pitch/stereo ghosts + spatial bloom
      ↓
optional Korg built-in REVERB only
      ↓
OUTPUT
```

The earlier three-custom-unit CORE + ECHO + SPACE architecture is historical and no longer the preferred MkI system target.

### MkI slot rule

User DELAY and user REVERB share the first-generation SDK memory regions and are not treated as a supported simultaneous pair. Therefore:

- FIELD occupies the user DELAY role.
- Any downstream reverb used simultaneously with FIELD on this MkI should be **built-in Korg reverb**.
- Albedo, as a user REVERB, replaces the user DELAY role rather than coexisting with FIELD.

See `reports/lattice/2026-09-11_sinevibes-comparison-slot-correction.md`.

---

## CORE — composer / looper

**Current implemented version:** `0.3-0`  
**Hardware status:** OPEN / FAIL-RETEST at high TIME.

Current role:

- recent-audio microloop composition;
- structured pitch rules plus rhythm/spacing/stereo variation;
- 16 total microloop voices in the current implementation;
- low TIME includes a disengaged region;
- maximum TIME enters recent-audio freeze/loop behavior.

### Confirmed physical MkI result

CORE alone is clean through much of its range, but distortion appears around the user's approximate **3 o'clock and higher** TIME region. Lowering the incoming source level did **not** remove the distortion.

This weakens a simple input-headroom explanation. Current source cost rises strongly with TIME; a runtime/workload failure is the leading hypothesis, but no measured hardware CPU percentage exists and dense granular playback behavior remains a possible contributor.

A host-side production-DSP probe also confirmed that ordinary microloop scheduling continues during settled freeze even when that work contributes negligibly to the audible result.

### Proposed next diagnostic build — not yet implemented

`CORE 0.3-1` runtime profile, pending approval:

1. cap ordinary microloop playback at **10 active voices**;
2. preserve current gain, patterns, history, loop rules, and FIELD 0.1-2;
3. suspend ordinary microloop scheduling/processing when full freeze is engaged;
4. add automated checks for the voice ceiling, freeze work suspension, state lifetime, and finite/bounded output;
5. physically retest CORE from the reported ~3 o'clock region through maximum, then repeat with FIELD.

Do not describe this as a proven CPU fix until hardware testing passes.

---

## FIELD — response / memory / atmosphere

**Current implemented version:** `0.1-2` (`LatField`, custom `delfx`)  
**Active development branch:** `field-0.1-0-test`  
**Draft PR:** #10  
**Hardware status:** standalone listening PASS for the reported test; overall validation OPEN.

FIELD 0.1-2 supersedes the earlier attack-only capture behavior. It admits fresh attacks plus CLOCK-spaced captures while source input qualifies, maintains two owned captures, protects the first clear answer from replacement, and may retire lower-priority later phrase events under load.

Controls:

- DELAY A / TIME = **CLOCK**
- DELAY B / DEPTH = **PLAYBACK MODE**
- DELAY + B / MIX = **MIX**

Playback modes:

1. FORWARD
2. REVERSE
3. PING-PONG
4. STUTTER

FIELD owns **response, multiplication, memory, and spatial dissolution**.

Its intended arc remains:

```text
clear fragment
   ↓
strong deliberate answer
   ↓
stereo / pitched responses
   ↓
ghosts
   ↓
fat ethereal bloom
   ↓
silence
```

### Confirmed physical MkI observations

- FIELD 0.1-2 standalone: reported as playing exactly as intended. Treat as a listening PASS, not long-duration validation.
- Corrosion + FIELD: reported working cleanly in the tested playing context.
- Tested built-in MOD effects + FIELD: did not show the CORE-specific distortion in the reported tests.
- Albedo + FIELD: not an independent simultaneous pair; consistent with the user DELAY/user REVERB slot-memory restriction.
- CORE + FIELD: FAIL / RETEST; this is currently the supported pair with the strongest reported distortion.

Preserve FIELD 0.1-2 as the current standalone sound-quality reference unless hardware evidence specifically requires a FIELD change.

---

## Historical standalone units

The repository still contains:

- **LATTICE ECHO 0.3-0** — historical standalone custom Delay experiment.
- **LATTICE SPACE 0.3-0** — historical standalone custom Reverb experiment.

Their useful musical ideas were folded into FIELD, but the official MkI system no longer depends on running both as separate heavy custom processors.

---

## Non-negotiable doctrine

### Hardware truth

- Compile/CI success is not physical validation.
- Browser/host probes are diagnostic only.
- Only physical original-MkI testing closes hardware gates.
- Record only observed device behavior; do not invent telemetry.

### Intentionality

- Randomize geometry more readily than harmony.
- Prefer recognizable phrase grammar over unrelated stochastic events.
- More activity should mean more musical behavior, not simply more simultaneous DSP.

### Audibility and sound quality

- Main effect identity must be obvious around useful middle settings.
- Preserve foreground responses before low-priority ghosts.
- Preserve a separate dry/source spine until final summing.
- No avoidable clicks, zippering, DC buildup, runaway resonance, accidental mono collapse, limiter pumping, or cheap pitch artifacts.
- “Experimental” is not permission for degraded audio quality.

### Finite FIELD behavior

- phrases expire;
- no wet self-recapture;
- event-count and absolute-TTL termination remain hard safeguards;
- the processor must return to a genuine rest state.

### Runtime/headroom

- perceived density does not need to equal simultaneous voice count;
- schedule work at event boundaries where possible;
- preserve margin for CORE + FIELD together;
- peak guards are containment, not a substitute for correct gain structure.

### Reporting

Every meaningful architecture, DSP, control, gain, sound-quality, optimization, build, CI, hardware-test, diagnosis, or decision step must add a report under `reports/lattice/` in the same development cycle.

Durable lessons belong in `LATTICE_HISTORY.md`; the fastest current truth belongs in `LATTICE_CURRENT_STATUS.md`.

---

## Current next milestone

Do **not** rebuild FIELD merely because CORE pairing fails.

The next approved decision point is whether to implement the scoped **CORE 0.3-1 runtime diagnostic profile**. If approved, success means:

1. CORE is clean from the reported ~3 o'clock TIME region through maximum on the physical MkI;
2. full freeze no longer performs unnecessary ordinary microloop work;
3. CORE + FIELD 0.1-2 can be pushed without the previously reported distortion collapse;
4. FIELD retains its current standalone sound quality;
5. extended physical MkI stability remains open until explicitly tested.
