# FIELD 0.1-0 — delay prominence below requested level

## Status / physical observation

2026-09-10. FIELD 0.1-0, built commit b8e9e3fd14997019cd45423fc54bdbc50b9ee128. Classification: **FAIL / RETEST** for desired delay prominence. No DSP change or rebuild.

Following the suggested MIX 3 o'clock check, DC reported: "it's audible but not as loud as id like it to be." Exact physical knob values were not restated. Delay audibility remains established; desired prominence is not met. No new missing-trigger, bloom-quality or runtime conclusion follows.

## Proposed adjustment — awaiting approval

A bounded FIELD 0.1-1 level-only comparison: target +6 dB (approximately 2x amplitude) on the four clear foreground responses before wet protection, retaining current capture/admission, phrase timing and lifetime, ghost levels and bloom-send levels. Existing protection can reduce the actual increase on loud or overlapping material; +6 dB is not a guaranteed post-guard level. Do not boost the whole wet bus or change the capture policy to solve this observation.

Validate level differences and protection activity with representative isolated and overlapping input before ARM build, then have DC compare at the same source level and MIX positions. If the proposed boost requires another architecture or protection change, pause and discuss that instead of silently expanding scope.

## Sound-quality / runtime intent

Make the clear delay answer more prominent while retaining foreground/background separation. Keep finite behavior and resource ceilings. No hardware CPU/headroom margin has been measured. Loud-source clipping or audible wet-guard pumping would fail the retest.

## Next step

Ask DC to approve this focused level adjustment before modifying DSP or building FIELD 0.1-1. The proposed periodic recapture redesign remains on hold.
