# ATTRACTOR 0.3 Specification

Target: original NTS-1 MkI `modfx`, API `1.1-0`.

## Canonical behavior

A virtual ball with inertia continuously drags the stereo image around:

```text
force -> velocity -> position -> boundary bounce
                    |
              stereo position
                    |
          input image gets dragged
```

- TIME = ENERGY: increases speed, acceleration and force-change rate.
- DEPTH = PULL: increases wet influence and how tightly the stereo image follows
  the ball.
- DEPTH 0 is sample-for-sample dry.
- Bounded, smoothed acceleration impulses supply irregularity; pan targets never
  teleport.
- Elastic wall collisions reflect velocity and keep position continuous.
- Constant-power placement moves both input channels while preserving reduced
  stereo width at high PULL.
- Reset, suspend and resume restore the same fixed physics state and PRNG seed.
- Output is bounded to the NTS-1 floating-point audio range.

## Deferred

Alternate arenas, tempo lock, external sync and orbit presets remain deferred.

