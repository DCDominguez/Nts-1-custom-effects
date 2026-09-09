# IRONROT Specification

Target: NTS-1 MkI `modfx`, API `1.1-0`.

## M1
- TIME = CHARACTER.
- DEPTH = CORROSION.
- Character moves from dark/rounded to bright/eroded.
- Corrosion increases drive and wet amount together.
- Rational soft saturation keeps output bounded.
- Slow tone-state filters are reset on suspend/resume.

## Deferred
Alternate clipping families, rectifier modes, oversampling, and dynamic bias are blocked until hardware QA.
