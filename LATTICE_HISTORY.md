> **MkI slot correction — 2026-09-11:** User DELAY and user REVERB share the SDK SRAM and SDRAM regions and are not a supported simultaneous pair. Earlier CORE + user ECHO + user SPACE proposals are superseded on that basis, not merely by an assumed CPU limit. CORE + FIELD remains a supported slot arrangement requiring physical runtime/headroom validation. Any reverb added alongside FIELD on the same MkI must be built-in; Albedo (user REVERB) replaces the user DELAY role rather than proving coexistence with FIELD. See [Sinevibes comparison and evidence](reports/lattice/2026-09-11_sinevibes-comparison-slot-correction.md).

# Human Soon LATTICE — Project History and Engineering Context

**Repository:** `DCDominguez/Nts-1-custom-effects`  
**Target:** original Korg Nu:Tekt NTS-1 digital kit MkI  
**Working branch:** `lattice-suite`  
**Current architecture:** CORE + FIELD  
**Last updated:** 2026-09-10

---

## Purpose of this document

This file is the narrative source of truth for LATTICE: what the system was trying to do, which architectures were tried, what physical MkI testing actually showed, what failed, what was learned, and why the next build is designed differently.

Future work should read this file before changing LATTICE DSP. Git history records **what changed**; this document records **why**.

Hardware observations are explicitly identified as such. Compile/CI success is never treated as physical hardware validation.

