# FIELD 0.1-1 — louder main echoes retest

**Hardware validation OPEN.** This is the approved level-only revision of FIELD 0.1-0. The displayed name remains **LatField**; verify version **0.1-1** in the Librarian. CORE remains the unchanged 0.3-0 reference.

## What changed

The four main echoes receive +6 dB before wet protection. Pitch ghosts, bloom sends, capture behavior, phrase timing and decay are unchanged. This is approximately twice the main-echo amplitude on inputs that do not activate protection; loud inputs may receive much less boost.

## First test with DC

1. Replace FIELD with `field/lattice_field.ntkdigunit` from this package and select LatField. Leave MOD/REVERB off initially. Keep the same source and source level used for 0.1-0.
2. Set host tempo 120 BPM, TIME approximately **1 o'clock**, DEPTH fully counterclockwise (Forward), and MIX **12 o'clock**. Use DELAY + B for MIX.
3. Play one short note, then a chord. Are the first two echoes now prominent enough?
4. Play another note while the earlier phrase continues. Compare its delayed response against the previous version.
5. Try MIX **3 o'clock**. Is the desired delay level available without crunch, pumping or burying the source? Avoid changing source level during the initial comparison.
6. If still faint, tell us the source and whether its output is set high. The existing peak guard can limit the boost on hot input. Do not change protection or ask for a broader architecture change based only on knob position.

## Before accepting

Check Reverse, Ping-Pong and Stutter; rapid notes; max MIX; source-stop decay. Confirm no new clicks, pumping, dropout or stuck tails. Then add unchanged CORE 0.3-0 and perform the full hardware QA including a 30-minute run. The full original procedure is included as `FULL-HARDWARE-QA.md`.

## Desktop evidence — not a hardware listening pass

For synthetic 0.10 and 0.40 FS peak tones, isolated and overlapping inputs produced approximately +6.03 dB first-answer peak increase, zero wet-guard interventions and no clipped samples. At 0.85 FS peak, the guard limited the first-answer increase to about +0.56 dB. Admission counts, response counts and final bloom-network contents matched 0.1-0 exactly in these checks. Actual source envelopes and hardware results may differ.

Reply with: **too quiet / right / too loud**, MIX position, and any crunch or pumping. New improvements beyond this approved adjustment require a pause before implementation.
