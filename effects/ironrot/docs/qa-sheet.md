# IRONROT Hardware QA Sheet

Status: **NOT YET HARDWARE VALIDATED**

## Build record
- Build / commit: ____________________
- `.ntkdigunit`: `ironrot.ntkdigunit`
- NTS-1 firmware: ____________________
- Sound Librarian: ____________________
- Date / tester: ____________________

## Load / safety
- [ ] Loads/selects normally
- [ ] DEPTH=0 is effectively dry
- [ ] Silence remains silent
- [ ] No DC-like blast on selection

## CHARACTER / TIME
Test 0 / 25 / 50 / 75 / 100% with DEPTH 50%.
- [ ] Low values are darker/rounder
- [ ] High values are brighter/more eroded
- [ ] Sweep is continuous

## CORROSION / DEPTH
Test 0 / 10 / 25 / 50 / 75 / 100% at three CHARACTER positions.
- [ ] Drive increases progressively
- [ ] Maximum remains bounded
- [ ] No channel imbalance

## Abuse / stability
- [ ] Rapid TIME sweeps
- [ ] Rapid DEPTH sweeps
- [ ] Both controls for 30 s
- [ ] Hot transient source does not latch/blast
- [ ] 30-minute max-CORROSION stability pass

## Defects
| Severity | Setting/source | Reproduction | Observed | Blocking? |
|---|---|---|---|---|
| | | | | |

## Final disposition
- [ ] PASS
- [ ] PASS WITH NOTES
- [ ] FAIL
- [ ] RETEST REQUIRED
