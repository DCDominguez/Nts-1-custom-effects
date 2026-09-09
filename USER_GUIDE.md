# Human Soon NTS-1 Suite — User Guide

Target: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**.

This guide documents the **current hardware-validated M1 builds**. Later M2+ features described in project specs are not assumed to exist in the current binaries unless explicitly stated here.

## NTS-1 control shorthand

The suite follows the original NTS-1 effect parameter model and the labels used in the Korg/logue SDK:

- **SPECTRA (`osc`)** uses `SHAPE`, `ALT`, and six user-oscillator edit parameters.
- **ModFX** use `TIME` and `DEPTH`.
- **DelFX** use `TIME`, `DEPTH`, and `SHIFT_DEPTH`; in the Human Soon delay units, `SHIFT_DEPTH` is used as **MIX**.
- **RevFX** use `TIME`, `DEPTH`, and `SHIFT_DEPTH`; in the Human Soon reverb units, `SHIFT_DEPTH` is used as **MIX**.

Select each user unit in the matching NTS-1 section, then use the normal NTS-1 controls for that section. Start at a conservative output level when exploring extreme settings.

---

# Oscillator

## SPECTRA

**What it is:** A four-voice swarm oscillator. One played note becomes up to four related internal voices with detune, harmonic intervals, slow drift, and bounded note-on variation. The four voices are summed to mono before the NTS-1 host filter/envelope/effects.

### Controls

| Function | What it does | How to use it |
|---|---|---|
| `SHAPE` | Morphs **Sine → Triangle → Saw → Square** | Start near sine for a clean tone, then sweep toward saw/square for more harmonics. The host shape LFO can also animate this morph. |
| `ALT` | Harmonic amount | At 0, secondary voices collapse toward unison. Raise it to move them toward the selected `HarmMode` intervals. |
| `Voices` | Selects 1–4 internal voices | Use 1 for a normal oscillator, 2 for a simple interval/detune pair, and 3–4 for the full swarm. |
| `Spread` | Static detune width | Keep near 0 for tight tuning; raise it for beating and ensemble width. |
| `Drift` | Independent slow pitch movement | Raise gradually for organic instability. At 0, voices do not drift. |
| `HarmMode` | Selects the interval constellation | Choose `UNISON`, `FIFTH`, `OCTAVE`, `MAJOR`, `MINOR`, `SUS`, `QUARTAL`, or `CLUSTER`. |
| `Motion` | Scales drift speed | Low values make Drift very slow; higher values make the voices move faster. |
| `Chaos` | Bounded note-on variation | At 0, repeated notes are deterministic. Raise it for small changes in phase, fine tuning, and internal balance on each note. |

### Quick start

Set `Voices=4`, `Spread` around 20–30%, `Drift` low, `Motion` low, `Chaos=0`, choose `MINOR` or `MAJOR`, then bring `ALT` up slowly. Use `SHAPE` to decide whether the result is soft, hollow, bright, or aggressive.

---

# Core experimental effects

## PARALLAX

**What it is:** A four-voice spatial chorus / swarm doubler. The current M1 build uses four independently timed and modulated wet voices with fixed stereo anchors. M1 does **not** yet claim stable true pitch offsets; that is a later milestone.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **SPREAD** | Separates the four arrival times | Low values give a tight chorus; higher values move toward ensemble/doubling and a more obvious time cluster. |
| `DEPTH` → **DIVERGENCE** | Makes the voices behave more independently | Raise it for deeper decorrelated modulation and a wider field. Lower it for a more conventional chorus. |
| `SHIFT_DEPTH` → **MIX** | Dry/wet balance | Start around the middle. Lower for subtle thickening; raise for the full spatial field. |

### Quick start

Use a sustained saw or pad, set MIX around 50%, SPREAD low-to-medium, then raise DIVERGENCE until individual voices become apparent without the dry source losing focus.

## CHORDGHOST

