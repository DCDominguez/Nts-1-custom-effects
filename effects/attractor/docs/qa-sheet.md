# ATTRACTOR Hardware QA Sheet

Status: **NOT YET HARDWARE VALIDATED**

## Build record
- Build / commit: ____________________
- `.ntkdigunit`: `attractor.ntkdigunit`
- NTS-1 firmware: ____________________
- Sound Librarian: ____________________
- Date / tester: ____________________

## Load / bypass behavior
- [ ] Loads/selects normally
- [ ] DEPTH=0 preserves original stereo signal
- [ ] Silence remains silent

## RATE / TIME
Test 0 / 25 / 50 / 75 / 100%.
- [ ] Motion speed increases
- [ ] Motion never becomes a simple obvious repeating sine
- [ ] No zipper/click on normal sweep

## ORBIT / DEPTH
- [ ] 0% centered/original
- [ ] 25/50/75% progressively wider motion
- [ ] 100% remains bounded
- [ ] Neither channel permanently disappears
- [ ] Mono collapse acceptable

## Abuse / stability
- [ ] Rapid TIME sweep 30 s
- [ ] Rapid DEPTH sweep 30 s
- [ ] Switch away/back recovers safely
- [ ] 30-minute RATE=100%, ORBIT=100% stability

## Defects
| Severity | Setting/source | Reproduction | Observed | Blocking? |
|---|---|---|---|---|
| | | | | |

## Final disposition
- [ ] PASS
- [ ] PASS WITH NOTES
- [ ] FAIL
- [ ] RETEST REQUIRED
