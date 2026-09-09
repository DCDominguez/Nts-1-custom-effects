# ATTRACTOR M2 Hardware QA — Stable Chaotic Orbit

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**  
Build: **ATTRACTOR 0.2-0**

## Record
- Date / tester: ____________________
- Firmware / Librarian: ____________________
- Commit / binary: ____________________
- Source / monitoring: ____________________

## Baseline
- [ ] Loads/selects safely
- [ ] TIME remains RATE; DEPTH remains ORBIT
- [ ] DEPTH=0 is effectively dry
- [ ] Stereo orientation preserved

## M2 — chaotic orbit
Use mono first, then stereo material.
- [ ] RATE 0 / 25 / 50 / 75 / 100% changes evolution speed continuously
- [ ] ORBIT 0 / 25 / 50 / 75 / 100% changes stereo excursion continuously
- [ ] Motion does not settle into an obvious repeating LFO cycle over 2–5 minutes
- [ ] Orbit crosses the center naturally instead of only jumping L↔R
- [ ] Constant-power pan keeps perceived level reasonably stable through the field
- [ ] No hard image flips or channel dropouts
- [ ] Stereo material keeps recognizable L/R orientation at moderate ORBIT
- [ ] External mono collapse has no severe level pumping

## Long-session / abuse
- [ ] Rapid RATE min↔max 30 s
- [ ] Rapid ORBIT min↔max 30 s
- [ ] RATE=100%, ORBIT=100% for 30 min
- [ ] No lockup, blast, permanent mute, or state collapse into fixed pan

## Result
- [ ] PASS — M2 hardware validated
- [ ] PASS WITH NOTES
- [ ] FAIL / RETEST

Notes:
