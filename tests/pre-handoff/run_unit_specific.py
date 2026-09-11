#!/usr/bin/env python3
import os
import pathlib
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
STUBS = ROOT / "tests" / "pre-handoff" / "stubs"
TESTS = ROOT / "tests" / "pre-handoff" / "unit_specific"
OUT = ROOT / "tests" / "pre-handoff" / "unit-specific-results.md"

CASES = [
    ("SPECTRA", TESTS / "test_spectra.cpp"),
    ("DUST", TESTS / "test_dust.cpp"),
    ("CARRIER", TESTS / "test_carrier.cpp"),
    ("VECTORFILTER", TESTS / "test_vectorfilter.cpp"),
    ("ATTRACTOR", TESTS / "test_attractor.cpp"),
    ("CHORDGHOST", TESTS / "test_chordghost.cpp"),
    ("PHASEWELL", TESTS / "test_phasewell.cpp"),
    ("ASCENDER", TESTS / "test_ascender.cpp"),
    ("HELIX", TESTS / "test_helix.cpp"),
    ("CAPSTAN", TESTS / "test_capstan.cpp"),
    ("SIDEBAND", TESTS / "test_sideband.cpp"),
    ("FAULTLINE", TESTS / "test_faultline.cpp"),
    ("BALLISTIC", TESTS / "test_ballistic.cpp"),
    ("RAINFALL", TESTS / "test_rainfall.cpp"),
    ("SWARMDELAY", TESTS / "test_swarmdelay.cpp"),
    ("GLITCHREPEAT", TESTS / "test_glitchrepeat.cpp"),
    ("BUCKETLINE", TESTS / "test_bucketline.cpp"),
    ("LONGMEMORY", TESTS / "test_longmemory.cpp"),
    ("SHARD", TESTS / "test_shard.cpp"),
    ("ABYSS", TESTS / "test_abyss.cpp"),
    ("AUREOLE", TESTS / "test_aureole.cpp"),
    ("LATTICE CORE", TESTS / "test_lattice_core.cpp"),
]


def run_case(name: str, src: pathlib.Path):
    with tempfile.TemporaryDirectory(prefix="a_class_") as td:
        exe = pathlib.Path(td) / "test"
        cmd = [
            "g++", "-std=c++17", "-O1", "-g",
            "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
            f"-I{ROOT}", f"-I{STUBS}",
            str(src), "-lm", "-o", str(exe),
        ]
        cp = subprocess.run(cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        if cp.returncode:
            return False, "compile: " + cp.stdout[-2000:].replace("\n", " | ")
        rp = subprocess.run(
            [str(exe)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
            env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0:halt_on_error=1"},
        )
        if rp.returncode:
            return False, "runtime: " + rp.stdout[-2000:].replace("\n", " | ")
        return True, rp.stdout.strip() or "PASS"


def main():
    rows = []
    failures = 0
    for name, src in CASES:
        ok, note = run_case(name, src)
        rows.append((name, "PASS" if ok else "FAIL", note))
        failures += 0 if ok else 1
        print(f"{name}: {'PASS' if ok else 'FAIL'} - {note}")

    with OUT.open("w") as f:
        f.write("# Project-specific A-class harness results\n\n")
        f.write("These tests exercise unit-specific behavioral contracts using the actual production DSP source. They supplement, rather than replace, the shared common gate and physical MkI/music checks.\n\n")
        f.write("| Unit | Result | Notes |\n|---|---:|---|\n")
        for name, result, note in rows:
            f.write(f"| {name} | **{result}** | {note.replace('|','/')} |\n")
        f.write(f"\n**Result: {len(rows)-failures}/{len(rows)} project-specific harnesses PASS.**\n")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
