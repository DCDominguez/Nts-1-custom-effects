# PARALLAX Test Plan

PARALLAX is staged so the spatial field, pitch engine, and final constellation are validated separately on the physical NTS-1.

## M0 — Toolchain

Pass when:

- Korg `logue-sdk` clones successfully
- submodules initialize
- official `platform/nutekt-digital/dummy-delfx` builds
- `make install` produces an `.ntkdigunit`
- the stock unit loads on the original NTS-1

## M1 — Four-voice spatial field

### Test sources

Use at least:

- sine or triangle sustained note
- saw sustained note
- short pluck
- full stereo material through the audio input

### Basic checks

- all four taps are audible at high MIX
- taps have clearly different arrival times
- no two voices modulate in obvious lockstep
- stereo anchors are distributed left-to-right
- TIME/SPREAD smoothly expands and contracts the timing field
- DEPTH/DIVERGENCE increases motion without producing discontinuities
- MIX moves predictably between dry and wet
- sustained tones do not click at LFO extrema
- input stereo orientation is preserved

### Mono check

Collapse the output to mono externally.

Listen for:

- severe recurring cancellation
- one SPREAD range disappearing entirely
- unacceptable level pumping caused by correlated taps

Some combing is expected from any chorus; total or rhythmically repeating collapse is not.

## M2 — One true pitch-shifted voice

Begin with Voice A only.

Test fixed offsets:

```text
-100 cents
-50 cents
-12 cents
-7 cents
+7 cents
+12 cents
+50 cents
+100 cents
```

Then test semitone intervals:

```text
-12
-7
+7
+12 semitones
```

Check:

- offset remains approximately stable instead of behaving only like vibrato
- grain/reset transitions do not click
- no large gain pulses at crossfades
- acceptable artifacts on sine, saw and pluck sources
- CPU remains stable while the other M1 voices run

## M3 — Multi-pitch field

Enable pitch shifting incrementally:

```text
1 voice → 2 voices → 4 voices
```

At each step test:

- CPU stability
- wet-sum headroom
- stereo image
- grain beating between voices
- whether added voices improve the effect enough to justify their cost

Stop escalation if reliability degrades. A `2 shifted + 2 conventional chorus` architecture is an acceptable outcome.

## M4 — Default constellation

Starting target:

```text
A: -9c  / ~8 ms  / L75
B: -3c  / ~14 ms / L20
C: +4c  / ~23 ms / R25
D: +10c / ~36 ms / R78
```

Tune by ear on hardware.

Acceptance questions:

- Does one oscillator sound like several related performers?
- Is the dry note still intelligible?
- Are the voices distinct without sounding like a conventional echo cluster?
- Does high DIVERGENCE remain musical?
- Is low DIVERGENCE useful as a normal chorus rather than only as an effect demo?

## M5 — Spatial drift

If added:

- each voice must stay near its stereo anchor
- voices must not repeatedly cross the full stereo field
- stereo movement must be slower than obvious autopan
- mono compatibility must not materially worsen

## Stress test

For a release candidate:

- SPREAD minimum and maximum
- DIVERGENCE minimum and maximum
- MIX 0%, 50%, 100%
- rapid knob movements
- silence followed by transients
- hot input material
- at least 30 minutes continuous playback
- suspend/resume or effect switching if available in the test setup

The dry path must remain valid under every failure mode.
