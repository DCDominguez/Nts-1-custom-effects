# Human Soon LATTICE System

Target: **original Korg Nu:Tekt NTS-1 digital kit MkI**.

LATTICE is a three-stage generative effects system designed around one musical behavior: **a simple incoming tone should be able to grow into a melodic phrase, then into echoes and atmosphere.**

```text
AUDIO IN
   |
   v
LATTICE CORE   (custom modfx)
 microloop composition + interval rules
   |
   v
LATTICE ECHO   (custom delfx)
 promote selected generated fragments into long echoes
   |
   v
LATTICE CLOUD  (custom revfx)
 dissolve the phrase into a harmonic ambient field
```

## Clean-room inspiration

The high-level reference is the musical behavior of Hologram Electronics Microcosm: public product documentation describes short repeated samples, pitch shifting, delay, looping, granular textures and a separate stereo reverb. LATTICE does not copy Microcosm algorithms, firmware, presets, UI, names or reverse-engineered behavior. The microloop scheduler, interval rules, event selector, stereo layout and reverb network here are original.

## Controls

### CORE (`modfx`)
- TIME = **LOOPS**
- DEPTH = **PATTERN**

### ECHO (`delfx`)
- TIME = **DISTANCE**
- DEPTH = **PICK**
- SHIFT+DEPTH = **MIX**

### CLOUD (`revfx`)
- TIME = **SPACE**
- DEPTH = **COALESCE**
- SHIFT+DEPTH = **MIX**

## Hardware observation vs documentation

The project owner's physical original NTS-1 MkI reports system firmware `N1.2` (v1.20) and panel `P1.0`. In normal long-term use, that unit has audibly run a custom ModFX, custom DelFX and custom RevFX together. This was explicitly re-tested with BALLISTIC (`delfx`) and ABYSS (`revfx`) on fresh external Chompi audio, with both processors responding independently.

Korg's public SDK documentation has language suggesting limitations around first-generation custom delay/reverb runtime sharing. Because the observed hardware behavior differs from a simple reading of that documentation, LATTICE treats simultaneous three-stage operation as **hardware-observed behavior on this target unit**, not as a universal claim for every MkI configuration. Full-chain QA remains mandatory.

## M1 identity gates

1. CORE: one sustained note becomes a recognizably melodic microloop phrase within seconds.
2. ECHO: only selected generated fragments are promoted into long trails; it must not collapse into an ordinary always-on delay.
3. CLOUD: low COALESCE preserves fragment identity; high COALESCE fuses the phrase into a continuous harmonic atmosphere.
4. Full chain: composition → selective memory → atmosphere remains perceptible as three stages rather than one generic wash.
