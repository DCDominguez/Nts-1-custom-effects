# CARRIER Hardware QA Sheet

Status: **NOT YET HARDWARE VALIDATED**

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**

## Build record

- Build / commit: ____________________
- `.ntkdigunit` filename: `carrier.ntkdigunit`
- NTS-1 firmware: ____________________
- Sound Librarian version: ____________________
- Test date: ____________________
- Tester: ____________________

## Test setup

- Source(s): ____________________
- Controller / MIDI setup: ____________________
- Monitoring chain: ____________________
- Output level: ____________________
- Stereo / mono monitoring used: ____________________

## M0 — load / pass-through

- [ ] Unit loads into a modulation FX user slot
- [ ] Unit can be selected without lockup
- [ ] TIME=0, DEPTH=0 is effectively dry
- [ ] Left/right orientation correct
- [ ] Digital silence remains silent
- [ ] No unexpected DC / blast on selection

Result: PASS / FAIL
Notes:

---

## M1 — FREQUENCY / TIME

Test with DEPTH around 50%.

- [ ] TIME 0%
- [ ] TIME 5%
- [ ] TIME 10%
- [ ] TIME 20%
- [ ] TIME 35%
- [ ] TIME 50%
- [ ] TIME 65%
- [ ] TIME 80%
- [ ] TIME 100%
- [ ] Low end produces smooth tremolo-rate movement
- [ ] Frequency rises continuously
- [ ] Audio-rate range produces stable sidebands
- [ ] Normal knob motion does not hard-reset phase
- [ ] No stuck carrier

Observed carrier-frequency notes / calibration:

Result: PASS / FAIL
Notes:

---

## M1 — POLARITY / DEPTH

- [ ] DEPTH 0% = dry
- [ ] DEPTH 25% = partial AM
- [ ] DEPTH 50% = full unipolar AM
- [ ] DEPTH 75% = AM/ring transition
- [ ] DEPTH 100% = bipolar ring modulation
- [ ] Transition is continuous
- [ ] Output remains bounded
- [ ] No unexpected inversion at AM midpoint

Result: PASS / FAIL
Notes:

---

## M2 — sideband split

Use mono input duplicated to both channels.

- [ ] DEPTH <= 50% remains essentially centered
- [ ] DEPTH > 50% widens progressively
- [ ] Neither channel disappears
- [ ] No uncontrolled stereo flips
- [ ] Full-ring mono collapse is acceptable
- [ ] Stereo input remains intelligible

Result: PASS / FAIL
Notes:

---

## Aliasing / source matrix

Representative settings tested on:

- [ ] sine
- [ ] triangle
- [ ] saw
- [ ] square
- [ ] short pluck
- [ ] bass-heavy source
- [ ] full stereo AUDIO IN material

- [ ] High-TIME saw/square aliasing documented
- [ ] No unexpected ultrasonic-looking instability / audible blast

Result: PASS / FAIL
Notes:

---

## Abuse / recovery

- [ ] Rapid TIME min/max sweep for 30 s
- [ ] Rapid DEPTH min/max sweep for 30 s
- [ ] Both controls rapidly alternated
- [ ] 30 s silence then transient resumes safely
- [ ] Switching away/back resumes safely
- [ ] Returning DEPTH to 0 restores dry signal

Result: PASS / FAIL
Notes:

---

## Stability

Worst case:

```text
TIME = 100%
DEPTH = 100%
bright input material
```

- [ ] 10 minutes
- [ ] 30 minutes
- [ ] NTS-1 remains responsive
- [ ] No runaway / blast
- [ ] No persistent mute
- [ ] No state corruption after switching

Result: PASS / FAIL

## Defects

| Severity | Setting / source | Reproduction steps | Observed behavior | Blocking? |
|---|---|---|---|---|
| | | | | |

## Final disposition

- [ ] PASS — hardware validated for current milestone
- [ ] PASS WITH NOTES — non-blocking issues only
- [ ] FAIL — revision required
- [ ] RETEST REQUIRED

Tester notes:

---
