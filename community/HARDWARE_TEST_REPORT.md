# Community Hardware Test Report — Korg logue SDK Platforms

Copy this template into an issue/PR/report and fill only what you actually observed.

## Candidate

- Human Soon unit:
- Version:
- Commit SHA / artifact source:
- Target product: `NTS-1 MkI` / `minilogue xd` / `prologue` / `NTS-1 MkII` / `NTS-3` / `microKORG2` / `drumlogue` / other:
- Unit type on that product: `osc` / `modfx` / `delfx` / `revfx` / `genericfx` / `synth` / `masterfx`:
- Product firmware (if known):
- SDK/build generation if known: `1.1` / `2.0` / `2.1`:

## Test environment

- Input/source:
- Monitoring/output path:
- MIDI clock source / BPM if relevant:
- Other active processors / slots:
- Built-in vs custom for each active processor:
- If NTS-3: runtime/slot arrangement and XY/touch use:
- If prologue: single/dual timbre and which timbre(s) feed the effect:

## Load/select

- Loads into product librarian/editor/device: PASS / FAIL
- Selects on device: PASS / FAIL
- Produces audio: PASS / FAIL
- Notes:

## Control/runtime sanity

Record exact values where the product exposes them, otherwise approximate knob/XY positions.

- Control A / TIME / primary parameter:
- Control B / DEPTH / secondary parameter:
- MIX / shifted parameter where applicable:
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

Describe only what happened: silence, crunch, clipping, stuck tail, missed echo, crash, unexpected pitch, touch mapping failure, one timbre missing, etc.

### Threshold / control position

Where does it begin? Example: "TIME around 3 o'clock and above."

### Does lowering input level change it?

YES / NO / NOT TESTED

### Does disabling another processor/runtime change it?

YES / NO / NOT TESTED

If yes, state exactly which processor/runtime.

### Does the same build/algorithm behave differently on another logue platform?

YES / NO / NOT TESTED

If yes, identify the other product and exact build.

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

A hardware pass on one logue SDK product does **not** automatically validate another product, even when Korg documents binary compatibility between members of the first-generation family.
