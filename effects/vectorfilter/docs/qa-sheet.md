# VECTORFILTER Hardware QA Sheet

Status: **NOT YET HARDWARE VALIDATED**

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**

## Build record
- Build / commit: ____________________
- `.ntkdigunit`: `vectorfilter.ntkdigunit`
- NTS-1 firmware: ____________________
- Sound Librarian version: ____________________
- Date / tester: ____________________

## M0 — load / safety
- [ ] Loads into a modulation FX user slot
- [ ] Selects without blast, mute, or lockup
- [ ] Left/right orientation correct
- [ ] Digital silence remains silent

## CUTOFF / TIME
Test 0 / 10 / 25 / 50 / 75 / 100%.
- [ ] Sweep is monotonic low → high
- [ ] No hard zippering under normal movement
- [ ] Extreme settings remain bounded

## VECTOR / DEPTH
At several cutoff positions test 0 / 25 / 50 / 75 / 100%.
- [ ] 0% reads as low-pass
- [ ] 50% reads as band-pass
- [ ] 100% reads as high-pass
- [ ] Morph is continuous
- [ ] Center region has stronger resonance without runaway

## Stereo / abuse
- [ ] Mono source remains centered
- [ ] Stereo source keeps L/R orientation
- [ ] Rapid TIME min/max for 30 s
- [ ] Rapid DEPTH min/max for 30 s
- [ ] Simultaneous rapid sweeps recover safely

## Stability
Worst case: high cutoff, VECTOR around center.
- [ ] 10 minutes
- [ ] 30 minutes
- [ ] NTS-1 remains responsive
- [ ] No persistent DC, runaway, or mute

## Defects
| Severity | Setting/source | Reproduction | Observed behavior | Blocking? |
|---|---|---|---|---|
| | | | | |

## Final disposition
- [ ] PASS — hardware validated for current milestone
- [ ] PASS WITH NOTES
- [ ] FAIL — revision required
- [ ] RETEST REQUIRED
