# LATTICE FIELD — Next Build Specification

**Project:** Human Soon Studio / NTS-1 Custom Effects  
**Target hardware:** original Korg Nu:Tekt NTS-1 digital kit MkI  
**Module target:** custom Delay (`delfx`)  
**Status:** FIELD 0.1-0 implementation candidate; hardware validation OPEN
**System role:** paired with LATTICE CORE in MOD; FIELD occupies DELAY. Optional Korg internal reverb may be used downstream.

Read `LATTICE_HISTORY.md` before implementation. It records why these constraints exist.

---

## 1. Product definition

LATTICE FIELD is a **finite, clocked phrase-and-space processor**. It captures short fragments from the incoming signal, turns each capture into a deliberate musical response, then progressively dissolves that response into an ethereal stereo bloom.

The effect must sound **intentional, obvious, finite, musical, and high-quality** rather than random, subtle, degraded, or endlessly regenerative.

Core sonic arc:

> **clear fragment → deliberate response → pitched/stereo ghosts → diffuse bloom → silence**

FIELD is not a conventional infinite-feedback delay and is not a delay with a separately triggered reverb bolted on.

---

## 2. Primary design goals

1. **Intentionality** — each capture produces a recognizable phrase grammar rather than loosely randomized repeats.
2. **Audibility** — the effect is clearly identifiable around middle MIX; the first two responses are strong.
3. **Finite behavior** — every seed expires. Endless rhythmic delay behavior is structurally impossible.
4. **Ethereal character** — later events become lighter, wider, more pitch-coloured, and more diffuse.
5. **Reliable bloom** — bloom is continuously fed by playback events and does not depend on a second onset detector.
6. **Headroom safety** — generous effect loudness inside a bounded gain structure; no collapse/clipping.
7. **Source clarity** — the original phrase remains intelligible and stable unless MIX is deliberately near fully wet.
8. **Audio fidelity** — no avoidable clicks, zippering, brittle pitch artifacts, DC buildup, stereo collapse, or limiter-driven sound.
9. **MkI efficiency** — perceived density comes from scheduling, stereo placement, pitch hierarchy, and diffusion rather than brute-force voice count.
10. **Clear system roles** — CORE composes/microloops; FIELD responds, multiplies, and dissolves.

---

## 3. NTS-1 MkI control mapping

Follow original NTS-1 panel nomenclature and logue SDK delay parameters.

| NTS-1 control | SDK parameter | FIELD function |
|---|---|---|
| DELAY **A / TIME** | `k_user_delfx_param_time` | CLOCK / rhythmic subdivision |
| DELAY **B / DEPTH** | `k_user_delfx_param_depth` | PLAYBACK MODE |
| DELAY + **B / MIX** | `k_user_delfx_param_shift_depth` | DRY/WET MIX |

### TIME / CLOCK

Stepped divisions:

`1/32 → 1/16T → 1/16 → 1/8T → 1/8 → 1/4T → 1/4 → 1/2`

Clock changes latch at safe phrase/event boundaries. Do not drag active read heads through memory when TIME moves.

Use host BPM from the logue SDK effect runtime. If BPM is unusable, fall back to a conservative internal tempo.

### DEPTH / PLAYBACK MODE

Four strong stepped zones:

1. **FORWARD**
2. **REVERSE**
3. **PING-PONG**
4. **STUTTER**

Mode identity must be obvious. Continuous morphing between modes is not required.

### MIX

- Low: source dominates, but the first response is still audible.
- Mid: FIELD identity is unmistakable.
- High: phrase dominates and increasingly dissolves into bloom.
- Max: very wet and ethereal, but still finite, stable, and intelligible enough to sound deliberate.

---

## 4. Signal flow

