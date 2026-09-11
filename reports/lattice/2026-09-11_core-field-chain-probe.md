# CORE + FIELD distortion narrowed; initial actual-DSP chain probe

## Status / physical observation
2026-09-11; CORE 0.3-0 + FIELD 0.1-2. DC confirms CORE + FIELD had the most distortion; the tested built-in MOD effects had no issues. This supersedes the earlier broad interpretation of built-in MOD distortion, but does not resolve earlier built-in REVERB observations. Exact settings, input, firmware and duration unspecified. Corrosion + FIELD remains reported clean.

Classification: CORE + FIELD FAIL / RETEST; built-in MOD + FIELD PASS for reported tests, not long-duration validation. No production source or device build changes.

## Internal diagnostic
Added tests/lattice-chain/probe.cpp and minimal usermodfx.h host shim. Includes actual CORE and FIELD production source in distinct namespaces, feeds CORE stereo output directly into FIELD. Uses 48 kHz, 120 BPM, FIELD CLOCK .56, mode .1, MIX .5, CORE DEPTH .5. Eight-second continuous C-D-E-like sine pitch cycling; input peaks .2/.4/.85; CORE TIME 0/.5/.9/1. TIME 1 case runs .9 for two seconds before engaging freeze. Measures interstage/output peaks, minimum guard gains, guard attacks, clipped/nonfinite samples and new CORE events during settled freeze. SDK hardware callbacks are shimmed; no ARM cycle measurements.

Reproduce: g++ -std=c++11 -O2 -Wall -Wextra -Werror -I tests/lattice-field/stubs tests/lattice-chain/probe.cpp -o /tmp/chain-probe && /tmp/chain-probe

At .4 input peak:
| CORE TIME | CORE output peak | Chain output peak | CORE guard min | FIELD wet guard min | FIELD guard attacks |
|---|---:|---:|---:|---:|---:|
| 0 | .400000 | .521841 | 1 | 1 | 0 |
| .5 | .833099 | .638197 | 1 | .667487 | 1041 |
| .9 | .860000 | .631130 | .916707 | .528542 | 1243 |
| freeze after 2 seconds | .860000 | .618640 | .995819 | .528542 | 220 |

A .528542 gain is approximately -5.54 dB. At .2 input none of the tested cases invokes either guard. At .85 input multiple cases invoke both protections. All 12 cases had zero samples at or above .99999 and zero nonfinite output. Peaks below clipping are not proof of distortion-free gain modulation. This diagnoses a reproducible level interaction, not DC's exact audible failure or CPU cause.

With freeze engaged after history fills, CORE schedules 82 additional ordinary voice events during settled freeze in each tested amplitude case. This confirms continuing ordinary work after its output contribution becomes negligible; earlier source review identified the same avoidable-work candidate. It does not quantify cycles saved or explain non-freeze distortion.

## Interpretation and next step
Do not reduce FIELD quality merely because CORE pairing fails. Preserve standalone FIELD and Corrosion pairing references. Establish whether CORE alone at the same settings is clean; separate CORE's own sound/limiting from added FIELD interaction. Candidate work is a CORE level/headroom review and freeze-work optimization, each subject to DC's pause-before-additional-improvements rule. No gain or CPU optimization implemented. Hardware runtime margin remains unknown.

