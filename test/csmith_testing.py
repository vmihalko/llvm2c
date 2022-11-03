#!/usr/bin/env python3

num_tests = 3
csmith_runtime_dir = 'csmith-runtime'

import tempfile, os, getopt, sys, shutil
from subprocess import run, DEVNULL

# TODO:
# * hardcoded paths
# * csmith arguments [simple, full_grammar]
# * llvm2c coredump!

def run_tests(how_many, arg_compile, arg_keep, dir_name):
    for i in range(how_many):
        with tempfile.NamedTemporaryFile(dir=dir_name) as tmp_file_name:
            tmp_file = tmp_file_name.name
            tmp_file_c = tmp_file + '.c'
            tmp_file_re_c = tmp_file + '_re.c'
            tmp_file_ll = tmp_file + '.ll'
            run(['csmith', '--output',  tmp_file_c])
            run(['clang', '-S', '-emit-llvm', tmp_file_c, '-o', tmp_file_ll], stdout=DEVNULL, stderr=DEVNULL)
            proc = run(['../build/llvm2c', '-o', tmp_file_re_c, tmp_file_ll])

            if proc.returncode != 0:
                print("llvm2c was not able to process inputed file!")
                break

            if arg_compile:
                proc = run(['clang', tmp_file_re_c])
                if proc.returncode != 0:
                    print("llv2c outputed file is uncompilable!")
                    break


def main(argv):
    try:
        opts, args = getopt.getopt(argv, "cekh",["--compile", "--execute", "--keep", "--tests"])
    except:
        print("argument parsing error")

    dirname = 'csmith-tests'
    if os.path.exists(dirname):
        shutil.rmtree(dirname)
    os.mkdir(dirname)
    
    run_tests(1, False, False, dirname)


if __name__ == '__main__':
    main(sys.argv)
