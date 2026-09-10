# FIELD — C-D-E test: only C echoed

## Status / physical observation

2026-09-10. Current test candidate FIELD 0.1-1, built commit 5179491c69d74be15b3c79aaa06962368bf35387. Hardware classification: **FAIL / RETEST** for consecutive-note response. Installed version and exact timing/settings were not restated.

Asked to play C–D–E about half a second apart and identify the delayed pitches, DC replied: "just c". Only C's delayed response is reported; D and E did not produce audible delayed responses in that test. This resolves the ambiguity in the preceding report. It does not independently measure whether the missing response arises at admission, capture or playback.

## Interpretation

Amplitude-based admission/rearming remains the leading hypothesis, supported by the earlier desktop demonstration that an equal-level pitch change admits no new seed. Do not claim hardware root cause is proven. Increasing echo gain has not established consecutive-note responsiveness.

## Proposed next revision — approval required

Revisit bounded, clock-spaced source recapture alongside attack-triggered admission, so continued playing can supply fresh fragments without first crossing a strong amplitude-attack threshold. Preserve two-seed/voice ceilings, protected replacement, finite per-seed lifetimes and no wet self-recapture. Ensure scheduled foreground responses have an opportunity to play before routine refresh retires their seed. Keep the approved 0.1-1 level tuning.

Tradeoff: a held incoming note may keep generating new phrases until source activity subsides; busy playing may shorten older phrases. This explicitly relaxes the original no-periodic-sustained-capture rule and does not promise exact detection of every note in audio. Upstream CORE freeze remains qualifying source activity.

## Next step

Ask DC to approve a FIELD 0.1-2 capture revision before changing code. Validate C-D-E continuity, equal-level arpeggios, separated notes, held-source behavior, two-seed retirement and return to silence after source stops, then physical retest. No DSP change or build was made for this report. All hardware runtime, bloom-quality and extended stability gates remain open.
