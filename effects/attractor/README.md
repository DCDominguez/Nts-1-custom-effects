# ATTRACTOR

ATTRACTOR is a clean-room Human Soon chaotic stereo-motion `modfx` for the original NTS-1 MkI.

| NTS-1 modulation control | ATTRACTOR function |
|---|---|
| TIME | **RATE** — slow to fast chaotic trajectory updates |
| DEPTH | **ORBIT** — center → wide stereo excursion |

Instead of a periodic LFO, two coupled bounded logistic states create a deterministic non-repeating pan trajectory. The pan target is smoothed so the chaos changes direction without audio-rate clicks.

Status: M1 source ready for CI.
