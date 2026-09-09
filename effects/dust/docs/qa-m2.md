# DUST M2 Hardware QA — Precision Reduction Engine

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**  
Build: **DUST 0.2-0**

## Record
- Date / tester: ____________________
- Firmware / Librarian: ____________________
- Commit / binary: ____________________
- Test source: ____________________

## Baseline
- [ ] Loads/selects safely
- [ ] TIME=0, DEPTH=0 is effectively dry
- [ ] Digital silence remains silent
- [ ] L/R orientation correct

## M2 — RATE
Use sustained sine, saw, drums, and full-range material.
- [ ] TIME 0 / 10 / 25 / 50 / 75 / 100% increases sample-hold destruction progressively
- [ ] Low end has more usable resolution than M1
- [ ] Ordinary TIME movement has no stuck sample or hard discontinuity
- [ ] Returning TIME to 0 restores full-rate behavior

## M2 — DAMAGE / quantizer interpolation
- [ ] DEPTH 0 / 10 / 25 / 50 / 75 / 100% becomes progressively coarser
- [ ] Bit-depth transitions do not sound like abrupt staircase mode switches
- [ ] Low-level signals do not develop obvious DC bias
- [ ] Silence does not generate dither/noise or stale held values

## Stereo fracture
- [ ] Mild settings remain centered on mono input
- [ ] Strong RATE+DAMAGE produces bounded L/R alias decorrelation
- [ ] Neither channel disappears or flips unpredictably
- [ ] External mono collapse remains usable

## Abuse / stability
- [ ] Rapid TIME/DEPTH min↔max for 30 s recovers cleanly
- [ ] Silence 30 s → transient: no stale-sample blast
- [ ] TIME=100%, DEPTH=100% for 30 min stable

## Result
- [ ] PASS — M2 hardware validated
- [ ] PASS WITH NOTES
- [ ] FAIL / RETEST

Notes:
