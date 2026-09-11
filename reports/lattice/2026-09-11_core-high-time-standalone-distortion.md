# CORE standalone high-TIME distortion

## Status / hardware observation
2026-09-11; CORE reference 0.3-0; no new build. DC reports CORE alone runs clean until TIME is around 3 o'clock or higher, where distortion appears. Exact parameter number, input level, DEPTH, oscillator/source and duration not measured. Do not convert clock-face position to an exact voice count.

Classification: CORE standalone FAIL / RETEST at high TIME. CORE + FIELD remains FAIL / RETEST. FIELD 0.1-2 standalone, Corrosion + FIELD, and tested built-in MOD + FIELD retain their reported passes.

## Source-grounded interpretation
Current CORE source maps TIME to up to 16 voices, increased modulation amount (affecting pitch/rhythm choices), and wet_drive from .90 toward 1.18. Freeze engages only when target TIME exceeds .985; the reported 3 o'clock onset cannot be attributed to full-freeze extra work without further evidence. More voices increase workload and alter summed signal levels simultaneously, so the onset does not distinguish runtime overload, gain-control artifacts or granular playback artifacts.

Earlier actual-source chain probe found CORE peak .833099 from .4 sine input at TIME .5 and .86 at TIME .9 with its guard active. At .2 input the tested cases did not activate guards. Those controlled synthetic results motivate level-sensitivity isolation; no claim of reproducing the exact audible hardware distortion.

## What changed / next step
Hardware report only. Preserve FIELD. Ask whether substantially reducing the incoming source level clears CORE-alone distortion at the same high TIME and DEPTH. A clear level dependence supports a signal/gain investigation; persistent distortion would leave runtime and playback artifacts open, not prove CPU exhaustion. Any CORE DSP revision remains subject to the user's pause-before-additional-improvements instruction. Existing full-freeze optimization candidate does not by itself address pre-freeze high-TIME distortion.