**What it is:** The M1 foundation of a harmonic delay. The current validated build is a **BPM-aware synced delay** with bounded feedback. The chord-decoding and reharmonizing stages are later milestones, so do not expect the M1 binary to retune repeats yet.

| Function | What it does now | How to use it |
|---|---|---|
| `TIME` | Chooses rhythmic delay timing/division | Use it while the NTS-1 is following the desired BPM/clock. Sweep through divisions to find the repeat rhythm. |
| `DEPTH` | Reserved for the chord-state control path used by later milestones | Leave it at a stable value for ordinary M1 use. Future versions use this path for OXI chord codes. |
| `SHIFT_DEPTH` → **MIX** | Dry/wet balance | Keep low for a background echo or raise it to make the delay prominent. |

### Quick start

Feed a short pluck, set the NTS-1 to the desired tempo source, choose a TIME division, and bring MIX up until the synced repeats sit correctly against the dry note.

---

# Human Soon ModFX

## DUST

**What it is:** Sample-rate and bit-depth reduction with a bounded stereo clock fracture at stronger settings.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **RATE** | Lowers the effective sample/update rate | Low values stay cleaner; raise it for stair-stepping, aliasing, and increasingly obvious sample-hold texture. |
| `DEPTH` → **DAMAGE** | Increases quantization and processed mix | Start low for grit; push high for coarse digital destruction. Strong RATE + DAMAGE introduces stereo fracture. |

### Quick start

Start with RATE around 25% and slowly raise DAMAGE. Then increase RATE until the alias texture becomes rhythmic rather than merely noisy.

## CARRIER

**What it is:** A dry-to-AM-to-ring-modulation engine with bounded stereo carrier phase separation at stronger ring settings.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **FREQUENCY** | Sets carrier speed from sub-audio movement into audio-rate sidebands | Low = tremolo/AM movement. Higher = metallic ring-mod sidebands. |
| `DEPTH` → **POLARITY** | Moves continuously from dry → unipolar AM → bipolar ring modulation | Around the middle gives strong AM; near maximum gives full ring modulation. |

### Quick start

Set POLARITY around 50% and sweep FREQUENCY to find a useful tremolo or AM rate. Then push POLARITY toward 100% for metallic tones and sideband splitting.

## VECTORFILTER

**What it is:** A resonant state-variable filter that morphs between low-pass, band-pass, and high-pass responses.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **CUTOFF** | Sets filter cutoff | Sweep it to expose or remove frequency content. |
| `DEPTH` → **VECTOR** | Morphs the filter response **LP → BP → HP** | Low values emphasize lows, the middle focuses the band around cutoff, and high values emphasize highs. |

### Quick start

Use a harmonically rich saw. Put VECTOR near the low-pass side, sweep CUTOFF, then move VECTOR through the center toward high-pass to hear the topology morph.

## IRONROT

**What it is:** A bounded tone-dependent distortion/erosion processor designed to change character rather than only increase gain.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **CHARACTER** | Changes the distortion character / tonal bias | Sweep slowly to find the part of the source spectrum you want the nonlinear stage to emphasize. |
| `DEPTH` → **CORROSION** | Controls the amount of nonlinear erosion | Use low values for edge and saturation; raise for heavier destruction. |

### Quick start

Set CORROSION low-to-mid, sweep CHARACTER until the source takes on the desired bite, then raise CORROSION only as far as needed.

## ATTRACTOR

**What it is:** A deterministic chaotic stereo-motion processor. Its motion is deliberately not a simple repeating sine-wave autopan.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **RATE** | Controls how quickly the chaotic orbit evolves | Low for slow spatial drift; higher for more animated movement. |
| `DEPTH` → **ORBIT** | Controls the width/strength of the stereo orbit | Keep low for subtle image motion; raise for wide wandering placement. |

### Quick start

Feed a mono pad, set RATE low, and increase ORBIT until the movement is clearly stereo. Then raise RATE only enough to match the track's motion.

## ZEROCROSS

