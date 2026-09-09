# Hardware Testing Queue

Target hardware: **Korg Nu:Tekt NTS-1 digital kit, original / MkI**.

Compilation is not hardware validation. A unit stays in this queue until it loads on the physical NTS-1 and passes its project test plan.

| Project | Unit type | Compile status | Hardware status | Next gate |
|---|---|---|---|---|
| SPECTRA | `osc` | PASS | READY FOR TEST | Pitch tracking, 1–4 voices, SHAPE, Spread, HarmMode/ALT, Drift, Chaos, aliasing, stability |
| PARALLAX | `delfx` | PASS | READY FOR TEST | Four decorrelated taps, SPREAD, DIVERGENCE, MIX, stereo field, stability |
| CHORDGHOST | `delfx` | PASS | READY FOR TEST | BPM-synced M1 delay, division changes, MIX, 10-minute stability; then OXI CC chord decode |

## Test rule

1. Load the packaged `.ntkdigunit` with the NTS-1 Sound Librarian.
2. Start at conservative monitoring level.
3. Test the exact controls and ranges in the project's test plan.
4. Record failures by note/range/setting rather than only describing the sound generally.
5. Do not advance a project milestone until the physical NTS-1 passes the current gate.

## Current status

All three projects compile successfully against Korg's current `nutekt-digital` SDK templates. None has yet been marked hardware-validated.
