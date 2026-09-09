# PARALLAX M2.1 Hardware QA — Spatial Aperture Revision

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**  
Build: **PARALLAX 0.2-1**

Hardware feedback on 0.2-0: functional, but the effect read too much like a chorus. This retest specifically gates the wider time/stereo identity.

## Record
- Date / tester: ____________________
- Firmware / Librarian: ____________________
- Commit / binary: ____________________
- Test source / monitoring: ____________________

## Baseline regression
- [ ] Loads/selects safely
- [ ] `TIME/SPREAD`, `DEPTH/DIVERGENCE`, `SHIFT+DEPTH/MIX` still work
- [ ] Voice A still gains a stable downward micro-detune as DIVERGENCE rises
- [ ] No grain/reset clicks on sustained material
- [ ] No large gain pulses at read-head crossfades

## M2.1 — temporal aperture
Use a dry pluck or short percussion hit first, then a pad/chord.
- [ ] SPREAD=0 remains a usable tight doubler / short ensemble
- [ ] Around SPREAD=25% the taps begin separating beyond ordinary chorus behavior
- [ ] Around SPREAD=50% at least the later arrivals are heard as distinct echoes
- [ ] SPREAD=100% produces clearly separated arrivals near ~40 / 160 / 360 / 620 ms
- [ ] High SPREAD sounds like a spatial multi-delay, not mainly like modulation/chorus
- [ ] Longest tap does not wrap, disappear, or corrupt the buffer

## M2.1 — stereo aperture
Use a mono source and monitor in stereo.
- [ ] DIVERGENCE=0 remains relatively compact but not collapsed
- [ ] DIVERGENCE=50% produces clearly separated stereo positions
- [ ] DIVERGENCE=100% places outer arrivals near the left/right edges
- [ ] The arrival path reads approximately LEFT EDGE → RIGHT MID → LEFT MID → RIGHT EDGE
- [ ] Stereo image is balanced overall rather than permanently leaning left or right
- [ ] Mono collapse is usable with no severe recurring cancellation

## Interaction
- [ ] SPREAD=100% + DIVERGENCE=0 gives temporal width without excessive stereo width
- [ ] SPREAD=0 + DIVERGENCE=100% gives a tight but wide doubler
- [ ] SPREAD=100% + DIVERGENCE=100% gives the intended full 3D field
- [ ] MIX 0 / 25 / 50 / 75 / 100% behaves continuously
- [ ] Rapid SPREAD and DIVERGENCE movement recovers safely

## Stability
- [ ] Bright transient source does not produce runaway level or buffer artifacts
- [ ] Sustained high-level source remains bounded
- [ ] 30-minute SPREAD=100% / DIVERGENCE=100% / MIX=100% run is stable

## Result
- [ ] PASS — 0.2-1 spatial aperture hardware validated
- [ ] PASS WITH NOTES
- [ ] FAIL / RETEST

Notes:
