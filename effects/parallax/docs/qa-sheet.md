# PARALLAX Hardware QA Sheet

Target: **Korg Nu:Tekt NTS-1 digital kit (original / MkI)**

Use this sheet for each physical test pass. PARALLAX remains unvalidated until the current milestone and release stress rows pass.

## Test record

| Field | Value |
|---|---|
| Date |  |
| Tester |  |
| NTS-1 firmware |  |
| NTS-1 Sound Librarian version |  |
| PARALLAX build / commit |  |
| `.ntkdigunit` filename | `parallax.ntkdigunit` |
| Test source |  |
| Monitoring / stereo chain |  |
| Notes / recording link |  |

## M0 — Load and basic operation

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| P0.1 | Load `parallax.ntkdigunit` with NTS-1 Sound Librarian | Unit transfers and appears as a user delay effect | ☐ PASS ☐ FAIL |  |
| P0.2 | Select PARALLAX | Effect initializes without lockup or blast | ☐ PASS ☐ FAIL |  |
| P0.3 | Pass audio at conservative level | Dry path remains valid and effect is audible | ☐ PASS ☐ FAIL |  |

## M1 — Four-voice spatial field

Test with: sustained sine/triangle, sustained saw, short pluck, and full stereo material through audio input.

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| P1.1 | High `MIX` | All four taps are audible | ☐ PASS ☐ FAIL |  |
| P1.2 | Listen to tap timing | Clearly different arrival times | ☐ PASS ☐ FAIL |  |
| P1.3 | Listen to modulation | No two voices move in obvious lockstep | ☐ PASS ☐ FAIL |  |
| P1.4 | Stereo monitoring | Voices distributed left-to-right | ☐ PASS ☐ FAIL |  |
| P1.5 | Sweep `TIME` / `SPREAD` | Timing field expands/contracts smoothly | ☐ PASS ☐ FAIL |  |
| P1.6 | Sweep `DEPTH` / `DIVERGENCE` | Motion increases without discontinuities | ☐ PASS ☐ FAIL |  |
| P1.7 | Sweep `MIX` dry → wet | Predictable dry/wet transition | ☐ PASS ☐ FAIL |  |
| P1.8 | Sustained tones | No clicks at modulation extrema | ☐ PASS ☐ FAIL |  |
| P1.9 | Stereo source orientation | Input stereo orientation preserved | ☐ PASS ☐ FAIL |  |

## Mono compatibility

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| PM.1 | Collapse output to mono externally | No severe recurring cancellation | ☐ PASS ☐ FAIL |  |
| PM.2 | Sweep `SPREAD` while mono | No range disappears entirely | ☐ PASS ☐ FAIL |  |
| PM.3 | Sustained input while mono | No unacceptable rhythmically repeating level pump | ☐ PASS ☐ FAIL |  |

## Release stress gate for current M1 build

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| PS.1 | `SPREAD` minimum / maximum | Stable at both extremes | ☐ PASS ☐ FAIL |  |
| PS.2 | `DIVERGENCE` minimum / maximum | Stable at both extremes | ☐ PASS ☐ FAIL |  |
| PS.3 | `MIX` 0 / 50 / 100% | Correct dry/wet behavior | ☐ PASS ☐ FAIL |  |
| PS.4 | Rapid control movement | No crashes, blasts or persistent corruption | ☐ PASS ☐ FAIL |  |
| PS.5 | Silence → sharp transients | No runaway level or click bursts beyond normal processing | ☐ PASS ☐ FAIL |  |
| PS.6 | Hot input material | Output remains bounded | ☐ PASS ☐ FAIL |  |
| PS.7 | Continuous playback ≥30 min | Stable for entire run | ☐ PASS ☐ FAIL |  |
| PS.8 | Effect switching / suspend-resume if available in setup | Returns cleanly | ☐ PASS ☐ FAIL ☐ N/A |  |

## Future M2+ pitch-engine placeholders

Do not mark these until the corresponding code milestone exists.

| ID | Check | Result | Notes |
|---|---|---|---|
| P2.1 | One true pitch-shift voice: -100, -50, -12, -7, +7, +12, +50, +100 cents | ☐ PASS ☐ FAIL ☐ N/A |  |
| P2.2 | Semitone offsets: -12, -7, +7, +12 | ☐ PASS ☐ FAIL ☐ N/A |  |
| P3.1 | 1 → 2 → 4 shifted voices CPU/stability | ☐ PASS ☐ FAIL ☐ N/A |  |
| P4.1 | Default constellation tuning | ☐ PASS ☐ FAIL ☐ N/A |  |
| P5.1 | Spatial drift around anchors | ☐ PASS ☐ FAIL ☐ N/A |  |

## QA result

- Overall: ☐ PASS ☐ FAIL ☐ BLOCKED
- Blocking defects:
  - 
- Non-blocking defects / tuning notes:
  - 
- Retest required: ☐ YES ☐ NO
- Hardware validated for current milestone: ☐ YES ☐ NO
