# Human Soon NTS-1 Suite — M2 to M4 Roadmap

Target: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**  
Baseline: **all 25 M1 units hardware validated on MkI**

This document defines the next development stages for the current suite. M1 is the frozen known-good baseline. Any source change that advances a unit to M2, M3, or M4 reopens that unit's hardware QA gate.

## Milestone convention

- **M1 — Working engine:** first useful implementation; compiled and physically validated.
- **M2 — Core capability:** add the serious DSP feature that completes the effect category.
- **M3 — Human Soon behavior:** add the distinctive controlled-instability / memory / interaction layer that makes the unit ours rather than a generic implementation.
- **M4 — Final instrument:** hardware tuning, musical ranges, gain staging, CPU/memory optimization, alias/artifact cleanup, parameter smoothing, stress testing, and release defaults.

Do not collapse milestones. Each M2/M3/M4 stage must compile, package, and pass its own MkI regression QA before the next stage begins.

---

# Oscillator / core projects

## SPECTRA

**M2 — Independent Drift**  
Complete the per-voice drift engine: independent LFO rates/phases, `Drift` depth, `Motion` rate scaling, and host `shape_lfo` interaction. QA focuses on non-correlated movement and pitch stability.

**M3 — Chaos**  
Add bounded note-on mutation of phase, fine tuning, and internal level. `Chaos = 0` must remain deterministic; higher values vary repeated notes without losing the selected harmonic identity.

**M4 — Musical Tuning**  
Tune interval constellations, Spread/Drift limits, normalization, high-note band-limiting, and default gain. Establish recommended conservative and extreme settings.

## PARALLAX

**M2 — One True Pitch Voice**  
Add one dual-read-head / dual-grain pitch shifter with stable cents offsets. Prove fixed detuning, click-free crossfades, and CPU headroom while the existing M1 field remains active.

**M3 — Multi-Pitch Field**  
Scale the pitch engine to two and then up to four wet voices. Preserve independent delay times, modulation rates, and stereo anchors. A `2 shifted + 2 chorus` architecture is acceptable if it is musically stronger or more reliable.

**M4 — Constellation Tuning**  
Tune the default microtonal ensemble by ear on hardware, compare 2/3/4-voice variants, optimize wet-sum headroom, and decide whether interval-based constellations are worth the CPU cost.

## CHORDGHOST

**M2 — OXI Chord Protocol**  
Exhaustively validate CC31 chord-code decoding, boundary values, special codes, and real-time chord changes without interrupting the delay tail.

**M3 — Fixed Interval Shifter**  
Add a semitone pitch shifter in the wet/feedback path and prove known forced intervals before any pitch detection is involved.

**M4 — Monophonic Pitch Detector**  
Add stable fundamental detection, confidence gating, note-change hysteresis, and last-stable-pitch behavior. Full voice-leading remains a later milestone after M4.

---

# Human Soon ModFX

## DUST

**M2 — Precision Reduction Engine**  
Refine the sample-hold clock and quantizer so RATE and DAMAGE remain smooth across their entire ranges. Add better low-level handling, DC safety, and optional light dither/noise shaping only if it improves the hardware result.

**M3 — Bit-Rot Memory**  
Make severe settings remember and corrupt a small amount of previous state: bounded stale-sample bleed, intermittent bit loss, or slowly changing quantizer bias. The behavior must remain deterministic enough to perform with and must never emit runaway DC.

**M4 — Release Calibration**  
Tune alias onset, minimum clean range, maximum destruction, stereo-fracture depth, loudness consistency, and mono compatibility. Keep M1's immediate dry-to-destroyed usability.

## CARRIER

**M2 — High-Rate Quality Pass**  
Improve audio-rate behavior with a more accurate carrier path and, only if MkI headroom allows, selective oversampling or a deliberately limited upper frequency range. Preserve the continuous Dry → AM → Ring `POLARITY` macro.

**M3 — Sideband Character**  
Add alternate bounded carrier behavior such as waveform morphing or controlled L/R sideband divergence without turning the unit into a mode menu. The macro controls must remain performable from `TIME` and `DEPTH`.

**M4 — Release Calibration**  
Tune carrier frequency law, AM/ring transition, stereo phase split, aliasing threshold, mono collapse, and output level.