**What it is:** A through-zero flanger architecture designed around the cancellation region where the delayed path crosses the dry reference.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **RATE** | Controls flanger sweep speed | Low for long jet-like motion; higher for obvious cyclic motion. |
| `DEPTH` → **SWEEP** | Controls sweep range/intensity | Raise it to make the zero-crossing and comb movement more dramatic. |

### Quick start

Use a bright sustained source, set RATE low, and bring SWEEP up until the central cancellation/jet effect becomes obvious.

## PHASEWELL

**What it is:** A six-stage asymmetric stereo phaser with a combined depth/resonance macro.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **RATE** | Sets phase-motion speed | Low for slow evolving notches; higher for obvious modulation. |
| `DEPTH` → **DEPTH / RESONANCE** | Deepens the phase sweep and its emphasis | Increase gradually; strong settings make the notches more pronounced and resonant. |

### Quick start

Start RATE low and DEPTH around 30–40%. Raise DEPTH until the phaser becomes obvious, then tune RATE to the musical phrase.

## ASCENDER

**What it is:** A barber-pole phaser built from multiple crossfaded phase lanes to create the impression of continuously climbing phase motion.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **CLIMB** | Controls apparent climbing speed | Low values create slow, almost endless movement; higher values make the illusion more obvious. |
| `DEPTH` → **HEIGHT** | Controls strength/range of the barber-pole structure | Raise it for a deeper and more spatial climbing effect. |

### Quick start

Use a sustained pad or noise-rich source, keep CLIMB slow, and increase HEIGHT until the repeated upward motion becomes clear without obvious reset points.

## HELIX

**What it is:** A barber-pole flanger using cyclic windowed delay heads to create an apparently continuous rotating/flanging motion.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **ROTATION** | Controls the speed of the cyclic flanger motion | Low for long spirals; higher for more animated rotation. |
| `DEPTH` → **HELIX** | Controls the strength/range of the helix field | Raise it to deepen the comb motion and make the cyclic illusion stronger. |

### Quick start

Use a bright chord or noise-rich sound, start ROTATION slow, then raise HELIX until the cyclic sweep is obvious but still smooth.

## CAPSTAN

**What it is:** Tape-motion instability: wow, flutter, and bounded wear/dropout behavior rather than a conventional chorus.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **MOTION** | Controls the amount/rate character of wow and flutter motion | Low for subtle transport instability; higher for obvious pitch/time wobble. |
| `DEPTH` → **WEAR** | Adds loss, damage, and bounded dropout character | Use low values for aged tape; higher values for more broken transport behavior. |

### Quick start

Start MOTION low and WEAR around 20%. Raise MOTION until pitch motion is audible, then use WEAR to decide whether it feels gently aged or mechanically damaged.

## SIDEBAND

**What it is:** A compact quadrature frequency shifter that translates spectral content instead of transposing it like a pitch shifter.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **SHIFT** | Controls the frequency-shift amount | Small shifts create detune/phase-like movement; larger shifts become metallic and inharmonic. |
| `DEPTH` → **DIVERGENCE** | Separates the stereo sideband behavior | Keep low for a tighter image; raise for a more separated stereo frequency-shift field. |

### Quick start

Use a voice or simple synth tone, begin with a small SHIFT, then increase until the source becomes noticeably inharmonic. Add DIVERGENCE for stereo width.

## FAULTLINE

**What it is:** A deliberately unstable but hard-bounded nonlinear filter. It is meant to approach failure-like behavior without actually letting the DSP run away.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **CUTOFF** | Sets the main filter region | Sweep it to place the instability in a useful frequency range. |
| `DEPTH` → **FAULT** | Increases nonlinear/unstable coefficient and feedback interaction | Raise gradually. Low is filter-like; high settings become increasingly broken and self-disturbing while remaining bounded. |

### Quick start

Use a sustained saw, set FAULT low, find a useful CUTOFF, then increase FAULT until the filter begins to sound unstable without overwhelming the dry musical content.

