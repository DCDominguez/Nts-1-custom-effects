# MkI-first expansion gate — 2026-09-11

## Decision

Human Soon will finish the original NTS-1 MkI engineering baseline before starting production ports to other logue SDK platforms.

Platform research and compatibility planning may continue, but implementation work for minilogue xd, prologue, NTS-1 MkII, NTS-3, microKORG2 and drumlogue is deferred until the MkI suite is in a sufficiently stable, testable state.

## Required MkI conditions before production expansion

1. Active MkI units are brought to the agreed A-class engineering standard or explicitly dispositioned.
2. Known MkI failures are resolved/retested or documented as accepted limitations, with LATTICE CORE high-TIME behavior and CORE + FIELD integration currently the highest-priority open items.
3. The 29-unit shared production-DSP common gate remains green.
4. Fresh original-NTS-1 ARM build/package CI remains green.
5. Each unit intended for porting has a behavioral contract strong enough to detect an accidental sonic/control regression during DSP-core extraction.
6. Hardware truth remains separate from host/CI truth.

## Why this order

The MkI suite is currently the musical and behavioral reference. Porting while that reference is still changing would multiply debugging surfaces and make it harder to distinguish platform-adapter defects from unresolved source-design defects.

A-class coverage gives us a repeatable contract before refactoring into portable DSP cores and platform wrappers.

## Expansion remains planned

The current platform roadmap is retained in `PLATFORM_SUPPORT.md`:

1. minilogue xd / prologue v1.1 audit;
2. NTS-1 MkII;
3. NTS-3;
4. microKORG2;
5. drumlogue where conceptually appropriate.

Korg websim support for NTS-1 MkII and NTS-3 remains a planned additional pre-hardware layer once those ports begin.

## Immediate engineering focus

Continue A-class backfill and unit-specific measurement tooling on MkI. LATTICE CORE remains the highest-priority deep harness because its physical high-TIME distortion is unresolved.

This is a planning/priority decision only. No production DSP behavior was changed by this report.
