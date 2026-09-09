# IRONROT M2 Hardware QA — Multi-Stage Corrosion

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**  
Build: **IRONROT 0.2-1**

## Record
- Date / tester: ____________________
- Firmware / Librarian: ____________________
- Commit / binary: ____________________
- Source / monitoring: ____________________

## Baseline
- [ ] Loads/selects safely
- [ ] TIME remains CHARACTER; DEPTH remains CORROSION
- [ ] DEPTH=0 is effectively dry
- [ ] Digital silence remains silent

## M2 — staged corrosion
Test bass, saw, guitar-like source, drums and full-range material.
- [ ] CHARACTER low retains more body/darkness
- [ ] CHARACTER high emphasizes edge/high-frequency attack
- [ ] CORROSION 0 → 25 → 50 → 75 → 100% increases nonlinear severity progressively
- [ ] Multiple stages sound more texturally complex than a single hard clip
- [ ] Strong settings retain bass rather than collapsing into thin fizz
- [ ] Zero-input/silence produces no DC offset from asymmetric stage
- [ ] Returning CORROSION to 0 restores the dry signal cleanly

## M2.1 feedback retest — texture vs loudness
The first M2 hardware pass was reported as working but perceptually **mostly getting louder**. Version 0.2-1 adds a stronger crossover/fracture stage and attenuation-only envelope matching.

Use a steady synth note or loop and compare CORROSION at 0 / 25 / 50 / 75 / 100% without changing any upstream/downstream level.
- [ ] CORROSION clearly changes harmonic texture, not only loudness
- [ ] 50–100% produces audible grit/fracture on sustained material
- [ ] CHARACTER still moves the damage from body-heavy/dark to edge-heavy/bright
- [ ] Perceived loudness at 100% is not materially higher than at 0%
- [ ] No obvious pumping from the slow level matcher
- [ ] Extreme CORROSION may be slightly quieter, but remains musically usable

## Abuse / stability
- [ ] Rapid CHARACTER sweep 30 s
- [ ] Rapid CORROSION min↔max 30 s
- [ ] Hot input transients do not create persistent state corruption
- [ ] Maximum CHARACTER + CORROSION for 30 min stable

## Result
- [ ] PASS — M2 hardware validated
- [ ] PASS WITH NOTES
- [ ] FAIL / RETEST

Notes:
