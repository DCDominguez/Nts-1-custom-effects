# Community Hardware Test Report — Original NTS-1 MkI

Copy this template into an issue/PR/report and fill only what you actually observed.

## Candidate

- Unit:
- Version:
- Commit SHA / artifact source:
- Unit type: `osc` / `modfx` / `delfx` / `revfx`
- Original NTS-1 MkI firmware (if known):

## Test environment

- Input/source:
- Monitoring/output path:
- MIDI clock source / BPM if relevant:
- Other active processors:
  - MOD:
  - DELAY:
  - REVERB:
- Built-in vs custom for each active processor:

## Load/select

- Loads into Librarian / device: PASS / FAIL
- Selects on device: PASS / FAIL
- Produces audio: PASS / FAIL
- Notes:

## Control/runtime sanity

Record the exact control or approximate clock-face position.

- TIME / A:
- DEPTH / B:
- MIX or SHIFT+B where applicable:
- Other project controls:
- Worst-case sweep performed:
- Runtime result: PASS / FAIL / PASS WITH NOTES

## Tone and musicality

- Identity is obvious: YES / NO / PARTLY
- Useful musical range:
- Weak/dead zones:
- Harsh/clipped/crunchy regions:
- Stereo impression:
- Would you use it in a track/performance? YES / NO / MAYBE
- Tone/musicality result: PASS / PASS WITH NOTES / FAIL

## Reproduction steps for any problem

1.
2.
3.
4.

### Exact observed symptom

Describe only what happened: silence, crunch, clipping, stuck tail, missed echo, crash, unexpected pitch, etc.

### Threshold / control position

Where does it begin? Example: "TIME around 3 o'clock and above."

### Does lowering input level change it?

YES / NO / NOT TESTED

### Does disabling another processor change it?

YES / NO / NOT TESTED

If yes, state exactly which processor.

## Result classification

Choose all applicable:

- [ ] LOAD PASS
- [ ] LOAD FAIL
- [ ] RUNTIME PASS
- [ ] RUNTIME FAIL
- [ ] MUSICAL PASS
- [ ] PASS WITH NOTES
- [ ] FAIL / RETEST

## Additional notes

Do not infer CPU percentage, memory exhaustion or SDK behavior from sound alone unless separately measured. It is fine to suggest a hypothesis, but label it **inference** rather than observation.
