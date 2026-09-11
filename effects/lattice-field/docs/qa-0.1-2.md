# FIELD 0.1-2 — focused MkI handoff

Status: BUILD ONLY until DC tests. Verify 0.1-2 in Librarian; select LatField in DELAY. CORE reference is unchanged 0.3-0.

## First check
Keep the same source and source level as the previous test. MOD and REVERB off. At 120 BPM: TIME about 1 o'clock (1/8), DEPTH fully counterclockwise (Forward), MIX noon (hold DELAY and turn B). Play C-D-E about half a second apart, then pause. Report which pitches echo and whether they are clean. Try MIX at 3 o'clock only after the noon comparison.

## Remaining device checks
- Sweep CLOCK and MODE; confirm physical control response and incoming tempo behavior. Active phrases retain their admitted settings.
- Play continuously at slow CLOCK. Expect some phrases to be selected, not every note; confirm echoes continue while playing.
- Hold a note, then stop incoming audio. New captures during sustain are now intentional. Allow the slowest active phrase plus bloom to finish; at 120 BPM allow 20 seconds, at 30 BPM up to 50 seconds. CORE freeze must stop sending audio for this test.
- Add unchanged CORE; exercise busy material and freeze. Listen for crackles, dropout or collapse. Finish with 30 minutes of representative use if the short checks pass.
- Judge echo presence, bloom body and playability. Desktop results do not answer these taste questions or establish processing margin.

## Known limitations
Soft/quiet input sensitivity, extra capture-duration timing, channel-balance Ping-Pong and pitched-ghost aliasing remain as documented in the internal audit. They are not fixed by this revision.
