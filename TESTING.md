# Hardware Testing Queue

Target hardware: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**.

Compilation is not hardware validation. A unit stays in this queue until it loads on the physical NTS-1 and passes its project QA sheet for the current milestone.

| Project | Unit type | Compile status | Hardware status | QA sheet | Next gate |
|---|---|---|---|---|---|
| SPECTRA | `osc` | PASS | READY FOR TEST | [SPECTRA QA](oscillators/spectra/docs/qa-sheet.md) | Pitch tracking, 1–4 voices, SHAPE, Spread, HarmMode/ALT, Drift, Chaos, aliasing, stability |
| PARALLAX | `delfx` | PASS | READY FOR TEST | [PARALLAX QA](effects/parallax/docs/qa-sheet.md) | Four decorrelated taps, SPREAD, DIVERGENCE, MIX, stereo field, mono compatibility, stability |
| CHORDGHOST | `delfx` | PASS | READY FOR TEST | [CHORDGHOST QA](effects/chordghost/docs/qa-sheet.md) | BPM-synced M1 delay, division changes, MIX, 10-minute stability; then OXI CC chord decode |

## QA rule

Every item marked **READY FOR TEST** must have a project-specific QA sheet before physical testing begins.

Each QA pass should record:

1. Date, tester, NTS-1 firmware, NTS-1 Sound Librarian version and exact build/commit.
2. Test source, controller/MIDI setup and monitoring chain.
3. PASS/FAIL result for every applicable row.
4. Concrete failure settings: note, control value, BPM, source type, stereo/mono state, or reproduction sequence.
5. Blocking vs non-blocking defects and whether a retest is required.
6. Final `Hardware validated for current milestone` status.

## Test rule

1. Load the packaged `.ntkdigunit` with the NTS-1 Sound Librarian.
2. Start at conservative monitoring level.
3. Follow the exact controls, ranges and acceptance criteria in the project's QA sheet and test plan.
4. Record failures by note/range/setting rather than only describing the sound generally.
5. Do not advance a project milestone until the physical NTS-1 passes the current gate.
6. Future milestone rows should remain `N/A` until that implementation exists.

## Current status

All three projects compile successfully against Korg's current `nutekt-digital` SDK templates. None has yet been marked hardware-validated.
