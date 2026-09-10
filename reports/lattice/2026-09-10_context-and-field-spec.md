# LATTICE Update Report — Context Archive and FIELD Specification

## 1. Status

- **Date:** 2026-09-10
- **Branch:** `lattice-suite`
- **Result classification:** **DESIGN ONLY**
- **Hardware status:** no new physical MkI validation in this report

This report records the repository-context consolidation and the current architectural pivot before the next DSP build.

## 2. Why this change happened

Physical original NTS-1 MkI testing established that the previous three-custom-unit LATTICE system could not be load-balanced reliably under aggressive use. CORE + ECHO + SPACE could each run, and pairwise use was substantially more usable, but the full chain repeatedly crossed into distortion/collapse under load.

At the same time, the standalone ECHO/SPACE behavior still had musical problems:

- ECHO could feel too subtle in the useful range;
- some echo behavior could appear to continue indefinitely because the system kept replenishing new captured material even when individual fragment generations were finite;
- SPACE did not always feel reliably present or sufficiently full/ethereal;
- the system needed to sound more deliberate and less like unrelated stochastic events;
- source clarity and sound quality needed to become explicit acceptance criteria rather than secondary concerns.

The architectural response is to retire the three-independent-custom-unit configuration as the preferred MkI target and move to a two-unit system:

`CORE [custom ModFX] -> FIELD [custom DelFX] -> optional Korg internal Reverb`

FIELD combines rhythmic/melodic echo behavior with its own spatial bloom so the MkI is not asked to run a heavy custom Delay and heavy custom Reverb simultaneously.

## 3. What changed

Repository context was consolidated into three primary documents:

- `LATTICE_SYSTEM.md` — current architecture and doctrine;
- `LATTICE_HISTORY.md` — full narrative engineering/history context;
- `LATTICE_FIELD_SPEC.md` — current next-build implementation specification.

A reporting protocol was also added as `LATTICE_REPORTING.md` so future meaningful development steps leave a chronological repository record in `reports/lattice/`.

### Current FIELD design target

FIELD is a finite phrase engine rather than a conventional endless feedback delay.

Core rules include:

- maximum two active seed phrases;
- finite event grammar per seed;
- Forward / Reverse / Ping-Pong / Stutter playback identities;
- event-count and absolute-TTL termination;
- no wet self-recapture;
- foreground/background hierarchy;
- strong first and second responses;
- sparse, musically constrained pitch ghosts;
- generation-dependent bloom send;
- source/dry spine preserved until final mix;
- dynamic event dropping before foreground events are sacrificed;
- bounded feedback and mandatory return to a real rest state;
- safe parameter transitions at event boundaries;
- individual voice fault containment;
- runtime margin reserved for CORE + FIELD simultaneous use.

## 4. Sound-quality intent

The next FIELD build must sound **obvious, intentional, ethereal, and high-quality** rather than merely generative.

The desired phrase arc is:

`clear fragment -> strong answer -> stereo/pitched responses -> ghosts -> fat ethereal bloom -> silence`

The original source must remain easy to hear and understand. FIELD should not obtain audibility by simply turning up every wet component at once.

Sound hierarchy:

1. dry/source remains intelligible;
2. the first response is unmistakable at useful middle settings;
3. the second response is still clearly audible;
4. later events become increasingly atmospheric;
5. bloom sits behind the foreground rather than masking it.

Pitch movement should sound musical rather than cheap or aliased. Fifth/octave ghosts should be sparse and structurally assigned by phrase grammar.

The bloom target is a **warm BODY plus airy HALO**. It should own the space without becoming muddy, hollowing out the center, or making the original transient disappear.

Clicks, zipper noise, obvious wrap artifacts, DC buildup, uncontrolled sub-bass, limiter pumping, unstable resonance, and mono collapse are explicit failures rather than acceptable “experimental” texture.

## 5. Runtime and headroom intent

FIELD must be computationally lighter than running the former standalone ECHO and SPACE simultaneously.

The system should create perceived density through event scheduling, stereo placement, finite lifetimes, diffusion, and hierarchy rather than brute-force simultaneous voices.

When resources are constrained, low-priority ghosts/tails are dropped before the current foreground response.

Peak guards remain containment only. If a guard is audibly working constantly, the gain structure has failed its design goal.

## 6. Build/CI result

No FIELD implementation or CI build is included in this report. This is an architecture/specification checkpoint only.

## 7. Physical MkI result

No new physical test performed for FIELD yet.

Relevant prior hardware fact driving this report: the previous three-custom-unit LATTICE configuration remained unreliable under full load even after the 0.3 headroom/voice-count pass.

## 8. Result classification

**DESIGN ONLY**

The architecture/specification is ready to implement, but no FIELD binary has yet been physically validated.

## 9. What we learned

- “Finite generations” is not enough if the capture engine can keep replenishing itself indefinitely.
- More simultaneous events do not necessarily sound larger or more musical.
- Audibility must be designed through hierarchy, not cumulative gain.
- Randomness should vary a recognizable phrase grammar rather than replace it.
- Reverb/bloom should be continuously derived from actual phrase energy instead of depending on a separate trigger that may fail to fire.
- Sound fidelity, dry-source clarity, transient preservation, stereo integrity, and clean pitch playback are core features.
- The original NTS-1 MkI should be designed around its practical runtime behavior rather than forcing the three-custom-unit concept.

## 10. Next step

Implement the first LATTICE FIELD build directly from `LATTICE_FIELD_SPEC.md`.

Before physical hardware validation, the build must pass compile/package CI only. Hardware acceptance remains open until the original NTS-1 MkI demonstrates:

- obvious foreground response at useful middle settings;
- finite phrase lifetime;
- reliable bloom for every audible phrase;
- warm BODY + ethereal HALO;
- clear source audio and clean first/second echoes;
- no click/crunch/dropout during control changes;
- stable CORE + FIELD simultaneous operation;
- extended physical run without runaway behavior or collapse.
