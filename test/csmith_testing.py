#!/usr/bin/env python3

num_tests = 1000
csmith_runtime_dir = 'csmith-runtime'

import tempfile, os
from subprocess import run, DEVNULL

dirname = 'csmith-tests'
for i in range(num_tests):
    run(['./csmith', '--output', dirname + '/program.c'])
    run(['clang', '-I', csmith_runtime_dir, '-S', '-emit-llvm', dirname + '/program.c'])
    proc = run(['./llvm2c', '-o', dirname + '/output.c', dirname + '/program.ll'])

    if proc.returncode != 0:
        print("Found bug!")
        break

    proc = run(['clang', '-I', csmith_runtime_dir, dirname + '/output.c'])
    if proc.returncode != 0:
        print("Found bug!")
        break