## VECTORFILTER

**M2 — Refined SVF Core**  
Upgrade the state-variable filter for better resonance behavior, coefficient stability, and smoother `CUTOFF` modulation while preserving the `LP → BP → HP` `VECTOR` morph.

**M3 — Vector Instability**  
Introduce bounded interaction between filter state, input level, and VECTOR position so the center region can breathe or lean without becoming a random wah. Instability must self-recover when controls return to conservative values.

**M4 — Release Calibration**  
Tune cutoff law, resonance ceiling, morph equal-loudness, high-frequency stability, and rapid-parameter behavior across bass, pads, percussion, and full-range input.

## IRONROT

**M2 — Multi-Stage Corrosion**  
Replace the simple distortion core with several coordinated nonlinear stages: pre-emphasis/tone shaping, asymmetric saturation, and post-stage cleanup. `CHARACTER` changes the spectral bias while `CORROSION` controls severity.

**M3 — Reactive Material Failure**  
Let input energy and recent signal history subtly change the nonlinear response: temporary softening, hardening, or crackle-like edge changes after strong transients. Keep the state bounded and silence-safe.

**M4 — Release Calibration**  
Match loudness across the macro range, control aliasing, tune bass retention and high-frequency bite, and decide whether selective oversampling is worth the CPU cost.

## ATTRACTOR

**M2 — Stable Chaotic Orbit**  
Refine the deterministic chaotic system so `RATE` controls evolution speed and `ORBIT` controls stereo excursion without collapsing into an ordinary periodic LFO. Use constant-power or near-constant-power panning.

**M3 — Perturbed Orbit**  
Add bounded transient-driven perturbations or slow attractor-state changes so the path can reorganize without losing continuity. No hard image flips or channel dropouts.

**M4 — Release Calibration**  
Tune minimum/maximum motion, stereo width, mono compatibility, perceived level, and long-session non-repetition.

## ZEROCROSS

**M2 — True Through-Zero Core**  
Refine the delayed reference path and modulation so the moving delay relationship crosses true zero cleanly. Add explicit feedback polarity control internally to achieve the intended cancellation/reinforcement behavior.

**M3 — Asymmetric Feedback Field**  
Introduce bounded L/R or positive/negative feedback asymmetry so each zero crossing has a different spectral shape instead of repeating one flanger sweep.

**M4 — Release Calibration**  
Tune sweep law, center cancellation, feedback ceiling, low-frequency phase behavior, wrap safety, and mono compatibility.

## PHASEWELL

**M2 — Precision All-Pass Network**  
Improve all-pass coefficient mapping, stage behavior, and resonance so the six-stage network produces deep but stable moving notches across the full `RATE` and `DEPTH` range.

**M3 — Unequal Wells**  
Give selected stages slightly different modulation depths/rates or bounded state coupling so the notch pattern deforms over time rather than sliding as one rigid comb.

**M4 — Release Calibration**  
Tune notch depth, resonance ceiling, stereo asymmetry, low-end integrity, and control smoothing.

## ASCENDER

**M2 — Continuous Barber-Pole Engine**  
Improve the multi-lane phase network and crossfade/window law so rising motion survives lane wrap points without obvious resets.

**M3 — Height Geometry**  
Make `HEIGHT` alter the spacing and relative emphasis of the climbing phase lanes, creating anything from subtle endless motion to a large synthetic ascent.

**M4 — Release Calibration**  
Tune climb rate, wrap masking, stereo behavior, resonance, and the point where the illusion remains strong without sounding like ordinary cyclic phasing.

## HELIX

**M2 — Continuous Multi-Head Flanger**  
Refine the cyclic moving delay heads, interpolation, and windowing so the barber-pole flanging illusion survives read-head resets and high feedback.

**M3 — Braided Feedback**  
Cross-couple selected heads or channels with bounded feedback so successive spectral sweeps twist around each other rather than repeating the same contour.

**M4 — Release Calibration**  
Tune head spacing, rotation rate, feedback ceiling, through-zero behavior, stereo width, and wrap masking.

## CAPSTAN

**M2 — Physical Motion Model**  
Separate slow wow, faster flutter, and correlated L/R transport movement. Improve interpolation so pitch motion remains smooth on sustained tones.

