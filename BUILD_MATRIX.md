# Human Soon NTS-1 Build Matrix

Built in the consolidated CI workflow against the Korg logue SDK cloned at runtime. **Compilation is not physical hardware validation.**

| Project | Module | text | data | bss | total |
|---|---|---:|---:|---:|---:|
| SPECTRA | `osc` | 5164 | 1080 | 140 | 6384 |
| PARALLAX | `delfx` | 1480 | 16 | 65556 | 67052 |
| CHORDGHOST | `delfx` | 1204 | 24 | 1048588 | 1049816 |
| DUST | `modfx` | 1060 | 0 | 32 | 1092 |
| CARRIER | `modfx` | 1008 | 0 | 20 | 1028 |
| VECTORFILTER | `modfx` | 1040 | 8 | 24 | 1072 |
| IRONROT | `modfx` | 796 | 8 | 16 | 820 |
| ATTRACTOR | `modfx` | 1044 | 16 | 20 | 1080 |
| ZEROCROSS | `modfx` | 1248 | 16 | 8200 | 9464 |
| PHASEWELL | `modfx` | 1144 | 16 | 60 | 1220 |
| ASCENDER | `modfx` | 1084 | 16 | 100 | 1200 |
| HELIX | `modfx` | 1208 | 16 | 16392 | 17616 |
| CAPSTAN | `modfx` | 1420 | 24 | 32784 | 34228 |
| SIDEBAND | `modfx` | 1136 | 8 | 80 | 1224 |
| FAULTLINE | `modfx` | 912 | 16 | 24 | 952 |
| BALLISTIC | `delfx` | 1140 | 24 | 524292 | 525456 |
| RAINFALL | `delfx` | 1264 | 80 | 524300 | 525644 |
| SWARMDELAY | `delfx` | 1220 | 24 | 1048580 | 1049824 |
| GLITCHREPEAT | `delfx` | 1016 | 32 | 262164 | 263212 |
| BUCKETLINE | `delfx` | 1252 | 24 | 524304 | 525580 |
| LONGMEMORY | `delfx` | 1032 | 24 | 1048588 | 1049644 |
| SHARD | `delfx` | 1208 | 24 | 131080 | 132312 |
| ABYSS | `revfx` | 1716 | 24 | 131080 | 132820 |
| AUREOLE | `revfx` | 1612 | 24 | 98316 | 99952 |
| NEBULA | `revfx` | 1392 | 64 | 262148 | 263604 |

Validated consolidated source commit: `af3a01e43176b2859cd978cf2bb11df2e92eaf24`

Consolidated CI run: `34302084686` — **PASS** for all 25 units.

The generated test artifact contains all 25 `.ntkdigunit` binaries, all 25 project-specific QA sheets, this build matrix, and the master hardware testing queue.

## Interpretation

- `text` / `data` / `bss` are reported by `arm-none-eabi-size` on each packaged build's ELF.
- Large `bss` values for delay/reverb projects are primarily their declared audio history buffers, including SDRAM sections in the Korg template/linker layout.
- These numbers are build-footprint records, not CPU-usage measurements.
- Physical MkI load/stability tests remain mandatory, especially for the largest buffer projects.