---

# Human Soon DelFX

All current Human Soon DelFX use `SHIFT_DEPTH` as **MIX**.

## BALLISTIC

**What it is:** A nonlinear multi-tap delay whose repeats can feel like an accelerating or decelerating bouncing object.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **RANGE** | Sets the overall time span of the tap pattern | Short for tight bouncing repeats; longer for a wider trajectory. |
| `DEPTH` → **TRAJECTORY** | Morphs the spacing between accelerating, even, and decelerating behavior | Sweep across the range to choose whether repeats bunch toward the start, stay even, or bunch toward the end. |
| `SHIFT_DEPTH` → **MIX** | Dry/wet level | Start around the middle and adjust after the trajectory is set. |

### Quick start

Use a short pluck, set RANGE around the middle, then sweep TRAJECTORY until the bounce moves in the desired direction.

## RAINFALL

**What it is:** A stochastic droplet-delay field with independently retargeted timing and stereo placement.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **DENSITY** | Controls how crowded the droplet field becomes | Low for isolated drops; high for a denser rain-like texture. |
| `DEPTH` → **WEATHER** | Controls timing/stereo scatter and overall disorder | Low for gentle rain; higher for a wider, less predictable field. |
| `SHIFT_DEPTH` → **MIX** | Dry/wet level | Use lower MIX for ambience and higher MIX for the effect as the main texture. |

### Quick start

Feed a pluck or percussion hit, set DENSITY low-to-mid, then raise WEATHER until the repeats stop feeling like a normal delay and start behaving like scattered droplets.

## SWARMDELAY

**What it is:** A BPM-derived four-tap ensemble delay with microtiming divergence.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **DIVISION** | Selects the tempo-related delay division/constellation | Choose a division that fits the song clock. |
| `DEPTH` → **DIVERGENCE** | Spreads the taps away from rigidly identical timing | Low keeps the ensemble tight; high produces a looser swarm around the beat. |
| `SHIFT_DEPTH` → **MIX** | Dry/wet level | Use lower values for rhythmic thickening and higher values for obvious ensemble echoes. |

### Quick start

Sync the NTS-1 to the track, choose a DIVISION, set MIX around 40–50%, and raise DIVERGENCE until the taps feel humanized without losing the pulse.

## GLITCHREPEAT

**What it is:** A bounded randomized repeater with finite repeat-state memory.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **SLICE** | Sets the captured/repeated slice length | Short slices give rapid stutters; longer slices sound more like loop fragments. |
| `DEPTH` → **CHANCE** | Controls how readily the repeat state is engaged/varied | Low for occasional events; high for a more active glitch stream. |
| `SHIFT_DEPTH` → **MIX** | Dry/wet level | Keep some dry signal for rhythmic clarity unless you want full glitch takeover. |

### Quick start

Use a drum loop, set SLICE short, CHANCE low, and MIX around half. Raise CHANCE until glitches happen often enough to become part of the groove.

## BUCKETLINE

**What it is:** A digital delay with analog-style clock wobble, loss, and saturation folded into an AGE macro.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **DELAY** | Sets delay time | Use short times for comb/doubling behavior and longer times for clear echoes. |
| `DEPTH` → **AGE** | Adds feedback loss, wobble, and saturation | Low = cleaner buffer; high = older, darker, less stable repeats. |
| `SHIFT_DEPTH` → **MIX** | Dry/wet level | Set after DELAY and AGE so the degraded repeats sit behind the source. |

### Quick start

Choose the delay time first, then slowly raise AGE until each repeat loses enough fidelity to feel like a decaying analog memory.

## LONGMEMORY

**What it is:** A long-delay-to-near-loop continuum with destructive aging in the stored material.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **LENGTH** | Sets the memory/delay span | Shorter values behave like a delay; longer values move toward captured-loop territory. |
| `DEPTH` → **MEMORY** | Increases persistence and aging behavior | Low values forget quickly; higher values retain material longer while allowing accumulated degradation. |
| `SHIFT_DEPTH` → **MIX** | Dry/wet level | Raise for more of the remembered layer. |

