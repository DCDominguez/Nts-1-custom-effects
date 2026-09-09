# LATTICE ECHO 0.1-0 — MkI Hardware QA

## Standalone
- [ ] Loads/selects normally
- [ ] DISTANCE spans clearly short to clearly long echo spacing
- [ ] PICK changes how often events are promoted into echo
- [ ] MIX 0 / 50 / 100 behaves safely
- [ ] Feedback decays and never runs away

## With LATTICE CORE
- [ ] ECHO reacts to the windowed microloop events rather than turning the whole stream into a conventional always-on delay
- [ ] Sparse PICK settings visibly promote only occasional fragments
- [ ] Dense PICK settings still leave recognizable event boundaries
- [ ] The pitch of promoted LATTICE fragments is preserved in the echo trail
- [ ] Stereo movement remains audible

## Stress
- [ ] Plucks, drums, sustained synth and noisy input remain stable
- [ ] Rapid DISTANCE movement recovers safely
- [ ] 30-minute feedback-heavy run stable

Result: [ ] PASS  [ ] PASS WITH NOTES  [ ] FAIL / RETEST
