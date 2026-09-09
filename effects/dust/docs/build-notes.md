# DUST Build Notes

This file records compile-time observations only. Physical behavior belongs in `qa-sheet.md`.

## Current target

- Korg `nutekt-digital`
- `modfx`
- API `1.1-0`
- source: `nts1/src/dust.cpp`

## Build gate

A build is considered ready for hardware QA only when:

- GitHub Actions completes successfully
- `dust.ntkdigunit` is packaged
- linker map/list artifacts are retained
- program/data/BSS sizes are recorded

Hardware validation is a separate status and must not be inferred from compile success.
