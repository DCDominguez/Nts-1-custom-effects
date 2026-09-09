# VECTORFILTER

VECTORFILTER is a clean-room Human Soon `modfx` for the original Korg Nu:Tekt NTS-1 digital kit MkI.

It is a stereo state-variable filter whose two NTS-1 controls become musical macros rather than exposing internal DSP coefficients.

| NTS-1 modulation control | VECTORFILTER function |
|---|---|
| TIME | **CUTOFF** — low to high cutoff |
| DEPTH | **VECTOR** — low-pass → band-pass → high-pass morph |

Human Soon distinction: resonance is not a third hidden fixed value. It rises toward the center of the VECTOR path, so the band-pass region becomes the most animated and narrow while the low/high extremes remain more controlled.

Status: M1 source ready for CI. Hardware validation required before expansion.
