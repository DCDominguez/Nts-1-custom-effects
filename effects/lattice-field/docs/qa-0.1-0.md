# FIELD 0.1-0 — DC's original NTS-1 MkI hardware test

Status: BUILD ONLY after successful ARM CI; hardware gates remain OPEN.
Load `lattice_field.ntkdigunit` into a user DELAY slot with the NTS-1 Librarian. Select **LatField**. The accompanying CORE binary is the unchanged **0.3-0** reference, not a freeze-fix update. Start with MOD and REVERB off.

## First five minutes

1. Feed a short, clearly articulated note from your usual source. Use the same source level between comparisons. Set host tempo to **120 BPM**, DELAY TIME approximately **1 o'clock**, DEPTH fully counterclockwise (Forward), and MIX **12 o'clock**. Knob clock positions are approximate physical guides; TIME has eight stepped zones.
2. Play one note, then stop the incoming audio. You should hear clear forward answers followed by quieter pitch ghosts and bloom. At the middle divisions, wait **12 seconds** before the next isolated test. Is the first answer obvious? Is the second still clear? Does the bloom feel full rather than thin?
3. Move DEPTH into the next quarter of its travel and play a NEW note: Reverse. Then third quarter: Ping-Pong. Final quarter: Stutter. Existing phrases retain their old mode. Judge backward articulation, alternating stereo, and a short rhythmic burst respectively.
4. Hold a constant note for 12 seconds. It should not gain another phrase merely because 0.58 seconds elapsed. Release, leave a gap, then play again: a new phrase should start. An actual new attack on top of a held sound may also start a phrase.
5. Try MIX at **9, 12 and 3 o'clock**, then maximum. Is the source clear at lower/middle MIX? Does max MIX produce an obvious finite effect without crunch or pumping?

## Control and density checks

- Play repeated short notes, including pairs and rapid third notes. Replacement should fade, never click or turn an old sample into a different note halfway through playback.
- Sweep TIME through all eight zones and DEPTH through all four while playing. New phrases adopt the changes; old phrases do not jump speed, freeze or restart.
- Sweep MIX continuously from dry to max and back. Listen for zipper noise, jumps or disappearing sound.
- Try a low bass note, a bright transient, a pad and a stereo source. Compare headphones and summed mono. Are fifth/octave ghosts musical, or brittle/aliased? A failed pitch-quality check stays open; do not call it intended lo-fi character.
- At max MIX, stop incoming audio and wait for rest. At **30 BPM and the slowest 1/2 setting**, allow **49 seconds** for the conservative maximum deadline plus the full bloom-rest window. At 120 BPM, slowest division, allow **19 seconds**. Distinct scheduled responses should finish earlier than this upper bound.
- Turn the effect off/on. It should restart cleanly and respect the panel's current parameters.

## CORE pairing — only after FIELD alone passes

1. Enable CORE **0.3-0** in MOD. Start CORE TIME low, then raise through approximately **9, 12 and 3 o'clock**. Test varied PATTERN settings.
2. Try CORE max-TIME freeze. FIELD may accept new attacks from that repeating incoming loop. To test FIELD expiry, stop CORE's incoming contribution too; stopping your fingers alone does not stop a frozen loop.
3. Sweep FIELD controls during CORE activity. Note any threshold where sound crunches, drops out, becomes stuck or fails to recover.
4. Run **CORE + FIELD for at least 30 minutes**, varying source and controls. Only then try optional **Korg built-in** reverb.

## Send back this short report

- FIELD loads / displayed name:
- Source and input level setting:
- First and second answers at noon MIX: too quiet / right / too loud
- Modes: Forward / Reverse / Ping-Pong / Stutter distinct?
- Bloom: too thin / right / too much; BODY and HALO impression:
- After input stops: repeats finish? bloom reaches silence?
- Clicks, crunch, pumping or dropouts; exact control/source setting:
- CORE + FIELD: stable? freeze behavior? elapsed test duration:

Do not infer CPU usage from loudness or distortion. CI and desktop checks establish neither MkI runtime margin nor sound quality. Record only what was heard and tested.
