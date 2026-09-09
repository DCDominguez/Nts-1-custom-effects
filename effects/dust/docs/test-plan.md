# DUST Test Plan

Target: physical Korg Nu:Tekt NTS-1 digital kit MkI.

## M0 — load and basic signal

- Load `dust.ntkdigunit` with NTS-1 Sound Librarian.
- Confirm the unit appears in a modulation FX user slot.
- Start with TIME=0 and DEPTH=0.
- Confirm dry audio passes with correct L/R orientation.

## M1 — RATE

Use sustained sine, saw, drum loop, and full-range music.

Sweep TIME from minimum to maximum with DEPTH=100%.

Pass when:

- minimum RATE setting is close to full-bandwidth behavior
- aliasing and stair-step character increase progressively
- no hard clicks from ordinary knob movement
- no stuck sample after returning TIME to minimum
- both channels remain active

Test points:

```text
TIME: 0%, 10%, 25%, 50%, 75%, 100%
```

## M1 — DAMAGE

Set TIME around 25% and sweep DEPTH.

Pass when:

- DEPTH=0 is effectively dry
- increasing DEPTH progressively increases quantization character
- maximum DEPTH is clearly coarse but bounded
- no large gain discontinuities
- digital silence remains silent

Test points:

```text
DEPTH: 0%, 10%, 25%, 50%, 75%, 100%
```

## M2 — stereo fracture

Use a mono source copied equally to left and right.

Compare:

```text
TIME 25 / DEPTH 25
TIME 50 / DEPTH 50
TIME 75 / DEPTH 75
TIME 100 / DEPTH 100
```

Expected:

- low settings remain mostly centered
- strong settings produce a bounded stereo alias difference
- no channel disappears
- no uncontrolled image flipping

Collapse output to mono externally and check for unacceptable cancellation.

## Transient test

Use short kicks, snares, and plucks.

Check:

- attack remains present at mild settings
- sample-hold does not create unexplained bursts above input level
- rapid note retriggers do not lock the reducer

## Parameter-abuse test

Rapidly alternate TIME and DEPTH between minimum and maximum for at least 30 seconds.

Pass when:

- no NaN/Inf-like blast
- no permanent mute
- no stuck held value
- returning both controls to zero restores clean signal

## Silence / resume test

- feed digital silence for 30 seconds
- resume with a transient
- switch away from DUST and back if possible

Pass when no stale held sample is emitted as a loud transient.

## Long stability test

Run worst-case settings for at least 30 minutes:

```text
TIME = 100%
DEPTH = 100%
```

Pass when output remains bounded and the NTS-1 remains responsive.
