# FIELD — consecutive-note spacing feedback

## Status / hardware observation

Date: 2026-09-10. Feedback follows delivery of FIELD 0.1-1 (build 5179491c69d74be15b3c79aaa06962368bf35387); installed version and knob settings were not restated. Classification: **FAIL / RETEST** for repeatability/phrase identity. No DSP change or rebuild.

DC reports: "i hear just one on consecutive notes. i dont hear another one until i wait like a second or two".

Do not assume this precisely distinguishes one repeat per source note from only the first source note producing a response. The reported audible wait is approximate; it is not measured hardware telemetry.

## Source check

The input trigger's explicit refractory interval is 5760 samples (120 ms at 48 kHz), not 1–2 seconds. Admission additionally requires its amplitude attack or release/rearm criteria. A second seed can be admitted while the first phrase is playing. The code does not intentionally wait for a whole phrase to finish. Missed rearming remains a hypothesis, not a confirmed hardware cause. Clocked response timing and perceptual masking have not been ruled out.

## Next diagnostic

Ask DC to play distinguishable C–D–E notes about half a second apart and identify which pitches are heard in the delayed response. This clarifies what "one" means before any capture redesign. No loudness, capture-policy, gain-protection or other improvement is implemented or approved by this observation. Existing user requirement to pause before new changes remains active.

## Sound / runtime

Intended repeated-note responsiveness is not established. No new conclusion on bloom, clipping, limiter pumping, CPU margin or extended stability is justified. Keep hardware gates open and the PR draft.