**M3 — Wear Memory**  
Add bounded transport defects that evolve with time: soft dropouts, temporary drag, or irregular flutter bursts whose probability follows `WEAR` and recent state.

**M4 — Release Calibration**  
Tune wow/flutter depth, dropout severity, stereo coherence, low-frequency stability, and recovery after extreme settings.

## SIDEBAND

**M2 — Higher-Rejection Frequency Shifter**  
Improve the quadrature/Hilbert network and calibrate `SHIFT` so frequency translation is cleaner, especially at small shifts and on sine sources.

**M3 — Divergent Sidebands**  
Use `DIVERGENCE` to separate L/R shift direction or amount in a bounded way, creating motion that is impossible with a single mono frequency shifter.

**M4 — Release Calibration**  
Tune shift law around zero, sideband rejection, stereo width, aliasing at large shifts, and level consistency.

## FAULTLINE

**M2 — Controlled Stability Boundary**  
Refine the nonlinear filter so `FAULT` moves the system toward a clearly defined edge of instability while hard state bounds and self-recovery remain guaranteed.

**M3 — Fault Events**  
Add bounded coefficient kicks, hysteresis, or short-lived state disturbances that occur more often as `FAULT` rises. The effect should sound damaged, not broken.

**M4 — Release Calibration**  
Map the musical safe zone, tune self-oscillation/near-oscillation behavior, remove pathological DC or ultrasonic states, and stress-test rapid control changes.

---

# Human Soon DelFX

All current Human Soon DelFX continue to use `SHIFT_DEPTH` as **MIX** unless a later milestone explicitly changes that assignment.

## BALLISTIC

**M2 — Continuous Trajectory Engine**  
Refine accelerating/even/decelerating tap timing so `TRAJECTORY` produces a continuous nonlinear timing curve rather than a few discrete patterns.

**M3 — Gravity / Bounce Character**  
Add bounded energy loss, stereo displacement, or bounce-to-bounce timing mutation so repeats feel like a physical system responding to gravity rather than a static multitap delay.

**M4 — Release Calibration**  
Tune RANGE, trajectory extremes, feedback, transient clarity, stereo spread, and tempo behavior.

## RAINFALL

**M2 — Multi-Droplet Scheduler**  
Increase the independence of droplet timing, damping, and stereo placement while keeping CPU and memory bounded. Retarget droplets smoothly instead of jumping delay positions.

**M3 — Weather Memory**  
Let `WEATHER` shape a slowly evolving state: drizzle, uneven showers, dense bursts, and temporary clearings. Changes should have inertia rather than instant random replacement.

**M4 — Release Calibration**  
Tune density scaling, transient buildup, stereo field, maximum overlap, feedback safety, and long-session repetition.

## SWARMDELAY

**M2 — Robust Clocked Constellations**  
Improve BPM/division handling, clock-change smoothing, and the four-tap timing constellation. Define predictable fallback behavior when external MIDI clock disappears.

**M3 — Microtiming Divergence**  
Expand `DIVERGENCE` into controlled per-tap microtiming, damping, and stereo differences so the delay feels performed rather than quantized.

**M4 — Release Calibration**  
Tune division table, clock transition behavior, wet gain, mono compatibility, and useful DIVERGENCE range from tight ensemble to loose swarm.

## GLITCHREPEAT

**M2 — Deterministic Repeat State Machine**  
Formalize finite repeat states, slice capture/retrigger timing, and probability so `SLICE` and `CHANCE` remain predictable enough for performance.

**M3 — Mutating Repeats**  
Allow selected repeat events to alter playback direction, slice length, level, or timing within hard bounds. Mutation must end and recover rather than getting stuck in a permanent glitch.

**M4 — Release Calibration**  
Tune probability law, slice sizes, tempo transitions, deterministic seeding/reproducibility strategy, and abuse recovery.

## BUCKETLINE

**M2 — BBD-Like Clock / Filter Coupling**  
Tie effective bandwidth and damping more directly to delay time/clock behavior, with stronger analog-style loss at longer or more degraded settings.

**M3 — Accumulating Age**  
Make repeats progressively lose bandwidth, gain subtle wobble, and saturate in a stateful way so `AGE` changes the memory itself rather than only the first pass.

**M4 — Release Calibration**  
Tune delay law, feedback loss, noise floor if used, saturation, maximum repeat count, and runaway protection.

