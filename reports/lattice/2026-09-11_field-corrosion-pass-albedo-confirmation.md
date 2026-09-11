# FIELD 0.1-2 — Corrosion pairing passes; Albedo coexistence clarified

## Status
2026-09-11; field-0.1-0-test; FIELD 0.1-2 built source e6b84c5426f883a4d05e085725f0551b13fdd9d8. Hardware feedback only; no code/build changes.

## Physical MkI observation
DC confirms Albedo and FIELD do not work together. They sound similar; turning off Albedo also left the LATTICE delay off. DC states: "corrosion and field work perfectly".

Corrosion + FIELD: PASS for the reported playing/listening test. Exact settings, duration, source, CPU margin and long-run stability were not supplied. Albedo + FIELD: unsupported simultaneous user REVERB/DELAY arrangement, consistent with previously verified shared SDK memory regions. Do not classify the absence of FIELD in that combination as a FIELD DSP clipping defect.

## What we learned
FIELD works standalone and with at least one user MOD effect. This refutes a blanket claim that FIELD cannot coexist with any user MOD, but does not prove sufficient processing margin for CORE or every custom MOD. Earlier reports of occasional distortion with built-in effects and problems with our custom effects remain unresolved; exact failing units/settings are needed. Signal headroom and processing load remain hypotheses.

## Next step
Preserve FIELD 0.1-2 and Corrosion + FIELD as listening references. Identify whether CORE 0.3-0 + FIELD is a confirmed failing supported pair before choosing an optimization. No new DSP change without the user's requested review/approval. Long-duration validation remains open.
