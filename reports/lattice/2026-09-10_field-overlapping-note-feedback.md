# FIELD 0.1-0 — clarified hardware feedback and admission check

## Status

2026-09-10, branch `field-0.1-0-test`, FIELD 0.1-0 built at b8e9e3fd14997019cd45423fc54bdbc50b9ee128. Classification: **FAIL / RETEST** for overlapping-note responsiveness; isolated delay audibility now established by user report. No DSP change or new binary.

## Physical report

DC clarified that delay is audible on a single note and on a chord. The first attempt may have been an arpeggio. DC reports hearing nothing when playing a note while delay is still happening. This supersedes a blanket interpretation that FIELD produces no delay. Bloom fullness, current MIX position, whether the dry note or only its new response is missing, and exact source articulation remain unconfirmed.

## Diagnostic evidence

Source admission has a 120 ms refractory interval, an amplitude-envelope attack threshold or release/rearm requirement. It does not intentionally lock admission until a whole phrase expires, and its detector reads FIELD input, not its own echo/bloom output.

A temporary host check of the unchanged DSP accepted two separated 125 ms notes spaced 500 ms apart while the first phrase remained active (2 seeds). A continuous same-amplitude tone changing pitch at 500 ms admitted only the original seed (1 seed). This demonstrates that amplitude-only admission can miss a pitch change without a sufficient new amplitude attack; it does not prove that this is DC's exact hardware cause. The source envelope and MIX behavior must be checked.

## Next step

Confirm at noon MIX whether a newly played note itself remains audible while its additional delay response is missing. This distinguishes dry-path loss from admission behavior. Explain the evidence and agree any proposed admission redesign with DC before implementing or building it. Existing fixed lifetime, no-self-recapture and two-seed limits remain the approved constraints. No new physical runtime/headroom measurement or bloom-quality pass is claimed.
