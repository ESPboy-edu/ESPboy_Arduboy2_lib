#!/usr/bin/env python3
"""Build test_fxdata.bin: the real FX data with the chapters swapped for tools/sim/test_chapters
(the chapter blob is the last thing in the FX data, so the art offsets don't move).
Usage: python3 tools/sim/make_test_fx.py   then   TESTS=1 FXDATA=test_fxdata.bin ./sim"""
import os, re, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.normpath(os.path.join(HERE, '..', '..'))
import glob, shutil, tempfile
blob = os.path.join(HERE, 'test_chapters.bin')
# the game checks the chapter count at boot, so pad the test chapters with the real later ones
tests = sorted(glob.glob(os.path.join(HERE, 'test_chapters', '[0-9]*.txt')))
real = sorted(glob.glob(os.path.join(ROOT, 'chapters', '[0-9]*.txt')))
tmp = tempfile.mkdtemp()
for i, f in enumerate(tests + real[len(tests):]):
    shutil.copy(f, os.path.join(tmp, f"{i:02d}_{os.path.basename(f)}"))
r = subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'chapters', 'make_chapters.py'),
                    '--src', tmp, '--bin', blob, '--no-header'])
if r.returncode: sys.exit(r.returncode)
off = int(re.search(r'chapters\s*=\s*(0x[0-9A-Fa-f]+|\d+)', open(os.path.join(ROOT, 'fxdata', 'fxdata.h')).read()).group(1), 0)
data = open(os.path.join(ROOT, 'fxdata', 'fxdata-data.bin'), 'rb').read()
open(os.path.join(HERE, 'test_fxdata.bin'), 'wb').write(data[:off] + open(blob, 'rb').read())
print("test_fxdata.bin written")
