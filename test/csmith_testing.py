#!/usr/bin/env python3

num_tests = 1000
csmith_runtime_dir = 'csmith-runtime'

import tempfile, os
from subprocess import run, DEVNULL

# with tempfile.TemporaryDirectory() as dirname:
dirname = 'csmith-tests'
for i in range(num_tests):
    run(['./csmith', '--output', dirname + '/program.c'])
    run(['clang', '-I', csmith_runtime_dir, '-S', '-emit-llvm', dirname + '/program.c'])
    proc = run(['./llvm2c', '-p', dirname + '/program.ll'], stdout=DEVNULL)
    if proc.returncode != 0:
        print("Found bug!")
        break
