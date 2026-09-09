# LATTICE CORE

LATTICE CORE is a clean-room microloop composition engine for the original Korg Nu:Tekt NTS-1 MkI.

It is intentionally a `modfx` unit even though it uses delay memory internally. That leaves the downstream `delfx` and `revfx` stages available for LATTICE ECHO and LATTICE CLOUD.

## Musical idea

A recent slice of the input is captured into several short repeating windows. Each newly captured microloop receives an interval from a deterministic musical rule. A single tone can therefore grow into a repeating melodic texture without MIDI note generation.

## Controls

| NTS-1 | Role |
|---|---|
| TIME | **LOOPS** — 0 = bypass; otherwise activates 1 to 8 simultaneous microloop voices |
| DEPTH | **PATTERN** — selects one of 8 stepped interval rules |

## M1 pattern bank

0. PLAIN — all microloops at unison
1. FIFTHS — every 3rd captured loop +7 semitones
2. TOWERS — every 5th +12, every 3rd +7
3. FALLING — every 5th -12, every 3rd -5
4. ORBIT — repeating `0,+7,0,+12,+7,0,-5,+12`
5. GLASS — repeating `+12,0,+7,0,-12,+7,+12,-5`
6. COUNTER — repeating `0,+7,-5,+12,-12,+5,-7,0`
7. HUMAN — asymmetric 12-event Human Soon rule

Microloop lengths are intentionally non-uniform and stereo positions are fixed but asymmetric. Each captured fragment repeats 2–4 times before that voice takes a fresh slice from recent input.

This is not a Microcosm clone. Public descriptions of Microcosm's use of short repeated samples, pitch shifting, delay and looping are only the high-level inspiration; the scheduler, interval rules, voice layout and DSP are original to this project.
