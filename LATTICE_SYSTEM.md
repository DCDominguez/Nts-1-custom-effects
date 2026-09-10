# Human Soon LATTICE System

Target: **original Korg Nu:Tekt NTS-1 digital kit MkI**.

LATTICE is a generative phrase-and-space system built around one musical behavior:

> **a simple incoming sound should become a deliberate musical response, then memory, then atmosphere — without losing source clarity or collapsing the MkI runtime.**

## Read these first

Before changing LATTICE DSP:

1. **`LATTICE_HISTORY.md`** — complete project history, physical hardware findings, failed architectures, and engineering lessons.
2. **`LATTICE_FIELD_SPEC.md`** — current next-build specification and hard guardrails.
3. **`LATTICE_REPORTING.md`** — mandatory update-reporting protocol for every meaningful development step.
4. **`reports/lattice/`** — chronological design, build, CI, and physical-hardware update reports.
5. **Issue #9** — raw physical MkI QA notes and open hardware gates.

Commit history records what changed. `LATTICE_HISTORY.md` records why. `reports/lattice/` records each development step as it happened.

---

## Current preferred MkI architecture

The original three-custom-unit architecture has been retired as the preferred system target after physical MkI load-balance testing.

```text
AUDIO IN / OSC
      ↓
LATTICE CORE   [custom MOD]
composition + microloops + performance loop/freeze
      ↓
LATTICE FIELD  [custom DELAY]
finite clocked phrase response + pitch/stereo ghosts + spatial bloom
      ↓
optional Korg internal REVERB
      ↓
OUTPUT
```

### CORE — composer / looper

Current implemented version: **0.3-0**.

Role:

- creates microloop composition from recent audio;
- uses structured pitch rules plus rhythm/spacing/stereo variation;
- uses 16 total microloop voices rather than the previous 32-voice stress design;
- low TIME includes a disengaged region;
- maximum TIME becomes a recent-audio loop/freeze rather than maximum simultaneous density.

CORE owns **composition**.

### FIELD — response / memory / atmosphere

Current status: **specification stage**.

Module target: custom Delay (`delfx`).

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

Its core behavior is:

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

FIELD must use finite seed phrases rather than an endless feedback delay. The spatial bloom is fed by playback events themselves and does not rely on a separate onset detector.

See `LATTICE_FIELD_SPEC.md` for implementation requirements.

---

## Historical standalone units

The following remain in the repository and may still be useful independently:

- **LATTICE ECHO 0.3-0** — standalone clocked custom Delay experiment.
- **LATTICE SPACE 0.3-0** — standalone spatial custom Reverb experiment.

They are no longer the preferred way to assemble the full MkI LATTICE system.

The reason is physical hardware behavior, not compile failure: CORE + ECHO + SPACE can each run, and pairwise combinations are substantially more usable, but aggressive three-unit use repeatedly crossed into distortion/collapse on the target MkI.

---

## Non-negotiable system doctrine

### Repository reporting

- Every meaningful architecture, DSP, control, gain, sound-quality, optimization, build, CI, or physical-test step must add a chronological report under `reports/lattice/` in the same development cycle.
- Follow `LATTICE_REPORTING.md`.
- Reports must distinguish design intent, CI result, inference, and physical MkI observation.
- Durable lessons must also be folded into `LATTICE_HISTORY.md`; current target changes must update this file and/or `LATTICE_FIELD_SPEC.md`.

### Hardware truth

- Compile/CI success is not physical validation.
- Only physical original-MkI testing closes hardware gates.
- Record only observed hardware behavior; do not invent tester telemetry.

### Intentionality

- Randomize geometry more readily than harmony.
- Prefer recognizable phrase grammar over unrelated stochastic events.
- More activity should mean more musical behavior, not merely more gain or more simultaneous DSP.

### Audibility

- The main effect identity must be obvious around useful middle settings.
- First-order foreground events must not become inaudible simply to make room for more ghosts.
- When overloaded, remove low-priority events before sacrificing the main response.

### Source clarity and sound quality

- Preserve a separate dry/source spine until final summing.
- No avoidable clicks, zippering, DC buildup, runaway resonance, unintended mono collapse, limiter pumping, or cheap pitch artifacts.
- “Experimental” is not permission for degraded audio quality.

### Finite behavior

- FIELD phrases must expire.
- No wet self-recapture.
- Use both event-count and absolute-TTL termination.
- The system must always return to a real rest state.

### Runtime/headroom

- Perceived density does not need to equal simultaneous voice count.
- Schedule work at event boundaries where possible.
- Preserve runtime margin for CORE + FIELD together.
- Peak guards are containment, not a substitute for correct gain structure.

---

## Clean-room inspiration

The high-level reference remains musical behavior described in public Hologram Electronics Microcosm documentation: short repeated samples, multiple playback speeds, clocked/rearranged material, delay, looping, pitch movement, and spatial processing.

LATTICE does not copy Microcosm algorithms, firmware, presets, UI, branding, or reverse-engineered behavior. The scheduler, phrase grammars, interval rules, stereo layout, capture logic, and bloom network are original.

---

## Hardware/documentation context

The project owner's physical original NTS-1 MkI reports firmware `N1.2` / v1.20 and panel `P1.0`. That device has audibly run custom ModFX + custom DelFX + custom RevFX together, including explicit BALLISTIC + ABYSS verification on fresh external audio.

Korg's public first-generation SDK documentation nevertheless describes custom Delay and Reverb as sharing a runtime and ordinarily supports only one of those custom processor types at a time when both slots are enabled.

LATTICE therefore does not assume that successful light triple-custom operation guarantees enough margin for a heavy three-stage custom system. The CORE + FIELD pivot is the practical response to repeated physical load-balance failure.

---

## Current next milestone

Implement FIELD from `LATTICE_FIELD_SPEC.md`, then physically establish:

- obvious intentional response at middle MIX;
- finite phrase lifetime with no endless distinct repeats;
- reliable bloom on every audible phrase;
- fat BODY plus airy/ethereal HALO;
- clear dry/source audio and clean foreground echoes;
- no clicks/crunch/dropouts during CLOCK, MODE, or MIX sweeps;
- CORE + FIELD aggressive-use stability;
- extended physical MkI run before hardware validation.