```text
INPUT
  │
  ├──────────────────────────────────────── dry spine
  │
  ▼
source-only seed detector
  │
short fragment capture
  │
2-seed phrase scheduler
  │
clocked playback grammar
  │
├── foreground response
├── stereo choreography
├── constrained pitch ghosts
└── generation envelope
  │
  ├──────────────────────────── clear echo path
  │
  └── generation-dependent bloom send
          │
          ▼
     short diffusion
          │
     compact stereo feedback field
          │
     warm BODY + airy HALO
          │
          ▼
        bloom
          │
          └──────────────────── wet summing
                                  │
                                  ▼
                           output protection
                                  │
                                  ▼
                                OUTPUT
```

---

## 5. Seed model

A **seed** is one captured dry/source fragment plus its scheduled phrase state.

### Maximum active seeds

**2 simultaneous seeds.**

When a third qualifying source event arrives, the oldest seed enters immediate final decay/retirement while the new seed is admitted.

### Source-only capture

The seed detector listens only to **incoming dry/source audio**.

FIELD playback and bloom must never be recaptured as new source events.

### Lifetime

Each seed has two independent expiry systems:

1. finite event/generation count;
2. absolute clock/sample TTL.

If either expires, no new event may be scheduled from that seed. Existing sound fades cleanly and the seed becomes invalid.

Target: approximately **4–8 major phrase events** plus optional low-level ghosts.

No conventional infinite rhythmic feedback loop.

---

## 6. Phrase grammar

The phrase is selected by PLAYBACK MODE and must remain recognizable across captures.

Randomness is constrained to small timing offsets, bounded pan variation, ghost presence, small lifetime differences, and permitted harmonic substitutions.

### FORWARD

Character: clear answer followed by receding fragments.

Example 8-step grammar:

`X . X x . X . g`

### REVERSE

Character: distinct backward answers with more space between important events.

Example:

`X . . R . r . g`

### PING-PONG

Character: deliberate spatial conversation, not mechanical L/R feedback.

Example:

`L . R . L x . R`

### STUTTER

Character: rhythmic retrigger phrase rather than uncontrolled machine-gun feedback.

Example:

`X X X . X . x .`

Stutter uses a hard retrigger cap and short windowed slices.

Legend:

- `X` = foreground event
- `x` = secondary event
- `g` = low-level ghost
- `R/r` = reverse event
- `L/R` = structural stereo role

---

## 7. Pitch language

Pitch is **structured and sparse**.

Most foreground events remain unison to keep the source intelligible.

Allowed designated ghost intervals:

- `0` semitones
- `+7` semitones
- `+12` semitones
- optional low-level `-12` semitone shadow

Example motifs:

`0 → 0 → +7 → 0 → +12`

`0 → +12 → 0 → +7`

Pitch choice is part of phrase grammar, not fully random per event.

Pitch-shifted playback must use clean interpolated reads and shaped fragment edges. If a transformation sounds degraded rather than musical on hardware, omit it rather than forcing it into the phrase.

---

## 8. Rhythm and spacing

CLOCK defines the skeleton. Variation makes the result feel alive without destroying the beat.

Permitted variation:

- small event timing offset around the grid
- slight fragment-length variation
- selective ghost omission/addition
- controlled spacing differences between generations

Avoid unconstrained timing randomization.

The same gesture should produce recognizably related behavior on repeated plays.

---

## 9. Stereo design

Stereo placement is structural, not arbitrary.

- foreground events use defined pan zones;
- PING-PONG explicitly assigns left/right roles;
- later ghosts may widen toward the edges;
- bloom may drift slowly across stereo without becoming obvious chorus;
- random pan remains bounded inside the assigned zone;
- preserve center energy so wide events do not hollow out the source;
- dry stereo remains stereo.

Goal: **purposeful spatial choreography**.

---

## 10. Level hierarchy and audibility

The first response must be unmistakable.

Illustrative event envelope:

| Event | Relative target |
|---|---:|
| 1 | 100% |
| 2 | 78% |
| 3 | 61% |
| 4 | 46% |
| 5 | 33% |
| 6 | 22% |
| Ghost A | 14% |
| Ghost B | 9% |
| End | 0% |

These are tuning targets, not fixed constants.

### Audibility floor

At approximately middle MIX:

- first foreground response is immediately audible;
- first response should feel roughly **70–90% as prominent as the source event** depending on mode;
- second foreground response remains clearly audible;
- ghosts may be subtle, but the primary phrase must not be subtle by accident.

If overlap normalization makes the foreground too quiet, **drop low-priority events before reducing the first response into inaudibility**.

---

## 11. Bloom / reverb behavior

Bloom is part of FIELD. It is **not separately triggered**.

Every playback event sends energy into the spatial network.

### Generation-dependent send

Early responses remain clear. Later generations progressively feed more bloom.

| Phrase stage | Clear echo | Bloom send |
|---|---:|---:|
| Event 1 | high | low |
| Event 2 | high | low-medium |
| Event 3 | medium-high | medium |
| Event 4 | medium | medium-high |
| Event 5 | low-medium | high |
| Ghosts | low | very high |

Narrative:

> **statement → response → memory → atmosphere**

### Bloom architecture

Use one compact shared network:

1. short irregular diffusion;
2. compact stereo feedback field;
3. damping / tonal shaping;
4. slow stereo motion.

Avoid embedding a second heavyweight standalone reverb architecture inside FIELD.

### Reliability rule

If an audible playback event exists, its bloom send exists. No separate onset threshold may suppress the bloom.

A very small direct-input room feed may be used to establish space between phrases.

---

## 12. Ethereal tone target

FIELD should not simply be dark.

### BODY

- warm low-mid density
- enough diffusion to make the space feel physically occupied
- controlled decay
- no boomy accumulation

### HALO

- quieter than BODY
- wider
- lighter/brighter
- receives selected octave/fifth ghosts
- shorter and more diffuse than BODY

Target impression:

> **floating, glassy, distant, deliberate — with enough body to own the space**

Later phrase events should progressively shift perceptual weight from clear BODY toward HALO/bloom.

---

## 13. Endless-echo prevention — hard requirement

FIELD must not rely on high conventional feedback for rhythmic repeats.

Required safeguards:

- finite per-seed event count
- absolute musical/sample TTL per seed
- no self-recapture from wet output
- maximum two live seeds
- explicit seed retirement state
- hard simultaneous playback-voice ceiling
- capped spatial feedback
- bloom feedback cannot spawn new phrase events
- state reset on SUSPEND / RESUME
- invalid-state protection where practical

At maximum MIX and slow CLOCK, atmosphere may decay for a while, but **new distinct rhythmic repeats must stop according to phrase lifetime**.

The processor must always have a reachable rest state.

---

## 14. Headroom strategy

The previous LATTICE builds proved that aggressive multi-stage gain can collapse into distortion on MkI hardware.

FIELD may be loud, but its gain structure must remain bounded.

Requirements:

- normalize event sum according to actual sounding playback count;
- retain internal headroom before bloom injection;
- normalize bloom send independently from clear echo path;
- bound feedback energy before runaway conditions;
- reduce dry level at strong MIX settings rather than stacking full dry + full wet;
- apply wet-path protection before sacrificing dry-source intelligibility;
- use final stereo peak protection only as emergency containment.

If the final guard is audibly pumping during ordinary material, upstream gain design has failed and must be retuned.

---

## 15. Audio-quality and clarity guardrails

These are non-negotiable.

### 15.1 Untouched dry spine

- dry/source stays separate from capture, pitch, diffusion, filtering, and feedback until final summing;
- near-dry MIX must be perceptually transparent;
- wet-path overload should not randomly pull the source down;
- source transient remains intact unless MIX is deliberately near fully wet.

### 15.2 Foreground/background depth

