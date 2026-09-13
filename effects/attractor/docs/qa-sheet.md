# ATTRACTOR 0.3 Hardware QA Sheet

Status: **HOST TESTED; HARDWARE RETEST REQUIRED**

## Build record

- Build / commit: ____________________
- `.ntkdigunit`: `attractor.ntkdigunit`
- NTS-1 firmware: ____________________
- Sound Librarian: ____________________
- Date / tester: ____________________

## Transparency and continuity

- [ ] DEPTH 0 is sample-for-sample dry
- [ ] No click/step during sustained notes at any TIME setting
- [ ] Wall impacts reverse direction without discontinuity
- [ ] Silence remains silent

## ENERGY / TIME

Test 0 / 25 / 50 / 75 / 100%.

- [ ] Low values drift and bounce slowly
- [ ] Speed and direction-change activity rise progressively
- [ ] Momentum remains audible between force changes
- [ ] 100% stays fluid and does not become tremolo/stutter

## PULL / DEPTH

- [ ] 0% is transparent
- [ ] 50% produces obvious left-to-right travel
- [ ] 100% produces wide movement without hard L/R switching
- [ ] Mono input travels clearly
- [ ] Stereo input travels as one image while retaining width
- [ ] Output stays bounded

## Reset and abuse

- [ ] Re-selecting ATTRACTOR restarts the same trajectory
- [ ] Suspend/resume restarts the same trajectory
- [ ] Rapid TIME sweep for 30 seconds
- [ ] Rapid DEPTH sweep for 30 seconds
- [ ] 30 minutes at ENERGY 100%, PULL 100%

## Defects

| Severity | Setting/source | Reproduction | Observed | Blocking? |
|---|---|---|---|---|
| | | | | |

## Final disposition

- [ ] PASS
- [ ] PASS WITH NOTES
- [ ] FAIL
- [ ] RETEST REQUIRED

