# VECTORFILTER Specification

Target: Korg Nu:Tekt NTS-1 digital kit MkI, `modfx`, API `1.1-0`.

## M1
- Stereo state-variable filter.
- TIME = CUTOFF.
- DEPTH = VECTOR morph: LP at 0%, BP at 50%, HP at 100%.
- Resonance/damping becomes strongest around VECTOR center.
- Parameter smoothing on both macros.
- Explicit internal-state clamps to prevent runaway.

## Deferred
- Nonlinear feedback.
- Self-oscillation.
- Alternate filter topologies.
- Tempo modulation.

These remain blocked until MkI QA passes.
