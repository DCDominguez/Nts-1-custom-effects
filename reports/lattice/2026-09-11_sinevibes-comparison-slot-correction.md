# Sinevibes comparison and MkI effect-slot correction

## Status and physical evidence
2026-09-11; FIELD 0.1-2 source e6b84c5426f883a4d05e085725f0551b13fdd9d8. Report/design correction only; no DSP or binary change.

DC reports our user-made MOD/reverbs overload, built-in effects mostly work with occasional distortion, and Sinevibes Corrosion/Albedo work individually and together. Preserve this as listening evidence; exact selected custom units/settings and whether FIELD remained active in every test are not established. Standalone FIELD remains PASS for reported intended behavior; chain integration remains FAIL / RETEST.

## Uploaded packages inspected
ZIP manifests and payload bytes inspected read-only. Each contains only manifest.json and payload.bin, no C/C++ source, symbols or memory map.

| Upload | Manifest version | Module | API | Payload bytes | SHA256 of upload |
|---|---|---|---|---:|---|
| Albedo+ | 1.5-0 | revfx | 1.1-0 | 4152 | 1e01d3aa807da1f04e1fdd8afcba103753ed4584600842a985268a11da02387a |
| Albedo- | 1.5-0 | revfx | 1.1-0 | 4272 | fe7fc56e262ac3f71a64557de44a5c9e1526f621dacbbc56c137cc72eb78b148 |
| Corrosion | 2.0-0 | modfx | 1.0-0 | 4052 | 133b30a1b21d552aa9ce525f5257d9fc4f0995ec67eded61768e5a12f8b20587 |

Payload hook magic is UREV for Albedo and UMOD for Corrosion. Albedo hook addresses are in the SDK reverb SRAM region; Corrosion in the MOD region. Corrosion's payload API word is 0x00010100 while manifest says 1.0-0; no evidence this benign-looking metadata difference explains any failure. No disassembly, emulation, original source recovery or CPU-cycle measurement performed. Payload/ZIP sizes do not reveal runtime work or uninitialized audio-buffer footprint. Do not compare payload size directly to FIELD ELF text/data/BSS.

## Primary documentation
Downloaded and extracted first-generation `KORG FX User Manual.pdf` from https://www.sinevibes.com/media/korgalbedo/KORG%20FX%20User%20Manual.zip linked by current Sinevibes product pages. Read text for platform notes, Albedo and Corrosion v2; second-generation g2 manual was not used for MkI claims.

First-generation manual documents Albedo+ as 16 forward grains, Albedo- as 10 reverse grains, freeze and feedback, 10 grain-size settings (40..230 ms forward; 80..460 ms reverse). Corrosion v2 documents 10 distortion types, 2x oversampling, self-leveling output gain, and an input gate. This manual is product documentation matching the generation/naming, not proof of every instruction in uploaded binaries. Current website headings Albedo v2 and Corrosion v3 are newer than uploaded manifest versions; don't silently attribute their newer-platform features to these files.

## Critical architecture correction
The first-generation Sinevibes manual explicitly says original NTS-1 permits a user plugin in DELAY or REVERB, not both. It also warns that all oscillators/effects run on a single processor and some combinations may overload.

Verified against local SDK f6f073bb83d5ef69310fca609c41ddcf722dd558 and current official Korg GitHub linker files:
- user DELAY and REVERB both: SRAM 0x20019000 length 12K, SDRAM 0xC0420000 length 2432K.
- user MOD: SRAM 0x20017800 length 6K, SDRAM 0xC0400000 length 128K.

Sources:
https://github.com/korginc/logue-sdk/blob/main/platform/nutekt-digital/dummy-delfx/ld/userdelfx.ld
https://github.com/korginc/logue-sdk/blob/main/platform/nutekt-digital/dummy-revfx/ld/userrevfx.ld
https://github.com/korginc/logue-sdk/blob/main/platform/nutekt-digital/dummy-modfx/ld/usermodfx.ld

Therefore earlier CORE + user ECHO + user SPACE was an unsupported simultaneous user-slot target; do not frame it solely as a CPU-load failure. CORE + FIELD is a valid slot arrangement but still needs runtime/headroom validation. FIELD + built-in reverb is a valid test arrangement. Corrosion + Albedo is user MOD + user REVERB, also valid. FIELD + Albedo is not a supported simultaneous MkI user DELAY+REVERB arrangement; establish active unit selection before interpreting this as a three-unit successful chain. Do not assert how DC's exact firmware switched selections without observing it.

## Comparison with our code and lessons
1. Corrosion v2 explicitly manages output level as part of the effect. Our CORE uses a near-full-scale guard/soft limiter, FIELD uses a wet guard and final clamp; containment does not provide a shared level contract for downstream effects. Investigate inter-stage peak/RMS and limiting, not only final bounded output. Corrosion's success does not by itself prove lower CPU demand.
2. Albedo's documented 16 forward grains show that grain count alone cannot explain efficiency. FIELD combines capture admission/scheduling, up to eight sounding voices and a shared four-line bloom network; CORE independently adds up to 16 voices. These are different workloads, not interchangeable voice-count budgets.
3. CORE 0.3-0 source continues the regular voice loop while full-freeze output crossfades completely to the frozen loop. This is a concrete avoidable-work candidate previously identified, not a measured cause of DC's distortion.
4. FIELD services scheduling, voice scanning, normalization and bloom per sample. Some control/state work may be movable to lower rates, but this requires measured benefit and output-equivalence tests; no optimization implemented.
5. Representative existing NEBULA code recalculates grain lengths/divisions per sample, uses repeated soft saturation, and has previous clipping retest history. ABYSS hard-clamps feedback writes. These justify targeted audits of named failing effects, not a blanket finding that every custom effect shares the same defect.
6. Current successful standalone tests do not validate the composition. Add actual chained-source DSP tests for named failing combinations before another hardware candidate; measure clipping/guards and delivered responses. Desktop chain tests still cannot establish MkI CPU deadlines.

## Proposed next step and approval boundary
First clarify whether Corrosion + FIELD remains clean, and whether selecting Albedo leaves FIELD's distinctive echoes audible. Get the exact failing custom MOD and reverb names. Then audit the supported failing pair internally, separating level sensitivity and unnecessary work. Preserve FIELD 0.1-2 as a reference. No new build or gain/CPU changes without DC's approval.

## Result classification
ASSESSMENT / DESIGN CORRECTION; combined effects FAIL / RETEST. No new physical validation, no binary modification, and no redistribution of uploaded proprietary payloads into the repository.
