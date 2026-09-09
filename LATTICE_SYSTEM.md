# Human Soon LATTICE System

Target: **original Korg Nu:Tekt NTS-1 digital kit MkI**.

LATTICE is a three-stage generative effects system designed around one musical behavior: **a simple incoming tone should become a melodic fragment field, then a clocked playback swarm, then a space around that swarm.**

```text
AUDIO IN
   |
   v
LATTICE CORE   (custom modfx)
 composition: rotating microloop ecosystem + interval rules
   |
   v
LATTICE ECHO   (custom delfx)
 multiplication: clocked fragment playback modes
   |
   v
LATTICE SPACE  (custom revfx)
 environment: dark drifting stereo reverb
```

## Clean-room inspiration

The high-level reference is the musical behavior described in public Hologram Electronics Microcosm documentation: short repeated samples, multiple loopers, playback-speed relationships, clocked/rearranged material, delay, granular textures and a separate stereo reverb. LATTICE does not copy Microcosm algorithms, firmware, presets, UI, names or reverse-engineered behavior. The scheduler, interval rules, playback modes, stereo layout and reverb network here are original.

## 0.2 architecture

### CORE — composer (`modfx`)
- TIME = **ACTIVITY**
- DEPTH = **PATTERN**
- 32 loop states remain available as a population.
- Only up to 16 are audible at once.
- Higher ACTIVITY increasingly randomizes capture age, loop length, entry wait, lifetime and stereo zone.
- Voices outside the foreground rotate in and out instead of accumulating permanently.
- PATTERN remains deterministic/musical; randomness changes geometry, not the harmonic rule.
- Output uses density-aware normalization plus a gentle soft limiter rather than the previous hard clamp.

### ECHO — clocked playback engine (`delfx`)
- TIME = **CLOCK**
- DEPTH = **PLAYBACK MODE**
- SHIFT+DEPTH = **MIX**

CLOCK divisions, low to high:
1. 1/32
2. 1/16T
3. 1/16
4. 1/8T
5. 1/8
6. 1/4T
7. 1/4
8. 1/2

PLAYBACK MODE, low to high:
1. **FORWARD**
2. **REVERSE**
3. **PING-PONG**
4. **STUTTER**

ECHO captures short recent fragments into four slots, then schedules them on the host BPM. Clock/mode changes are latched on future events rather than moving live read heads. Each captured fragment can persist for multiple generations, creating many arrivals without a continuously unstable feedback tap field.

### SPACE — environment (`revfx`)
- TIME = **SPACE**
- DEPTH = **DRIFT**
- SHIFT+DEPTH = **MIX**

SPACE is intentionally simpler than the previous CLOUD concept. It uses irregular early reflections, pre-delay and a four-line FDN. DRIFT adds slow stereo movement, small delay modulation and slightly darker damping. It should sound like a large room that never sits completely still, not a third generative processor.

## Hardware observations that drove 0.2

Physical original NTS-1 MkI testing of the previous versions found:
- CORE 0.1-2: the useful lower/mid ACTIVITY region exposed separate loops, while the upper range consolidated into a loud steady/beeping tone. The 32-voice code itself ran, but the simultaneous deterministic summing was not musically useful.
- ECHO 0.1-1: aggressive knob movement could produce crunch or complete signal loss. The continuously moving 12-tap design is therefore retired.
- CLOUD 0.1-1: audible but still more complex than needed and not close enough to the intended simple spatial role.

These are hardware-reported sound/behavior observations, not proof of universal MkI behavior.

## Hardware observation vs documentation

The project owner's physical original NTS-1 MkI reports system firmware `N1.2` (v1.20) and panel `P1.0`. In normal long-term use, that unit has audibly run a custom ModFX, custom DelFX and custom RevFX together. This was explicitly re-tested with BALLISTIC (`delfx`) and ABYSS (`revfx`) on fresh external Chompi audio, with both processors responding independently.

Korg's public SDK documentation states that first-generation NTS-1 custom delay and reverb effects share a runtime and ordinarily only one of those two can be custom at a time. Because the observed target unit differs from that simple documented limitation, LATTICE treats simultaneous three-stage operation as **hardware-observed behavior on this specific unit**, not a universal claim. Individual-stage QA remains mandatory before judging the full chain.

## 0.2 identity gates

1. CORE: the lower/mid range still exposes recognizably separate melodic microloops.
2. CORE: maximum ACTIVITY becomes a moving swarm, not a steady beep or clipped mass.
3. ECHO: CLOCK changes rhythmically restructure repeats without crunching or dropping the signal.
4. ECHO: FORWARD / REVERSE / PING-PONG / STUTTER are plainly distinguishable.
5. ECHO: repeated fragments form a dense field over time while the direct signal remains audible.
6. SPACE: a clear reverb tail is audible at moderate MIX without crushing the source.
7. SPACE: DRIFT changes movement/darkness without reading as chorus.
8. Full chain: composition -> multiplication -> environment remains perceptible as three separate jobs.
