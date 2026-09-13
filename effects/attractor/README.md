# ATTRACTOR 0.3

ATTRACTOR is a clean-room VSDC stereo-motion `modfx` for the original NTS-1 MkI.

It models a virtual ball with position, velocity and bounded acceleration. The
ball drags the full stereo image as it moves and reflects continuously from the
edges of the field.

| NTS-1 modulation control | ATTRACTOR function |
|---|---|
| TIME | **ENERGY** — slow drifting bounces to faster, more aggressive motion |
| DEPTH | **PULL** — dry stereo at zero to wide image-dragging motion at maximum |

The force sequence is deterministic from reset. PULL uses constant-power source
placement and narrows, but does not collapse, the original stereo width as it
rises. At 50% the travel is intentionally obvious; at 100% both original stereo
channels follow the ball without hard left/right switching.

Status: 0.3 source and host-side behavioral tests ready for hardware QA.

