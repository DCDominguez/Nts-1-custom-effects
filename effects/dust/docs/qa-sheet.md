# DUST Hardware QA Sheet

Status: **NOT YET HARDWARE VALIDATED**

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**

## Build record

- Build / commit: ____________________
- `.ntkdigunit` filename: `dust.ntkdigunit`
- NTS-1 firmware: ____________________
- Sound Librarian version: ____________________
- Test date: ____________________
- Tester: ____________________

## Test setup

- Source(s): ____________________
- Monitoring chain: ____________________
- Output level: ____________________
- Stereo / mono monitoring used: ____________________

## M0 — load / pass-through

- [ ] Unit loads into a modulation FX user slot
- [ ] Unit can be selected without lockup
- [ ] TIME=0, DEPTH=0 passes usable dry signal
- [ ] Left/right orientation correct
- [ ] No unexpected DC / blast on selection

Result: PASS / FAIL
Notes:

---

## M1 — RATE

- [ ] 0%
- [ ] 10%
- [ ] 25%
- [ ] 50%
- [ ] 75%
- [ ] 100%
- [ ] Aliasing increases progressively
- [ ] Normal knob movement does not hard-click
- [ ] Returning TIME to 0 clears heavy sample-hold behavior

Result: PASS / FAIL
Notes:

---

## M1 — DAMAGE

- [ ] 0%
- [ ] 10%
- [ ] 25%
- [ ] 50%
- [ ] 75%
- [ ] 100%
- [ ] DEPTH=0 is effectively dry
- [ ] Quantization increases progressively
- [ ] Maximum setting remains bounded
- [ ] Digital silence remains silent

Result: PASS / FAIL
Notes:

---

## M2 — stereo fracture

- [ ] Mono input remains centered at mild settings
- [ ] Strong settings produce audible stereo decorrelation
- [ ] Neither channel disappears
- [ ] No uncontrolled stereo flips
- [ ] Mono collapse acceptable

Result: PASS / FAIL / N/A
Notes:

---

## Abuse / recovery

- [ ] Rapid TIME min/max sweep
- [ ] Rapid DEPTH min/max sweep
- [ ] Both controls rapidly alternated for 30 s
- [ ] 30 s silence then transient resumes safely
- [ ] Switching away/back resumes safely
- [ ] Returning controls to 0 restores normal signal

Result: PASS / FAIL
Notes:

---

## Stability

Worst case:

```text
TIME = 100%
DEPTH = 100%
```

- [ ] 10 minutes
- [ ] 30 minutes
- [ ] NTS-1 remains responsive
- [ ] No runaway / blast
- [ ] No persistent mute

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
