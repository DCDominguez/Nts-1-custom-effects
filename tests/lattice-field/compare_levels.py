#!/usr/bin/env python3
"""Bounded A/B level and headroom check; not a hardware loudness guarantee."""
import math
import pathlib
import subprocess
import tempfile

root = pathlib.Path(__file__).resolve().parents[2]
old_commit = '5179491c69d74be15b3c79aaa06962368bf35387'
source = 'effects/lattice-field/nts1/src/lattice_field.cpp'
with tempfile.TemporaryDirectory(prefix='field-levels-') as work:
    work = pathlib.Path(work)
    old = work/'baseline.cpp'
    old.write_bytes(subprocess.check_output(['git','show',f'{old_commit}:{source}'],cwd=root))
    binaries=[]
    for name,path in [('baseline',old),('candidate',root/source)]:
        binary=work/name
        subprocess.run(['g++','-std=c++11','-O2','-Wall','-Wextra','-Werror',
            '-I',str(root/'tests/lattice-field/stubs'),
            f'-DFIELD_SOURCE="{path}"',str(root/'tests/lattice-field/level_probe.cpp'),'-o',str(binary)],check=True)
        binaries.append(binary)
    print('input_peak overlap first_answer_gain_dB old_guard_min new_guard_min old_peak new_peak new_guard_attacks')
    for amp in [.1,.4,.85]:
        for overlap in [0,1]:
            rows=[]
            for binary in binaries:
                rows.append(list(map(float,subprocess.check_output([str(binary),str(amp),str(overlap)],text=True).split())))
            old,new=rows
            gain=20*math.log10(new[1]/old[1])
            assert new[5]==0 and new[0]<1, 'clipped test output'
            if not overlap:
                assert old == new, 'isolated response changed from 0.1-1'
            if amp<=.4:
                if not overlap:
                    assert abs(gain)<.01, 'isolated first response gain changed'
                assert new[4]==0, 'ordinary source invokes wet guard'
            print(f'{amp:.2f} {overlap} {gain:.3f} {old[3]:.4f} {new[3]:.4f} {old[0]:.4f} {new[0]:.4f} {int(new[4])}')
    print('PASS: isolated 0.1-1 responses identical; dense inputs bounded without ordinary-level guard hits.')
    print('Loud-input gain is guard-limited. Desktop checks do not establish audible hardware pumping or CPU margin.')
