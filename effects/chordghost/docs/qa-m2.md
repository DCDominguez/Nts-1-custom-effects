# CHORDGHOST M2 Hardware QA — OXI Chord Protocol

Target: **Korg Nu:Tekt NTS-1 digital kit MkI**  
Build: **CHORDGHOST 0.2-0**

M2 validates the control path only. Repeats are still harmonically neutral until M3, so chord-state changes should **not** yet retune audio.

## Record
- Date / tester: ____________________
- Firmware / Librarian: ____________________
- OXI One MKII firmware: ____________________
- Commit / binary: ____________________
- MIDI routing: ____________________

## Baseline regression
- [ ] Loads/selects safely
- [ ] BPM-synced delay still works at all six divisions
- [ ] `SHIFT+DEPTH/MIX` still works
- [ ] Delay tail is stable at normal feedback

## M2 — OXI / CC31 control path
Send OXI CC31 directly to CHORDGHOST.
- [ ] Sweep CC31 0 → 127: no clicks, blasts, mute, or tail interruption
- [ ] Send boundary pairs 11/12, 23/24, 35/36, 47/48, 59/60, 71/72, 83/84, 95/96, 107/108, 119/120
- [ ] Send 120 (harmonic bypass code): delay continues normally
- [ ] Send 121 (HOLD): delay continues normally
- [ ] Send 122–127 reserved values: delay remains stable and previous valid state is preserved internally
- [ ] Sequence `Cmaj7=24 | Am7=45 | Fmaj7=29 | G7=55` for at least 32 bars with no audio interruption
- [ ] Rapid chord-code changes at 1/16-note rate do not destabilize the delay

Exact chord-mask decoding is also checked in CI because M2 has no audible reharmonization stage yet.

## Suspend / resume
- [ ] Switch away from CHORDGHOST and back: no stale delay blast
- [ ] Last valid chord control state can be resent immediately without fault

## Stress
- [ ] BPM changes while CC31 changes are occurring
- [ ] TIME and MIX changes while CC31 changes are occurring
- [ ] 30-minute continuous OXI-driven run stable

## Result
- [ ] PASS — M2 hardware validated
- [ ] PASS WITH NOTES
- [ ] FAIL / RETEST

Notes:
