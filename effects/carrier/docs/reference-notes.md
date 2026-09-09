# CARRIER Clean-Room Reference Notes

## Public functional reference

Korg's public logue SDK unit index describes Sinevibes Ring as a **2x oversampling ring modulator** for prologue, minilogue xd, and the original NTS-1.

That description establishes only the broad effect class. CARRIER does not copy or attempt to reconstruct Sinevibes code, coefficients, control curves, UI, presets, or oversampling implementation.

## Independent CARRIER choices

CARRIER differs by design:

- one macro continuously traverses dry -> AM -> ring modulation
- carrier frequency uses a strongly curved slow-to-audio-rate sweep
- strong ring settings add a bounded stereo carrier phase split
- oversampling is deliberately deferred until MkI hardware testing shows it is justified

## Korg API basis

The original NTS-1 ModFX API exposes:

- `MODFX_INIT`
- `MODFX_PROCESS`
- `MODFX_PARAM`
- `TIME` and `DEPTH` user-facing parameter IDs

Parameter values are converted from the host's Q31 representation with `q31_to_f32(value)`. Korg's `fx_sinf()` helper returns `sin(2*pi*x)` from a normalized phase ratio.

## Hardware assumption

The current implementation uses a 48 kHz sample-rate constant to convert carrier frequency in Hz into phase increment. This is an implementation assumption to be checked on the physical original NTS-1; it is not claimed as an owner-manual specification.