## LONGMEMORY

**M2 — Delay-to-Loop Continuum**  
Refine the transition from ordinary delay into near-loop / long-memory behavior with smooth time changes and stable long-buffer operation.

**M3 — Destructive Memory Aging**  
Make material decay structurally over repeated writes: bandwidth loss, soft saturation, tiny timing drift, or write-bleed. The memory should visibly age in sound without collapsing unpredictably.

**M4 — Release Calibration**  
Tune maximum practical length, loop threshold, feedback/memory law, state clearing, switching behavior, and long-session stability.

## SHARD

**M2 — Calibrated Dual-Grain Pitch Shift**  
Calibrate the dual-grain engine to musically useful semitone/cents positions, improve crossfade windows, and prove stable up/down/unity pitch ratios.

**M3 — Shard Field**  
Expand from one coherent pitch to a bounded multi-grain field with small pitch/time/stereo offsets around the target. `GRAIN` controls fragmentation rather than simple wetness.

**M4 — Release Calibration**  
Tune pitch accuracy, window length, transient smearing, high-frequency aliasing, stereo spread, and CPU limits.

---

# Human Soon RevFX

All current Human Soon RevFX continue to use `SHIFT_DEPTH` as **MIX**.

## ABYSS

**M2 — Refined FDN Space**  
Improve delay-line ratios, damping, modulation, and diffusion so the four-line FDN produces a smoother large space with less metallic ringing.

**M3 — Moving Architecture**  
Allow `SPACE` to slowly alter internal line relationships or modulation depth so the room appears to stretch or deform while the tail is alive.

**M4 — Release Calibration**  
Tune decay time, damping, modulation, low-frequency buildup, stereo field, freeze-adjacent settings, and runaway safety.

## AUREOLE

**M2 — Reliable Octave Feedback**  
Refine the dual-grain octave shifter in the feedback path for cleaner +12-semitone behavior, smoother grain crossfades, and predictable CPU use.

**M3 — Halo Constellations**  
Let `HALO` move from subtle octave bloom toward a bounded multi-layer shimmer character using octave emphasis, small detunes, or carefully limited interval variants.

**M4 — Release Calibration**  
Tune shimmer level, feedback injection, high-frequency damping, pitch leakage, decay stability, and mix law so the effect remains useful below extreme settings.

## NEBULA

**M2 — Denser Cloud Engine**  
Improve grain windows, scheduling, diffusion, and overlap so the cloud becomes smoother at high density without losing the granular identity.

**M3 — Evolving Cloud State**  
Add slowly changing grain density, scatter, damping, and optionally tiny bounded pitch dispersion so the reverb can drift between sparse particles and a continuous fog.

**M4 — Release Calibration**  
Tune cloud density, grain length, diffusion, tail smoothness, CPU ceiling, stereo width, and parameter transitions.

---

# M2–M4 QA rule

For every unit:

1. Preserve the current M1 binary as the known-good fallback.
2. Implement only the current milestone.
3. Compile and package against Korg's current original-NTS-1 SDK template.
4. Update that project's QA sheet with milestone-specific checks before hardware testing.
5. Load and test on the physical MkI.
6. Record failures using exact control values/source/BPM/stereo state where applicable.
7. Do not advance to the next milestone until the current milestone passes.

## Suggested development order

To maximize reusable DSP and minimize risk:

1. **PARALLAX M2** — establishes a reusable pitch-shift engine.
2. **SHARD M2** — calibrates granular pitch behavior in a dedicated effect.
3. **CHORDGHOST M2/M3** — validates OXI control and reuses the pitch engine.
4. **SPECTRA M2/M3** — finishes the oscillator's already-designed expressive layers.
5. **ModFX M2 wave** — VECTORFILTER, IRONROT, SIDEBAND, ZEROCROSS first.
6. **Delay M2 wave** — SWARMDELAY, LONGMEMORY, BALLISTIC, RAINFALL, BUCKETLINE, GLITCHREPEAT.
7. **RevFX M2 wave** — ABYSS, AUREOLE, NEBULA after the pitch/grain utilities are proven.
8. M3 Human Soon behavior unit-by-unit.
9. M4 release calibration only after each unit's M3 identity is stable.
