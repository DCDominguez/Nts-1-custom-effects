# DUST reference notes

## Public functional references

Korg's public logue SDK modulation-effect index describes Sinevibes DCM8 as a sample-rate and bit-depth reducer for the NTS-1 class of hardware. Recent September 2026 reporting describes Sinevibes Downgrade as another lo-fi processor built around sample-rate and bit-depth reduction, with parameter smoothing and an input gate.

DUST does not reproduce those implementations. These references establish only the broad effect category.

## Independent DUST design choices

- RATE uses a quadratic sample-hold mapping rather than attempting to match a commercial control law.
- DAMAGE combines wet proportion and quantizer depth into one macro.
- No synthetic dither/noise is added in v0.1; silence therefore does not require a noise gate.
- Strong settings introduce bounded left/right reducer-clock divergence as a Human Soon-specific stereo extension.
- Parameter smoothing is implemented independently in the audio loop.

## Korg API basis

The official original NTS-1 `usermodfx.h` exposes exactly two user-facing modulation-effect parameters:

- `k_user_modfx_param_time`
- `k_user_modfx_param_depth`

The runtime processes stereo frames through `MODFX_PROCESS`, supports up to 64 frames per call, and accepts normalized effect parameters through `MODFX_PARAM` after Q31 conversion.