For raw hardware notes and open validation gates, see [Issue #9 — LATTICE M1 physical MkI identity + full-chain QA](https://github.com/DCDominguez/Nts-1-custom-effects/issues/9).

---

# 1. Baseline before LATTICE

The repository already had a broad original-NTS-1 custom-effect suite before LATTICE began.

`HARDWARE_VALIDATION.md` records the project owner's report that all 25 M1 units were physically tested on the original NTS-1 MkI and worked correctly at the suite level. This matters because later LATTICE instability must not be generalized to every custom effect in the repository.

LATTICE was created as a separate system experiment: instead of one isolated effect, the goal was to turn a simple incoming sound into a self-developing musical phrase and spatial field.

---

# 2. LATTICE origin and clean-room intent

The core question was:

> Can one simple incoming tone become a melodic, generative, spatial phrase on the original NTS-1 MkI without requiring a DAW or a much larger processor?

The broad behavioral inspiration came from public descriptions of Hologram Electronics Microcosm: repeated short samples, rearranged playback, pitch relationships, rhythmic sequencing, delay, looping, granular-like density, and spatial processing.

LATTICE is a clean-room design. It does **not** copy Microcosm source code, firmware, presets, UI, branding, parameter maps, or reverse-engineered algorithms. The LATTICE scheduler, pitch rules, event logic, stereo behavior, capture strategy, and spatial networks are original implementations.

The original doctrine was:

```text
simple input
   ↓
composition
   ↓
selective memory / multiplication
   ↓
atmosphere
```

This became three custom units:

- **LATTICE CORE** — `modfx`
- **LATTICE ECHO** — `delfx`
- **LATTICE CLOUD** — `revfx`, later reframed as SPACE

Initial system commit:

- `7814592f857ca0d9f0f8b212848c4ea81de2d3cd` — **Add LATTICE M1 generative FX system**

That commit also introduced the dedicated LATTICE GitHub Actions workflow that builds against a freshly cloned current Korg logue SDK.

---

# 3. Early platform constraints

The target is specifically the **original NTS-1 MkI**, not the MkII.

The current project builds against Korg's `nutekt-digital` target using API `1.1-0` and Cortex-M4 / STM32F446-class toolchain settings.

## 3.1 CORE SDRAM lesson

The first CORE implementation attempted two 32,768-sample floating-point history buffers for stereo capture. That did not fit the MkI ModFX SDRAM allocation.

Commit:

- `52211f665c2e4569bdb7b797c2f7f15236b57af3` — **LATTICE CORE: fit microloop memory in MkI ModFX SDRAM**

The fix used **one mono history buffer**, then created stereo later by independently panning playback voices. This preserved the useful capture window while fitting the target.

Permanent lesson:

> Audio-history memory and audible voice count do not need to scale together. A shared capture history can feed many playback states; CPU, overlap, and summing behavior become the more important limits.

## 3.2 Delay/Reverb runtime caveat

Korg's public first-generation NTS-1 SDK documentation describes custom Delay and Reverb as sharing a runtime and ordinarily only supports one of those custom processor types at a time when both slots are active.

The project owner's physical MkI reports firmware `N1.2` / v1.20 and panel `P1.0`, and that unit has audibly run custom ModFX + custom DelFX + custom RevFX together. A fresh test using BALLISTIC (`delfx`) and ABYSS (`revfx`) on external Chompi audio found both responding independently.

The repository therefore historically treated triple-custom operation as **hardware-observed on this particular unit**, not as a universal MkI guarantee.

Later LATTICE stress tests showed the practical limit of that observation: three lightweight custom units may coexist, while three heavy LATTICE processors do not remain reliable enough to use as the core system architecture.

---

# 4. LATTICE 0.1-0 — first three-stage implementation

## CORE 0.1-0

Role: microloop composer.

Controls:

- TIME = **LOOPS**
- DEPTH = **PATTERN**

Architecture:

- shared recent-audio history
- initially 8 microloop voices
- irregular short loop lengths and capture ages
- windowed playback
- deterministic pitch-rule families
- independent stereo positions

The pitch language included unison, fifths, octaves, falling intervals, and structured patterns such as ORBIT, GLASS, COUNTER, and HUMAN.

Key design principle: pitch should be **rule-based**, not arbitrary random-note generation.

## ECHO 0.1-0

Role: select generated material and turn it into a longer echo field.

Controls:

- TIME = **DISTANCE**
- DEPTH = **PICK**
- SHIFT+DEPTH = **MIX**

Architecture used event/onset selection, short captured fragments, a variable delay line, feedback, and filtering.

## CLOUD 0.1-0

Role: dissolve the generated phrase into ambient space.

Controls:

- TIME = **SPACE**
- DEPTH = **COALESCE**
- SHIFT+DEPTH = **MIX**

Architecture used early reflections feeding a modulated four-line feedback delay network.

## First physical MkI result

The project owner tested all three 0.1-0 units and reported:

- all three were **too subtle**;
- ECHO sounded like roughly **one echo**, not the intended many-arrival field;
- CLOUD/REVERB could **squash the overall mix** as it was turned up, making the source difficult to hear.

Conclusion: 0.1-0 compiled and ran, but **failed the sound-identity gate**.

Permanent rule established here:

> A custom effect that technically runs but cannot be clearly heard doing its intended job is not a successful build.

---

# 5. LATTICE 0.1-1 — character pass

The next pass tried to make each stage unmistakable.

Final version-bump commit in the pass:

- `316b765e1ca06104e8aaf882108912d5c18bca77` — **Bump LATTICE CLOUD to 0.1-1**

Changes across the pass:

### CORE

- louder wet field
- longer repeats
- clearer pitch-pattern identity

### ECHO

- moved toward a **12-tap stereo echo field** rather than one dominant repeat
- retained event selection
- stronger feedback field

### CLOUD

- more dry retained in parallel
- stronger wet injection/feedback
- attempted to eliminate the original mix-squash behavior

The dedicated workflow compiled all units and the bundle successfully. Physical testing remained the authority.

---

# 6. CORE 0.1-2 — 32-voice stress experiment

The project deliberately pushed CORE beyond a conservative design to learn where the concept and hardware broke.

Version commit:

- `3f338ffd01baad751fac227cf415d5430a19f514` — **Bump LATTICE CORE to 0.1-2 for 32-voice stress build**

Changes included:

- maximum simultaneous voice count increased from 8 to 32
- one shared 32,768-sample history retained
- cheaper quartic grain window replaced the sine-derived window
- pan gains and phase increments were precomputed
- primary / secondary / ghost gain hierarchy
- TIME continuously increased simultaneous population toward 32

This was explicitly a **stress test**, not a final architecture.

## Physical MkI result

Issue #9 records the user report:

- around the physical **6–9 o'clock** TIME region, individual loops were audible;
- past roughly **9 o'clock**, CORE collapsed into a **loud steady beep/tone**.

The clock position is only a physical control reference and must not be translated into an exact voice count.

The project interpretation was that many related periodic loops were becoming a coherent tonal mass rather than a richer cloud. This was treated as a **sound-design failure**, not a successful 32-voice hardware pass.

Permanent lesson:

> Population size can be useful, but instantaneous concurrency can be expensive both computationally and musically. More voices can produce *less* perceived complexity when they align into one periodic mass.

---

# 7. Early ECHO and CLOUD hardware lessons

## ECHO instability

The user reported that pushing the delay or moving its controls aggressively could cause:

- sound to disappear completely; or
- the output to turn into crunch/distortion.

No hardware telemetry proved a single root cause, but the continuously moving multi-tap/feedback architecture was considered too fragile to keep.

## CLOUD identity problem

The reverb remained less convincing than the concept required. It did not consistently feel large or integrated enough, and the earlier version had already demonstrated mix-balance problems.

At this stage the project recognized a wider architectural mistake:

> Too much complexity was being consolidated in every stage at once.

A dense CORE feeding a dense ECHO feeding a dense CLOUD becomes a wall. The roles were redefined:

- **CORE = composer**
- **ECHO = rhythmic population expander**
- **SPACE = environment**

---

# 8. LATTICE 0.2 — role separation

Documentation head:

- `a7aa1f599ccd80fa17349232c52712d0f0e6091b` — **Document LATTICE 0.2 composer clocked echo and space architecture**

Dedicated workflow run:

- `34391516695` — CORE, ECHO, SPACE, and bundle built successfully against a fresh Korg logue SDK clone.

## CORE 0.2

The system retained a **32-state ecosystem**, but only up to **16 voices were audible simultaneously**.

Higher ACTIVITY increasingly varied:

- capture age
- loop length
- entry wait
- lifetime
- stereo position

PATTERN remained deterministic while timing/geometry became more varied.

Permanent principle:

> Randomize geometry; preserve musical intent.

## ECHO 0.2

The moving multi-tap design was retired.

Controls became:

- TIME = **CLOCK**
- DEPTH = **PLAYBACK MODE**
- SHIFT+DEPTH = **MIX**

Clock divisions:

`1/32 → 1/16T → 1/16 → 1/8T → 1/8 → 1/4T → 1/4 → 1/2`

Playback modes:

- FORWARD
- REVERSE
- PING-PONG
- STUTTER

The design captured short fragments and scheduled future playback events from host BPM rather than moving live delay heads when TIME changed.

## SPACE 0.2

CLOUD was simplified into SPACE.

Controls:

- TIME = **SPACE**
- DEPTH = **DRIFT**
- SHIFT+DEPTH = **MIX**

The job became a dark, slowly moving stereo environment rather than another note-generating processor.

## Why 0.2 still did not solve the system

Physical use showed that total headroom/runtime remained precarious. The user could often run two LATTICE units much harder than all three. Some other custom effects could coexist at high settings with two LATTICE units, while certain other FX combinations could also provoke collapse.

This suggested that the problem was not simply one broken module. Total chain energy and total runtime cost both mattered.

---

# 9. LATTICE 0.3-0 — headroom/stability redesign

The next build traded brute-force population for modulation, clearer effect identity, and guarded gain.

Relevant commits:

- `2f59dc10f4c9cbfd371df850771d42c338fbaa39` — **Bump LATTICE CORE to 0.3-0**
- `ef51fdcda44e45e0220620af75f4f1e18cb52d82` — **Bump LATTICE ECHO to 0.3-0**
- `799791aa02013e2b03d9376a74e6232e52e89531` — **Thicken LATTICE SPACE 0.3 with diffusion and guarded gain**
- `b379f680655ffc99b72cd86494e773aedb084665` — **Bump LATTICE SPACE to 0.3-0**

Dedicated workflow run:

- `34444893419` — CORE, ECHO, SPACE, and bundle built successfully.

## CORE 0.3

- reduced to **16 total microloop voices**
- more variation through pitch, rhythm, capture age, loop length, lifetime, phase, and stereo placement
- TIME minimum became a true disengaged region
- TIME maximum became a **freeze/loop state** using recent capture rather than maximum microloop density
- history changed to 65,536 signed 16-bit mono samples, roughly 1.36 s at the implementation's 48 kHz assumption
- freeze uses up to roughly 1.25 s with wrap crossfade
- stage peak guard added around 0.86 full scale before final soft limiting

CORE's intended performance arc became:

```text
bypass / dry
   ↓
small microloop population
   ↓
more pitch/rhythm/space modulation
   ↓
dense but bounded microloop field
   ↓
recent-audio loop / freeze
```

## ECHO 0.3

- reduced to **16 playback voices**
- dynamic voice ceiling according to CLOCK division
- fast clocks use fewer simultaneous voices
- STUTTER uses a stricter budget
- wet path made substantially louder because previous ECHO behavior was difficult to hear
- dry dominance reduced at high MIX
- output peak guard added

## SPACE 0.3

- compact two-stage diffusion before a four-line FDN
- reduced memory sizes
- stronger early/late injection and low-mid body
- maximum feedback reduced to approximately 0.81
- cheap triangle-style drift modulation replaced multiple sine-LFO reads
- output peak guard added

The sonic request had shifted from simply “more reverb” to **fatter space**: a room that owns the space while remaining controlled.

---

# 10. 0.3 physical result — three-unit architecture rejected

Physical original-MkI testing still could not reliably load-balance **CORE + ECHO + SPACE** together.

User report:

- pairwise combinations were much more usable;
- with all three LATTICE custom units active, pushing controls could still cross a point where the whole sound collapsed into distortion;
- some other modules could be pushed very hard while only two LATTICE units were active;
- some other demanding FX combinations could reproduce similar failure behavior.

Issue #9 records the resulting decision: stop making the official MkI LATTICE system depend on three independent heavy custom processors.

This is the main architecture pivot.

---

# 11. Architecture pivot — CORE + FIELD

The preferred MkI LATTICE system is now **two custom units**:

```text
AUDIO IN / OSC
      ↓
LATTICE CORE   [MOD]
      ↓
LATTICE FIELD  [DELAY]
      ↓
optional Korg internal REVERB
      ↓
OUTPUT
```

Standalone ECHO and SPACE remain useful repository modules and experiments, but they are no longer the preferred combined MkI system architecture.

FIELD merges the useful musical functions of ECHO and SPACE without running two independent heavyweight processors.

Its job is not “delay plus reverb.” Its job is:

> hear a fragment, answer it deliberately, and let the answer evaporate into space.

The spatial bloom is produced from playback events themselves rather than by a separately triggered reverb stage. This both reduces duplicated runtime and prevents the old perception that the reverb sometimes failed to trigger.

---

# 12. Current FIELD design doctrine

The implementation specification lives in `LATTICE_FIELD_SPEC.md`.

The most important decisions are repeated here so future spec revisions cannot erase the historical reason for them.

## 12.1 Intentionality over randomness

FIELD must sound **on purpose**.

Each accepted capture becomes a finite **seed phrase** with recognizable rhythmic grammar. Randomness is restricted to small variation inside that grammar.

Allowed variation includes:

- slight timing offset
- bounded pan movement
- ghost presence/omission
- small lifetime variation
- permitted harmonic substitutions

Do not independently randomize timing, pitch, direction, gain, pan, and lifetime all at once. That reads as algorithmic accident rather than composition.

## 12.2 Finite phrase engine

Earlier delay behavior could feel endless because the processor could keep accepting new captures while older fragments still had generations remaining. Individual captures could be finite while the system as a whole continuously replenished itself.

FIELD therefore requires:

- **maximum two active seeds**
- finite event count per seed
- independent absolute clock/sample TTL
- explicit retirement/fade state
- no wet-output self-recapture
- no conventional infinite rhythmic-feedback loop

Endless distinct rhythmic repeats should be structurally impossible.

## 12.3 Playback controls

- TIME = **CLOCK**
- DEPTH = **PLAYBACK MODE**
- SHIFT+DEPTH = **MIX**

Modes:

1. FORWARD
2. REVERSE
3. PING-PONG
4. STUTTER

Each mode should use an identifiable phrase grammar, not merely a small technical variant of the same delay.

## 12.4 Pitch language

Most important events stay at unison so the source remains intelligible.

Sparse designated ghosts may use:

- 0 semitones
- +7 semitones
- +12 semitones
- optional low-level -12 semitone shadow

Pitch is part of phrase grammar, not a broad random pitch generator.

## 12.5 Ethereal tonal target

The desired FIELD sound is **ethereal, deliberate, floating, glassy, and spatial**, but not thin or washed out.

Tonal model:

- **BODY** — warm low-mid physical density
- **HALO** — quieter, wider, lighter upper component with selected fifth/octave ghosts

The phrase should evolve like this:

```text
clear fragment
   ↓
strong answer
   ↓
stereo/pitched responses
   ↓
ghosts
   ↓
bloom / halo
   ↓
silence
```

## 12.6 Bloom reliability

FIELD bloom must **not have its own onset trigger**.

Every audible playback event produces a defined bloom send. Later generations send progressively more energy to the bloom network.

Therefore:

> if there is an audible FIELD playback event, there is corresponding spatial energy.

A tiny direct-input room feed may be used for continuity, but bloom must not depend on a second trigger detector that can miss events.

## 12.7 Audibility is a feature requirement

Earlier ECHO versions were too hard to hear. FIELD must not repeat that mistake.

At approximately middle MIX:

- the first response must be immediately obvious;
- the second response must still be clearly audible;
- the phrase must not require maximum MIX to reveal its identity.

The preferred solution is **fewer, louder, hierarchically important events**, not simply increasing the entire wet bus.

When density threatens audibility or headroom, drop ghost/tail events before turning the main response into background noise.

## 12.8 Source clarity and audio quality

The dry/source path remains separate until final summing.

Non-negotiable quality requirements:

- no avoidable clicks at capture, reverse, stutter, replacement, or retirement boundaries
- smooth MIX changes
- pitch-shift/interpolated playback clean enough that harmony reads as intentional
- no accidental mono collapse
- no severe summed-mono cancellation of the source or first response
- no DC or uncontrolled sub-bass accumulation in regenerative paths
- no resonance that grows generation after generation
- no limiter/peak-guard pumping during ordinary material
- no wet-path overload that randomly buries or flattens the dry source

A deliberately experimental effect is still expected to sound like a **high-quality processor**.

## 12.9 Spatial narrative

Bloom rises with phrase age:

```text
first answer    = clear, low bloom
second answer   = clear, slightly more space
middle events   = balanced response + bloom
late events     = lower clear level, high bloom
final ghosts    = mostly atmosphere
then silence
```

This creates the intended narrative:

> **statement → response → memory → atmosphere**

---

# 13. Global engineering guardrails learned from LATTICE

## Hardware truth

- Compile success is not hardware success.
- CI success is not hardware success.
- Browser/WebAudio models are not hardware success.
- Only physical original-MkI testing closes hardware gates.
- Record only what the user actually reports from the device.

## Gain structure

- Do not make every stage nearly full-scale and expect the final limiter to solve it.
- Preserve headroom before summing.
- Normalize according to **actual active events**.
- Important foreground events may be loud; low-priority ghosts are sacrificed first when energy becomes excessive.
- Peak guards are emergency containment, not the sonic engine.

## CPU/runtime

- Perceived event count does not need to equal simultaneous DSP voice count.
- Schedule work at event boundaries wherever possible.
- Avoid expensive audio-rate operations when cheaper perceptually equivalent methods exist.
- Faster clocks should automatically use lower simultaneous voice budgets.
- Preserve margin for CORE + FIELD running together.

## Musical intentionality

- Randomize timing/geometry more readily than harmony.
- Create recognizable event hierarchies.
- Use finite phrases and explicit rest states.
- More activity means “more of this effect's musical behavior,” not merely more gain or more simultaneous processes.

## Audio quality

- Source intelligibility is a requirement.
- First-order effect identity must be audible without extreme settings.
- No clicks, zippering, runaway feedback, stuck buffers, accidental clipping, or distortion collapse may be accepted as “character.”
- If a transformation sounds degraded rather than musical, omit or redesign it.

---

# 14. Current status

## CORE

Current implemented version: **0.3-0**.

Direction:

- 16 total microloop voices
- modulation emphasis on pitch, rhythm, spacing, lifetime, and stereo position
- true disengaged low-TIME region
- maximum TIME becomes recent-audio loop/freeze rather than maximum density

CORE remains the LATTICE **composer** and performance looper.

## ECHO

Current standalone version: **0.3-0**.

Status:

- useful standalone experiment
- not the final official system processor
- historical problems included weak audibility, effectively endless phrase populations, and insufficiently intentional behavior

Its useful lessons are being absorbed into FIELD.

## SPACE

Current standalone version: **0.3-0**.

Status:

- useful standalone spatial experiment
- not the final official system processor
- historical problems included insufficient spatial ownership, unreliable perceived triggering/identity, and contribution to the heavy three-custom-unit architecture

Its useful diffusion/body/drift ideas are being absorbed into FIELD.

## FIELD

Current status when this history was created: **specification stage, not yet implemented or hardware validated**.

Current spec: `LATTICE_FIELD_SPEC.md`.

---

# 15. Next-build success definition

The next FIELD build does not pass until physical MkI testing confirms:

1. one note produces an obvious deliberate response around middle MIX;
2. FORWARD, REVERSE, PING-PONG, and STUTTER clearly differ;
3. first and second responses remain intelligible against the source;
4. pitch ghosts sound musical rather than artifact-heavy;
5. every seed stops producing distinct rhythmic repeats after its finite phrase life;
6. wet output cannot continuously recapture itself;
7. every audible phrase produces audible bloom;
8. bloom becomes stronger later in the phrase instead of masking the first response;
9. BODY feels full while HALO feels airy/ethereal;
10. the source remains clear and stereo-stable;
11. MIX/CLOCK/MODE sweeps do not click, crunch, disappear, or enter permanent distortion;
12. CORE + FIELD can be used aggressively together without the previous three-unit collapse behavior;
13. the system returns to a genuine rest state after input and bloom decay;
14. CORE + FIELD remains stable for an extended physical run before being called hardware validated.

---

# 16. Key repository references

- `HARDWARE_VALIDATION.md` — baseline M1 physical hardware record
- `LATTICE_SYSTEM.md` — concise current system overview
- `LATTICE_FIELD_SPEC.md` — current detailed FIELD design specification
- `effects/lattice-core/` — CORE implementation and QA
- `effects/lattice-echo/` — standalone ECHO implementation and QA
- `effects/lattice-cloud/` — historical CLOUD / standalone SPACE implementation and QA
- `.github/workflows/build-lattice-suite.yml` — dedicated current-SDK CI path
- [Issue #9](https://github.com/DCDominguez/Nts-1-custom-effects/issues/9) — hardware feedback and open validation gates

---

# 17. Short context for future agents

> LATTICE started as three heavy custom processors: CORE composed microloops, ECHO multiplied them, and CLOUD/SPACE turned them into atmosphere. Physical MkI testing repeatedly showed that each stage could work alone or in pairs, but all three together were too difficult to keep stable under aggressive settings. Earlier builds also exposed subtler failures: CORE's 32 simultaneous voices collapsed into a steady tone, ECHO was initially too subtle and could behave as though it repeated forever, and the separate reverb did not consistently own the space. The architecture therefore pivoted to **CORE + FIELD**. CORE remains a 16-voice microloop composer/looper. FIELD will be a finite two-seed clocked phrase engine that gives loud, recognizable answers, uses sparse structured pitch ghosts, and progressively sends later events into a fat ethereal bloom. FIELD must never self-recapture, must have event-count and absolute-TTL kill mechanisms, must keep the dry source intelligible, and must prioritize high-quality audio over maximum event count. The design target is not random cloud generation; it is **statement → response → memory → atmosphere → silence**.


# 18. FIELD 0.1-0 implementation cycle — 2026-09-10

DC approved the repository review's six proposals: explicit phrase admission, separate event/voice budgets (8 normal/4 fastest), protected capture ownership, foreground-first level budgets, immutable expiry, and one shared BODY/HALO bloom. DC requested a pause before adopting any newly discovered improvement beyond that approved scope.

FIELD's first implementation uses six deterministic responses per seed and preserves stereo capture. Fresh attacks or release/rearm admit input; the old timed sustained-input fallback is absent. CLOCK/MODE/tempo latch per seed; expiry cannot grow with a knob change. Retiring reads drain before capture storage can be reused. The raw incoming signal here includes CORE's output, so an upstream frozen loop may qualify as new input repeatedly.

CORE is intentionally retained at 0.3-0 for the first A/B hardware comparison. The already identified freeze-release and full-freeze runtime corrections remain a separately versioned follow-up. Historical ECHO/SPACE binaries are not the preferred FIELD package.

See `reports/lattice/2026-09-10_field-0.1-0-build.md` for actual test/build evidence and `LATTICE_FIELD_SPEC.md` section 22 for exact implementation constants. No new physical hardware result is claimed here.


# 19. FIELD 0.1-1 — approved foreground level retest

DC clarified that isolated and subsequent notes did produce audible delay, but it was faint and remained below the desired prominence following the higher-MIX check. Earlier assumptions of a missing trigger were superseded by that correction. The proposed capture redesign was put on hold.

DC approved a focused +6 dB boost to the four main echoes, retaining pitched ghost levels, bloom sends and finite-phrase behavior. FIELD 0.1-1 applies this boost only in the clear foreground sum before existing wet protection. CORE remains 0.3-0. See the 0.1-1 build/result reports for test evidence. Hardware acceptance is still open.

## 2026-09-11 — response delivery before hardware handoff
The 0.1-1 audit reproduced replacement starvation: 24 captures but no echoes during six seconds of busy input at slow CLOCK. DC approved clocked recapture and first-answer protection, plus the three-layer test protocol. The durable lesson is to test audible response delivery, not admission counts alone. With two capture buffers and long delay intervals, busy input must be selectively skipped to preserve an admitted first answer. Other audit findings remain explicitly deferred.
