# VECTORFILTER M2 Hardware QA — Refined SVF Core

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**  
Build: **VECTORFILTER 0.2-0**

## Record
- Date / tester: ____________________
- Firmware / Librarian: ____________________
- Commit / binary: ____________________
- Source / monitoring: ____________________

## Baseline
- [ ] Loads/selects safely
- [ ] TIME remains CUTOFF; DEPTH remains LP → BP → HP VECTOR
- [ ] Mono remains centered; stereo orientation preserved

## M2 — TPT SVF stability / response
Test sine, saw, bass, pad, drums and full-range input.
- [ ] CUTOFF 0 / 10 / 25 / 50 / 75 / 100% is monotonic and smooth
- [ ] VECTOR 0% sounds low-pass
- [ ] VECTOR 50% sounds band-pass with stronger but bounded resonance
- [ ] VECTOR 100% sounds high-pass
- [ ] Morph through 25 / 50 / 75% has no abrupt level hole or jump
- [ ] High cutoff + center VECTOR does not ring uncontrollably
- [ ] Low cutoff on bass retains stable state without motorboating/DC
- [ ] Rapid CUTOFF movement has less zippering / instability than M1

## Stress
- [ ] Rapid TIME min↔max 30 s
- [ ] Rapid DEPTH min↔max 30 s
- [ ] Both controls rapidly swept together
- [ ] High cutoff + VECTOR center for 30 min
- [ ] No runaway, mute, DC, or NTS-1 lockup

## Result
- [ ] PASS — M2 hardware validated
- [ ] PASS WITH NOTES
- [ ] FAIL / RETEST

Notes:
