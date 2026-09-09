# CHORDGHOST Hardware QA Sheet

Target: **Korg Nu:Tekt NTS-1 digital kit (original / MkI)**

Use this sheet for each physical test pass. The current compiled build is M1 plus the chord-state decoder scaffold; do not mark later milestones until their DSP exists.

## Test record

| Field | Value |
|---|---|
| Date |  |
| Tester |  |
| NTS-1 firmware |  |
| NTS-1 Sound Librarian version |  |
| CHORDGHOST build / commit |  |
| `.ntkdigunit` filename | `chordghost.ntkdigunit` |
| MIDI source / OXI setup |  |
| Test source |  |
| Monitoring chain |  |
| Notes / recording link |  |

## M0 — Load and basic operation

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| C0.1 | Load `chordghost.ntkdigunit` with NTS-1 Sound Librarian | Unit transfers and appears as a user delay effect | ☐ PASS ☐ FAIL |  |
| C0.2 | Select CHORDGHOST | Effect initializes without lockup or blast | ☐ PASS ☐ FAIL |  |
| C0.3 | Pass a short percussive monophonic synth signal | Stable dry + delayed output | ☐ PASS ☐ FAIL |  |

## M1 — Plain BPM-synced delay

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| C1.1 | `MIX` minimum | Dry signal remains audible | ☐ PASS ☐ FAIL |  |
| C1.2 | Increase `MIX` | Wet signal appears predictably | ☐ PASS ☐ FAIL |  |
| C1.3 | Let repeats decay | Feedback does not run away | ☐ PASS ☐ FAIL |  |
| C1.4 | Sweep `TIME` through all six divisions | All six delay divisions selectable | ☐ PASS ☐ FAIL |  |
| C1.5 | Change `TIME` while repeats are active | No hard clicks or unstable jumps | ☐ PASS ☐ FAIL |  |
| C1.6 | Change NTS-1 BPM | Delay follows host BPM | ☐ PASS ☐ FAIL |  |
| C1.7 | Stereo input | No unexpected channel swap or collapse | ☐ PASS ☐ FAIL |  |
| C1.8 | Continuous operation ≥10 min | Stable throughout | ☐ PASS ☐ FAIL |  |

## M1 boundary checks

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| CB.1 | Lowest practical BPM used in setup | Stable, bounded delay time | ☐ PASS ☐ FAIL |  |
| CB.2 | Highest practical BPM used in setup | Stable, bounded delay time | ☐ PASS ☐ FAIL |  |
| CB.3 | `TIME` minimum | Correct shortest division | ☐ PASS ☐ FAIL |  |
| CB.4 | `TIME` maximum | Correct longest division | ☐ PASS ☐ FAIL |  |
| CB.5 | `MIX` 0 / 50 / 100% | Correct dry/wet behavior | ☐ PASS ☐ FAIL |  |

## M2 — OXI chord protocol

Use the OXI mapping already defined for the project: **CC31 / DELAY DEPTH** carries the 7-bit chord-state code.

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| C2.1 | Send CC31 values 0…127 | Decoder is monotonic; no skipped or duplicated critical states | ☐ PASS ☐ FAIL |  |
| C2.2 | Inspect boundaries 11/12, 23/24, 35/36, 47/48, 59/60 | Correct quality transition at each boundary | ☐ PASS ☐ FAIL |  |
| C2.3 | Inspect boundaries 71/72, 83/84, 95/96, 107/108, 119/120 | Correct quality/special transition | ☐ PASS ☐ FAIL |  |
| C2.4 | Inspect 120/121 and 121/122 | Bypass / hold / reserved behavior decodes as specified | ☐ PASS ☐ FAIL |  |
| C2.5 | Sequence Cmaj7 / Am7 / Fmaj7 / G7 = 24 / 45 / 29 / 55 | State changes align exactly with progression without interrupting delay audio | ☐ PASS ☐ FAIL |  |

## Future M3–M6 placeholders

Do not mark these until the corresponding implementation exists.

| ID | Check | Result | Notes |
|---|---|---|---|
| C3.1 | Fixed interval shifter 0, ±3, ±4, ±7, ±12 semitones | ☐ PASS ☐ FAIL ☐ N/A |  |
| C4.1 | Monophonic pitch detector accuracy / stability | ☐ PASS ☐ FAIL ☐ N/A |  |
| C5.1 | Voice-leading targets always belong to active chord | ☐ PASS ☐ FAIL ☐ N/A |  |
| C5.2 | Existing feedback tail moves into new chord after harmonic change | ☐ PASS ☐ FAIL ☐ N/A |  |
| C6.1 | Release stress test ≥30 min | ☐ PASS ☐ FAIL ☐ N/A |  |

## QA result

- Overall: ☐ PASS ☐ FAIL ☐ BLOCKED
- Blocking defects:
  - 
- Non-blocking defects / tuning notes:
  - 
- Retest required: ☐ YES ☐ NO
- Hardware validated for current milestone: ☐ YES ☐ NO