- foreground echo arrives clearly first;
- bloom rises with phrase age rather than masking the first response;
- new foreground events may lightly duck bloom if necessary for articulation;
- BODY supports the source instead of occupying the same level/spectral density;
- HALO remains lower than the main response.

Desired depth:

> **source in front → answers behind it → bloom behind the answers**

### 15.3 Clean capture/playback boundaries

Required:

- short fade/crossfade at fragment start and end;
- reverse uses the same edge treatment;
- stutter retriggers are windowed;
- CLOCK/MODE changes do not abruptly relocate sounding reads;
- retiring/replacing a seed fades rather than hard-deletes audible state.

Clicks, snaps, and sudden DC-like steps are implementation failures.

### 15.4 Pitch quality

- shifted ghosts remain below foreground level;
- non-unity ratios use interpolation;
- avoid multiple identical shifted ghosts landing together;
- window shifted fragment edges;
- omit a pitch event if it produces brittle/cheap artifacts on physical hardware.

### 15.5 Stereo integrity

- dry stereo remains stereo;
- mono capture is allowed only deliberately and must not collapse total output to mono;
- preserve center energy;
- bloom decorrelation must not create severe cancellation;
- test headphones and summed mono.

### 15.6 Spectral accumulation

- block DC in regenerative paths;
- prevent uncontrolled sub-bass buildup;
- preserve useful low-mid BODY;
- soften later generations progressively, not the entire output repeatedly;
- first responses retain source definition;
- no resonance peak may grow generation after generation.

### 15.7 Dynamics

- dry transient remains intact except near full wet;
- peak protection should primarily manage wet contribution before reducing total output;
- audible guard/limiter pumping is a failure;
- if protection is constantly active, reduce upstream energy rather than accepting limiter tone.

### 15.8 Silence/rest behavior

After a phrase expires:

- no rhythmic repeat continues;
- bloom decays naturally to inaudibility;
- no stuck sample remains in a playback voice;
- no residual oscillation remains in feedback state;
- no new seed appears without qualifying new dry/source material.

---

## 16. Structural safety guardrails

### Seed admission

A new seed requires:

1. qualifying dry/source input energy;
2. detector hysteresis/refractory protection;
3. enough separation from the previous accepted source event;
4. an available seed slot or explicit oldest-seed replacement decision.

FIELD output never enters this detector.

### Seed retirement

Each seed stores:

- remaining scheduled events;
- absolute expiry clock/sample count.

If either reaches zero, no new event may be scheduled from that seed.

### Event budget

When simultaneous playback capacity is full:

1. preserve the newest foreground response;
2. drop/retire the quietest ghost first;
3. then retire the oldest decaying event;
4. never increase total gain to compensate for a dropped event.

### Bloom energy budget

- normalize summed bloom sends before feedback injection;
- feedback coefficient has a hard ceiling below unity;
- feedback does not increase because more phrase events overlap;
- after input stops, the network must have a guaranteed decay path toward zero;
- bloom cannot feed seed capture or phrase scheduling.

### Parameter safety

- CLOCK and MODE latch at safe event boundaries;
- MIX is smoothed;
- other large coefficient changes are smoothed or boundary-latched;
- no knob position may create zero-length fragments, out-of-range reads, unbounded feedback, or division by zero.

### Invalid-state recovery

Where practical, reject non-finite or clearly invalid DSP state.

A bad voice/seed is retired individually. It must not destroy the entire audio path.

SUSPEND/RESUME always returns FIELD to a known clean state.

---

## 17. Resource budget

FIELD replaces simultaneous standalone LATTICE ECHO + LATTICE SPACE in the preferred MkI system.

Target limits:

- **2 active seed captures**
- **8 sounding playback voices maximum**, with six scheduled responses per seed
- **4 sounding voices** for newly admitted seeds at the fastest two subdivisions; existing voices always drain normally
- one compact bloom network shared by all events
- no separate reverb onset detector
- no continuously moving multi-tap bank
- scheduler calculations primarily at event boundaries
- avoid expensive audio-rate transcendental work where cheaper equivalents are perceptually sufficient

Perceived size comes from **phrase structure + stereo + pitch + diffusion**, not maximum simultaneous DSP activity.

Preserve runtime margin for LATTICE CORE + FIELD together.

---

## 18. Interaction with LATTICE CORE

Preferred MkI system:

```text
AUDIO IN / OSC
      ↓
LATTICE CORE  [MOD]
      ↓
LATTICE FIELD [DELAY]
      ↓
optional Korg internal REVERB
      ↓
OUTPUT
```

CORE remains responsible for microloop composition and performance-loop/freeze behavior.

FIELD should respond clearly even to one simple note arriving from CORE and must not rebuild CORE's dense microloop population internally.

---

## 19. Hardware acceptance criteria

The next FIELD build does not pass until these are physically verified on the original NTS-1 MkI.

### Identity

- one note produces an obvious deliberate response around middle MIX;
- FORWARD, REVERSE, PING-PONG, and STUTTER are immediately distinguishable;
- phrase behavior sounds designed rather than randomly scattered;
- pitch ghosts add identity without obscuring source.

### Finite behavior

- every isolated note eventually reaches silence without new input;
- no mode produces endless distinct rhythmic repeats;
- sustained playing does not let wet output recapture itself indefinitely.

### Bloom

- every audible echo phrase produces audible spatial bloom;
- bloom progressively increases across later generations;
- space feels full/fat while retaining an ethereal halo;
- bloom never appears to “miss” a trigger.

### Level and clarity

- FIELD is easy to hear without extreme MIX;
- middle MIX makes the first two responses unmistakable;
- dry/source phrase remains intelligible;
- bloom sits behind the foreground response;
- maximum MIX is strong but does not clip/collapse;
- peak protection is not audibly pumping;
- CORE + FIELD can both be pushed aggressively without the previous three-unit collapse behavior.

### Fidelity

- no clicks at capture, reverse, stutter, seed replacement, or retirement boundaries;
- no obvious zipper noise while sweeping MIX;
- no unintended mono collapse or unstable center image;
- pitch ghosts sound like intentional harmony rather than artifacts;
- processor returns to a clean rest state;
- summed-mono playback does not severely cancel source or first response.

### Stability

- sweep TIME through every CLOCK zone while audio runs;
- sweep DEPTH through all four modes while audio runs;
- sweep MIX dry-to-max repeatedly;
- test fast subdivisions under dense source material;
- test CORE + FIELD together for at least 30 minutes;
- no silence/dropout, runaway feedback, stuck phrase, invalid playback state, or permanent distortion state.

---

## 20. Out of scope for this build

Do not add until FIELD identity and stability pass:

- more than two active seed phrases
- arbitrary scales/chord detection
- broad random pitch selection
- spectral/granular FFT processing
- convolution reverb
- infinite/freeze feedback mode
- additional front-panel macro controls
- additional playback modes
- deliberate lo-fi degradation, clipping, bitcrushing, or saturation as a substitute for clean DSP

Priority:

> **obvious, intentional, finite, ethereal, clear, high-quality, and stable**

---

## 21. Design principle

> **LATTICE FIELD should not sound like a delay with reverb attached. It should sound like the machine heard a phrase, answered it deliberately, and then let the answer evaporate into space.**


## 22. Approved 0.1-0 implementation contract (2026-09-10)

This section makes the approved review proposals concrete. It supersedes illustrative tables where they differ. The product identity and hardware gates above remain mandatory.

- **Admission:** incoming stereo energy only; 120 ms refractory interval, transient hysteresis or a release/rearm (20 ms below the lower energy threshold). No periodic sustained-input fallback. A fresh attack can qualify while another source sustains. These thresholds require physical source-level calibration.
- **Upstream distinction:** FIELD sees CORE's output. A CORE freeze with recurring attacks can continue qualifying as input. Finite/rest guarantees apply after qualifying FIELD input ends; they do not promise silence while upstream CORE keeps generating new attacks.
- **Capture:** two owned stereo float captures, 4096 samples/channel each (up to 85.3 ms at 48 kHz). A 512-frame stereo history supplies a fixed 256-frame preroll. Capture length is half a tick, clamped to 1024–4096 frames. No wet signal enters either buffer. On replacement, the oldest seed retires over 96 samples before its storage is reused; identity checks reject stale voices.
- **Phrase:** six responses, levels 1.00 / .78 / .61 / .46 / .14 / .09. First four are unison; last two are +7 / +12 semitone ghosts. No extra random events in this first controlled comparison.
- **Timing:** event times in CLOCK ticks are Forward/Ping-Pong `[1,2,3,4,6,8]`, Reverse `[1,3,4,6,7,8]`, Stutter `[1,1.25,1.5,2,3,4]`, measured after nominal capture completion. Reverse plays backward; Ping-Pong alternates structural stereo balance; Stutter shortens slices. Original stereo is retained in foreground playback.
- **Control latching:** CLOCK, MODE and host BPM snapshot at seed admission. MIX is smoothed continuously. Clock division index and host tempo do not imply transport/beat-phase synchronization; the phrase is tempo-relative from its own capture.
- **TTL:** each seed's immutable sample deadline is admission + `10*tick + 4096` frames, independent of event-count completion. A waiting foreground event never extends this deadline. Counter comparisons handle uint32 sample-clock wrap for these bounded intervals.
- **Capacity:** eight physical voice slots, four-admission ceiling at the fastest two CLOCK subdivisions. Every allocated slot is serviced, even after a budget change. Ghosts are omitted when full. Foreground admission fades a ghost/quiet tail over 96 frames and waits for room; this may shift that event by about 2 ms. Transitioning from an older slow phrase to a fast phrase drains the old voices instead of cutting them off.
- **Pitch:** linear interpolation, 96-frame edges and darker low-level pitched ghosts. This is playback-rate transposition, not independent time stretching. Physical aliasing/quality gates remain OPEN.
- **Hierarchy:** foreground and ghosts have separate smoothed overlap normalization. Bloom sends are independently normalized. No gain is added to replace an omitted event.
- **Bloom:** one shared four-line FDN (1493/2111/2971/3797 samples), two short allpasses, DC rejection, damping and .82 feedback. BODY and quieter HALO are outputs of this network, not separate reverb engines. All audible events send to bloom, using .18/.30/.48/.72/1.10/1.35 stage send weights. Smoothed bloom level is lower during foreground answers. No direct-input room feed in 0.1-0.
- **Rest:** with no sounding voice, bloom decays naturally; a final 6–8 second envelope guarantees inaudible output by 8 seconds after the last voice. Storage clears incrementally thereafter (at most another 4096 frames), avoiding a large audio-callback clear. Host tests require output below 1e-6 FS at final rest. New qualifying input resets that bloom timer.
- **Mix:** dry gain `1-MIX`, wet gain `.8*MIX`, with a stereo-linked wet peak guard at .90 FS and an emergency final clamp at +/-1. Mid-MIX first-answer design ratio is approximately .8 of dry before stereo balance; prominence remains a listening criterion. Near-dry transparency applies to valid normalized source input.
- **Memory:** declared audio arrays total 139,264 bytes (136 KiB) in SDRAM. Linker results separately establish executable/state SRAM use. Desktop tests do not establish MkI CPU margin.
- **Testing baseline:** pair FIELD with unchanged CORE 0.3-0 first. The previously proposed CORE freeze corrections remain a separate versioned follow-up so FIELD's effect can be isolated.

The first hardware package includes the exact project/SDK/toolchain identifiers, unit checksums, memory reports, desktop test result and hardware QA sheet. BUILD ONLY is not hardware acceptance.