### Quick start

Set LENGTH long, bring MEMORY up until phrases begin to persist, then tune MIX so the stored layer sits behind or competes with the live input.

## SHARD

**What it is:** A dual-grain pitch field. Unlike SIDEBAND, this is time-domain pitch shifting rather than frequency translation.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **PITCH** | Moves the grain field through downward, near-unity, and upward pitch regions | Sweep slowly and listen for the central near-unity region before moving into obvious transposition. |
| `DEPTH` → **GRAIN** | Changes grain/window character and texture | Lower for smoother shifting; higher for more obvious grain structure and fragmentation. |
| `SHIFT_DEPTH` → **MIX** | Dry/wet level | Blend dry for doubled pitch effects or raise toward wet for a transformed voice. |

### Quick start

Use a sustained single note, put PITCH near the center, set GRAIN low, then move PITCH upward or downward. Increase GRAIN only when you want the granular mechanism to become audible as part of the sound.

---

# Human Soon RevFX

All current Human Soon RevFX use `SHIFT_DEPTH` as **MIX**.

## ABYSS

**What it is:** A large modulated four-line feedback-delay-network reverb aimed at huge non-static spaces without uncontrolled runaway.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **DECAY** | Controls reverb tail length | Start low-to-mid and raise for increasingly long spaces. |
| `DEPTH` → **SPACE** | Controls the spatial/modulation character of the FDN | Low for a tighter room-like field; high for wider, more animated space. |
| `SHIFT_DEPTH` → **MIX** | Dry/wet level | Keep lower for send-like ambience or raise for full immersion. |

### Quick start

Use a short pluck, set DECAY around the middle, then raise SPACE until the tail stops feeling like a simple static echo network.

## AUREOLE

**What it is:** A bounded shimmer reverb: a feedback-delay network with an octave-shifted halo injected into its feedback structure.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **DECAY** | Controls tail length | Set the base space first. |
| `DEPTH` → **HALO** | Controls how much octave-shifted shimmer enters the feedback field | Low gives a normal-ish reverb bed; higher values make the rising halo more obvious. |
| `SHIFT_DEPTH` → **MIX** | Dry/wet level | Use moderate MIX to keep the source defined under the shimmer. |

### Quick start

Set DECAY around 50%, MIX around 30–50%, then slowly raise HALO until the octave layer blooms behind the original note.

## NEBULA

**What it is:** A four-grain stochastic diffusion-cloud reverb. It is designed to dissolve input into a moving granular atmosphere rather than a conventional room model.

| Function | What it does | How to use it |
|---|---|---|
| `TIME` → **SPACE** | Controls the overall cloud/spatial span | Lower for a tighter cloud; higher for a more extended field. |
| `DEPTH` → **CLOUD** | Controls granular cloud density/strength | Low leaves more identifiable events; high turns the signal into a denser diffuse texture. |
| `SHIFT_DEPTH` → **MIX** | Dry/wet level | Lower for a cloud behind the source; high for near-total dissolution. |

### Quick start

Use a short melodic phrase, set SPACE around the middle, increase CLOUD until individual repeats disappear into texture, then set MIX according to how much of the original phrase should remain recognizable.

---

# Practical operating notes

- Start extreme feedback, instability, pitch, and degradation effects at conservative monitor level.
- `MIX=0` is the safest starting point for DelFX/RevFX when loading an unfamiliar setting; bring the wet path up gradually.
- Test modulation effects on sustained tones first; test delays on short plucks/percussion; test reverbs on both transients and sustained material.
- The current M1 binaries are the hardware-validated baseline. Any M2+ build that changes DSP behavior should be treated as a new test candidate until it passes its QA sheet.
- Project-specific engineering details and future milestones remain in each unit's `SPEC.md`, `docs/test-plan.md`, and `docs/qa-sheet.md` where present.
