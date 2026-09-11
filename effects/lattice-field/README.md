# LATTICE FIELD 0.1-2

Original NTS-1 MkI custom Delay; displayed as **LatField**. Hardware validation is open.

Two incoming stereo fragments become finite, clocked answers, then fifth/octave ghosts and a shared spatial bloom. The first four responses are unison; the final two ghosts are +7 and +12 semitones. Forward, Reverse, Ping-Pong and Stutter use distinct phrase tables. CORE remains the composer; FIELD supplies responses and atmosphere.

| Control | Function |
|---|---|
| DELAY A / TIME | CLOCK: 1/32, 1/16T, 1/16, 1/8T, 1/8, 1/4T, 1/4, 1/2 |
| DELAY B / DEPTH | Four equal zones: Forward, Reverse, Ping-Pong, Stutter |
| DELAY + B / MIX | Dry to fully wet; smoothed |

CLOCK, host tempo and MODE latch when the next seed is admitted. An existing phrase completes its original grammar. No phrase changes speed or extends its deadline when a knob moves. MIX responds continuously.

At noon MIX, the design gains are dry 0.5 and main-answer approximately 0.798 before stereo balance, overlap management and protection. FIELD 0.1-1 adds +6 dB only to the four main echoes, after bloom sends. Ghost and bloom-send levels are unchanged; the shared wet guard can attenuate all wet components when peaks demand it. These gains are not a claim about perceived loudness. Use hardware listening to judge first-answer prominence and bloom.

FIELD 0.1-2 admits fresh attacks and also samples qualifying continuous input at CLOCK-spaced intervals (minimum 120 ms). Every capture is protected until its first clear answer finishes. When both buffers are protected, incoming captures are skipped, not queued. Later responses can be retired with a short fade to make room. A held tone or CORE freeze can now keep creating responses; finite decay starts after incoming audio stops qualifying. Every admitted phrase retains its original deadline. This is a selective phrase sampler, not a guarantee that every played note repeats.

This test build preserves CORE 0.3-0 for comparison. Do not install the legacy custom ECHO/SPACE as part of the preferred chain. Start with FIELD alone and reverb off; add CORE after the standalone checks pass. Optional built-in reverb comes last.

See [hardware QA](docs/qa-0.1-2.md), [implementation specification](../../../LATTICE_FIELD_SPEC.md), and [build report](../../../reports/lattice/2026-09-11_field-0.1-2-build.md).

Known limitations carried into this focused build: quiet/soft attacks can capture weak fragments; response timing includes capture duration (up to 85.33 ms); Ping-Pong balances channels without transferring a one-sided source; pitched ghosts can alias. These are documented pending separate approval.
