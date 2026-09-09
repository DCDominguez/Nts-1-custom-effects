# SPECTRA M2 Hardware QA — Independent Drift

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**  
Build: **SPECTRA 0.2-0**

## Record
- Date / tester: ____________________
- Firmware: ____________________
- Sound Librarian: ____________________
- Commit / binary: ____________________
- MIDI source: ____________________

## Baseline regression
- [ ] Loads and selects without blast/lockup
- [ ] Single-voice pitch still tracks C2–C6
- [ ] `SHAPE`, `ALT`, `Voices`, `Spread`, and `HarmMode` still behave as in M1
- [ ] `Drift=0` produces no audible drift contribution

## M2 — Independent Drift
Use `HarmMode=UNISON`, `ALT=0`, `Spread=0`, `Chaos=0`, `Voices=4`.

- [ ] `Drift` 0 → 25 → 50 → 75 → 100% increases pitch wander smoothly
- [ ] `Motion` 0 → 25 → 50 → 75 → 100% changes drift speed smoothly
- [ ] Sustained sine/triangle for 1–2 minutes: voices do not move in obvious lockstep
- [ ] No single shared-vibrato sensation dominates at medium settings
- [ ] High Drift + low Motion remains slow rather than becoming a static offset
- [ ] High Motion + low Drift changes speed without excessive depth
- [ ] Ordinary knob movement produces no clicks or large pitch steps

## Host interaction
- [ ] NTS-1 LFO target = shape still animates waveform morph without corrupting pitch/drift
- [ ] Drift remains independent while host `shape_lfo` is active

## Stress
Set `Voices=4`, `Drift=100%`, `Motion=100%`, `Spread=100%`, `ALT=100%`.
- [ ] 10 minutes stable
- [ ] 30 minutes stable
- [ ] No runaway, stuck voice, DC-like output, or NTS-1 lockup

## Result
- [ ] PASS — M2 hardware validated
- [ ] PASS WITH NOTES
- [ ] FAIL / RETEST

Notes:
