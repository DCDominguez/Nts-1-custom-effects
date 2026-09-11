# FIELD 0.1-0 — new dry note audible, no new delay

## Status and physical result

2026-09-10. FIELD 0.1-0; built commit b8e9e3fd14997019cd45423fc54bdbc50b9ee128. **FAIL / RETEST** for subsequent-note response. No new build or DSP change.

Asked whether the new note itself is audible but its additional repeats are absent at noon MIX, DC replied: "i can hear the note. no delay". Record this as dry-note audibility and missing new delayed response; exact knob values were not restated. Prior isolated note/chord delay audibility remains established. Bloom quality and runtime stability remain unvalidated.

## Interpretation

This narrows the symptom to new wet response rather than complete dry-path loss. Admission is a leading hypothesis based on the prior unchanged-DSP test: continuous equal-level pitch changes do not qualify as amplitude attacks. It is not yet a proven hardware root cause or evidence that wet output blocks the detector.

## Proposed change — pending DC approval

Retain onset capture but allow bounded clock-spaced refresh from qualifying incoming source while playing continues. Preserve two seeds, protected replacement, finite per-seed events/deadline, and no wet-to-detector routing. Require an opportunity for the foreground answer before routine refresh retires it. Exact cadence and admission thresholds would be implementation/tuning decisions after approval.

Tradeoff: a held source can continue generating new phrases until source input subsides, and busy playing can shorten older phrases. This relaxes the previously approved prohibition on periodic sustained-input capture. It does not promise detection of every individual note in audio. A CORE freeze remains ongoing incoming source to FIELD.

## Next step

Pause and obtain DC's approval before implementing this change. Then test continuous equal-level note sequences, new notes during existing tails, held notes, and source-stop decay. CPU/headroom and hardware sound-quality gates remain open. No code has changed in this report.
