# NEBULA Hardware QA

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**  
Current retest build: **NEBULA 0.1-1**

## Record
- Build / commit: ____________________
- Firmware / Librarian: ____________________
- Date / tester: ____________________
- Source / monitoring chain: ____________________

## Baseline
- [ ] Loads without memory error
- [ ] TIME expands apparent cloud size
- [ ] DEPTH increases density/feedback
- [ ] Grain wraps are free of hard clicks
- [ ] Tail is diffuse rather than obvious repeating taps
- [ ] MIX works
- [ ] Stereo field remains balanced
- [ ] Silence eventually decays
- [ ] Rapid controls recover

## 0.1-1 clipping hotfix
The first hardware pass revealed audible clipping. Version 0.1-1 adds explicit wet-path headroom, softer feedback-memory limiting, reduced input injection, and a sub-unity wet output gain.

Test with a full-level sustained synth, bright saw, drums/transients, and a dense stereo source.
- [ ] MIX 0 / 25 / 50 / 75 / 100% has no obvious hard clipping
- [ ] DEPTH 0 / 50 / 100% has no crackly overload on sustained material
- [ ] TIME 0 / 50 / 100% does not expose clipping bursts when grains retarget
- [ ] Hot transient source at DEPTH=100%, MIX=100% remains bounded
- [ ] Dense stereo source at DEPTH=100%, MIX=50% remains bounded
- [ ] Reverb tail remains audible and does not feel excessively attenuated
- [ ] No new pumping, channel imbalance, or sudden tail collapse
- [ ] 30-minute maximum-cloud stability

## Result
- [ ] PASS — clipping hotfix hardware validated
- [ ] PASS WITH NOTES
- [ ] FAIL
- [ ] RETEST REQUIRED

Notes:
