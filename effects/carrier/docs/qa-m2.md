# CARRIER M2 Hardware QA — High-Rate Quality Pass

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**  
Build: **CARRIER 0.2-0**

## Record
- Date / tester: ____________________
- Firmware / Librarian: ____________________
- Commit / binary: ____________________
- Test source: ____________________

## Baseline
- [ ] Loads/selects safely
- [ ] TIME=0, DEPTH=0 is effectively dry
- [ ] DEPTH path remains continuous Dry → AM → Ring
- [ ] Stereo phase split still appears only in the ring region

## M2 — high-rate quality
Use sine first, then bright saw/square/pluck.
- [ ] TIME low end remains smooth tremolo
- [ ] Transition into audio-rate modulation is continuous
- [ ] High-rate region remains responsive with no CPU glitching
- [ ] Bright sources at TIME 75–100% show acceptable aliasing for MkI
- [ ] Compare against M1: high-rate output is equal or cleaner, not harsher from processing artifacts
- [ ] No obvious gain pumping at the 2x-processing threshold
- [ ] Maximum carrier range still feels musically useful despite the safer upper limit

## POLARITY / stereo
- [ ] DEPTH 0 / 25 / 50 / 75 / 100% retains expected Dry / AM / Ring progression
- [ ] High DEPTH creates bounded stereo split
- [ ] External mono collapse remains usable

## Abuse / stress
- [ ] Rapid TIME min↔max for 30 s
- [ ] Rapid DEPTH min↔max for 30 s
- [ ] TIME=100%, DEPTH=100%, bright input for 30 min
- [ ] No blast, mute, runaway, or effect-state corruption

## Result
- [ ] PASS — M2 hardware validated
- [ ] PASS WITH NOTES
- [ ] FAIL / RETEST

Notes:
