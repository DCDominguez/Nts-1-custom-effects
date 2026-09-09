# SPECTRA Hardware QA Sheet

Target: **Korg Nu:Tekt NTS-1 digital kit (original / MkI)**

Use this sheet for each physical test pass. Do not mark SPECTRA hardware-validated until all release-gate rows pass.

## Test record

| Field | Value |
|---|---|
| Date |  |
| Tester |  |
| NTS-1 firmware |  |
| NTS-1 Sound Librarian version |  |
| SPECTRA build / commit |  |
| `.ntkdigunit` filename | `spectra.ntkdigunit` |
| MIDI source / controller |  |
| Monitoring chain |  |
| Notes / recording link |  |

## M0 — Load and basic operation

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| S0.1 | Load `spectra.ntkdigunit` with NTS-1 Sound Librarian | Unit transfers and appears as a user oscillator | ☐ PASS ☐ FAIL |  |
| S0.2 | Select SPECTRA on the NTS-1 | Oscillator initializes without lockup or blast | ☐ PASS ☐ FAIL |  |
| S0.3 | Play a basic note at conservative level | Stable audible output | ☐ PASS ☐ FAIL |  |

## M1 — Core oscillator

Recommended host setup from the project test plan: filter mostly open, resonance low, modulation effects minimal, delay/reverb off, simple envelope.

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| S1.1 | Chromatic pitch test C2 → C6 in single-voice mode | Tracks played pitch without stuck notes | ☐ PASS ☐ FAIL |  |
| S1.2 | Fine pitch movement | No obvious stepping | ☐ PASS ☐ FAIL |  |
| S1.3 | `Voices` = 1, 2, 3, 4 | Density increases without a large level jump | ☐ PASS ☐ FAIL |  |
| S1.4 | Sweep `SHAPE` minimum → maximum | Continuous sine → triangle → saw → square morph | ☐ PASS ☐ FAIL |  |
| S1.5 | Listen during SHAPE sweep | No clicks, abrupt gain jumps, obvious DC-like offset | ☐ PASS ☐ FAIL |  |
| S1.6 | `HarmMode=UNISON`, `ALT=0`, `Drift=0`, `Chaos=0`; `Spread=0` | Tuning collapses cleanly | ☐ PASS ☐ FAIL |  |
| S1.7 | Increase `Spread` to maximum | Stable beating; overall tuning remains centered | ☐ PASS ☐ FAIL |  |
| S1.8 | `ALT=100%`; test UNISON/FIFTH/OCTAVE/MAJOR/MINOR/SUS/QUARTAL/CLUSTER | Root remains present and interval character matches mode | ☐ PASS ☐ FAIL |  |
| S1.9 | Sweep `ALT` 0 → 50 → 100% in each harmonic mode | Continuous transition from unison to full constellation | ☐ PASS ☐ FAIL |  |

## M2 — Drift

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| S2.1 | `Spread=0`, `Chaos=0`; increase `Drift` | Voices move independently | ☐ PASS ☐ FAIL |  |
| S2.2 | Sweep `Motion` minimum → maximum | Motion speed changes smoothly | ☐ PASS ☐ FAIL |  |
| S2.3 | Hold sustained notes 1–2 minutes | No obvious shared vibrato dominates; no instability | ☐ PASS ☐ FAIL |  |

## M3 — Chaos

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| S3.1 | `Chaos=0`; retrigger same note repeatedly | Repeats effectively deterministic | ☐ PASS ☐ FAIL |  |
| S3.2 | Chaos 25 / 50 / 75 / 100% | Successive notes vary in phase, fine tuning, internal balance | ☐ PASS ☐ FAIL |  |
| S3.3 | Maximum Chaos | Variation remains bounded and musically related | ☐ PASS ☐ FAIL |  |

## Host LFO / alias / stress

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| S4.1 | NTS-1 LFO target = shape | Host `shape_lfo` moves SPECTRA waveform morph without corrupting pitch | ☐ PASS ☐ FAIL |  |
| S4.2 | High notes with SAW/SQUARE, especially OCTAVE/QUARTAL/CLUSTER | Aliasing acceptable for release | ☐ PASS ☐ FAIL |  |
| S4.3 | Worst-case: Voices 4, Spread 100%, Drift 100%, ALT 100%, Motion 100%, Chaos 100% | No runaway level, blast, lockup, persistent DC | ☐ PASS ☐ FAIL |  |
| S4.4 | Continuous worst-case operation ≥30 min | Stable for entire run | ☐ PASS ☐ FAIL |  |

## Integration — PARALLAX

Run only after SPECTRA and PARALLAX pass independently.

| ID | Check | Expected result | Result | Notes |
|---|---|---|---|---|
| SI.1 | SPECTRA → NTS-1 host voice → PARALLAX | Internal pitches remain intelligible | ☐ PASS ☐ FAIL ☐ N/A |  |
| SI.2 | Collapse result to mono externally | No unacceptable cancellation | ☐ PASS ☐ FAIL ☐ N/A |  |
| SI.3 | Compare SPECTRA `Spread` vs PARALLAX `DIVERGENCE` | Controls remain complementary enough to justify both | ☐ PASS ☐ FAIL ☐ N/A |  |

## QA result

- Overall: ☐ PASS ☐ FAIL ☐ BLOCKED
- Blocking defects:
  - 
- Non-blocking defects / tuning notes:
  - 
- Retest required: ☐ YES ☐ NO
- Hardware validated: ☐ YES ☐ NO
