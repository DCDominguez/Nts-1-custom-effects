# ASCENDER Spec

ASCENDER is a continuously rising barber-pole phaser for the original NTS-1 MkI.

## Architecture

Three four-stage all-pass lanes are phase-offset by one third-cycle and windowed across their wrap points. Their moving sum is mixed against a permanent dry reference so the all-pass phase motion becomes an audible moving notch field rather than disappearing at 100% wet. A small bounded feedback path reinforces the spectral climb without turning the effect into a resonator.

## Controls

- **TIME** — CLIMB / cycle speed. The useful range is intentionally biased toward perceptible movement during ordinary playing rather than ultra-slow modulation.
- **DEPTH** — HEIGHT / phase-field range and intensity. Higher values widen the all-pass excursion, increase controlled feedback, and move the wet/dry relationship toward the classic approximately half-dry / half-wet phaser region.

## 0.2-0 physical-listening correction

The previous voice could reach effectively 100% wet at maximum DEPTH. On physical MkI testing the effect was technically active but its intended rising motion was not clearly audible. Version 0.2-0 keeps the unshifted reference present, increases the useful climb-rate range, slightly widens stereo phase offset, and adds a bounded feedback contribution.

The A-class harness now includes a perceptual proxy: a sustained tone at high DEPTH must show a materially stronger moving RMS/notch envelope than the low-DEPTH case. This does not replace listening tests; 0.2-0 still requires physical MkI musical validation.

**Target:** original Nu:Tekt NTS-1 digital kit / logue SDK API 1.1-0 / ModFX.
