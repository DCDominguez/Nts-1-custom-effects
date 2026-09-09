# PARALLAX M2 Hardware QA — One True Pitch Voice

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**  
Build: **PARALLAX 0.2-0**

## Record
- Date / tester: ____________________
- Firmware / Librarian: ____________________
- Commit / binary: ____________________
- Test source / monitoring: ____________________

## Baseline regression
- [ ] Loads/selects safely
- [ ] `TIME/SPREAD`, `DEPTH/DIVERGENCE`, `SHIFT+DEPTH/MIX` still work
- [ ] Four arrivals remain audible at high MIX
- [ ] Stereo anchors remain intentional

## M2 — Voice A pitch engine
Use a sustained sine/triangle, MIX high, then repeat with saw/pluck.
- [ ] `DIVERGENCE=0` keeps Voice A essentially unison
- [ ] Increasing DIVERGENCE introduces a stable downward micro-detune toward ~-9 cents
- [ ] Pitch offset sounds stable rather than only like periodic vibrato
- [ ] No grain/reset clicks on sustained notes
- [ ] No large gain pulses at dual-head crossfades
- [ ] Voice A remains spatially left-anchored while pitch engine runs
- [ ] Other three M1 chorus voices remain intact

## Stress / compatibility
- [ ] SPREAD min/max with DIVERGENCE min/max
- [ ] MIX 0 / 50 / 100%
- [ ] Rapid SPREAD + DIVERGENCE movement recovers safely
- [ ] External mono collapse has no severe recurring cancellation
- [ ] Bright saw/pluck does not produce unacceptable new artifacts
- [ ] 30-minute worst-case run stable

## Result
- [ ] PASS — M2 hardware validated
- [ ] PASS WITH NOTES
- [ ] FAIL / RETEST

Notes:
