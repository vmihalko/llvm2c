#!/usr/bin/env python3

import argparse, tempfile, os, getopt, sys, shutil, subprocess
csmith_runtime_dir = 'csmith-runtime'

# TODO:
# * hardcoded paths to scripts and  llvm2c binary 
# * add possibility to modify csmith flags

csmith_options = [ '--no-bitfields', '--no-comma-operators', '--no-math64', '--inline-function', '--no-unions', '--no-volatiles', '--no-safe-math', '--no-consts', '--no-const-pointers']

def generate_test():
    llvm2c = "/home/vmihalko/DIPLO/llvm2c/build/llvm2c"
    fcsmth = "/home/vmihalko/DIPLO/llvm2c/test/fix-csmi.sh"
    dfcsmth = "/home/vmihalko/DIPLO/llvm2c/test/decomp-fix-csmi.sh"
    # Create a temporary directory to store the generated files
    with tempfile.TemporaryDirectory() as tmpdir:
        # Create a temporary file to store the output of csmith
        with tempfile.NamedTemporaryFile(mode='w+', suffix='.c', delete=False) as tmpfile:
            dbg_files = []

            # Generate C code using csmith and write it to the temporary file
            csmith = subprocess.run(['csmith'] + csmith_options + ['--output', tmpfile.name], check=True)
            print(f"Executed {' '.join(csmith.args)}")


            # Compile the C code using clang
            try:
                subprocess.run(['clang', '-o', os.path.join(tmpdir, 'origin'), tmpfile.name], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                dbg_files.append(os.path.join(tmpdir, 'origin'))
            except subprocess.CalledProcessError as e:
                cp = subprocess.run(['cp'] + ['-v'] + dbg_files + [ '/tmp/'], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                print(f'ERROR: The origin executable {tmpfile.name} failed to compile.\n For dbg purpose {" ".join(cp.args[1:-1])} copied to /tmp/')
                print(e)
                return False

            # Call the fix-csmith.sh script on the temporary file
            subprocess.run([fcsmth, tmpfile.name], check=True)
            
            # Generate LLVM IR code from the compiled code
            subprocess.run(['clang', '-g', '-S', '-emit-llvm', '-o', os.path.join(tmpdir, 'test.ll'), tmpfile.name], check=True,
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            
            dbg_files.append(os.path.join(tmpdir, 'test.ll'))

            # Generate C code from the LLVM IR code using llvm2c
            try:
                subprocess.run([llvm2c, '-add-includes', '-o', os.path.join(tmpdir, 'llvm2c_output.c'), os.path.join(tmpdir, 'test.ll')], check=True,
                               stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            except subprocess.CalledProcessError as e:
                cp = subprocess.run(['cp'] + ['-v'] + dbg_files + [ '/tmp/'], check=True)
                print(f'ERROR: The llvm2c executable crashed.\nFor dbg purpose {" ".join(cp.args[2:-1])} copied to /tmp/')
                print(e)
                return False

            # Call the decomp-fix-csmith.sh script on the llvm2c output
            subprocess.run([dfcsmth, os.path.join(tmpdir, 'llvm2c_output.c')], check=True)
            dbg_files.append(os.path.join(tmpdir, 'llvm2c_output.c'))

            # Compile the llvm2c output using clang
            try:
                subprocess.run(['clang', '-o', os.path.join(tmpdir, 'llvm2c_output'), os.path.join(tmpdir, 'llvm2c_output.c')], check=True,
                               stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                dbg_files.append(os.path.join(tmpdir, 'llvm2c_output'))
            except subprocess.CalledProcessError as e:
                cp = subprocess.run(['cp'] + ['-v'] + dbg_files + [ '/tmp/'], check=True, stderr=subprocess.DEVNULL)
                print(f'ERROR: The llvm2c_output executable failed to compile.\n For dbg purpose {" ".join(cp.args[1:-1])} copied to /tmp/')
                print(e)
                return False

            cp = subprocess.run(['cp'] + ['-v'] + dbg_files + [ '/tmp/'], check=True, stderr=subprocess.DEVNULL)
            # Run the test executable and get the output
            try:
                test_output = subprocess.run([os.path.join(tmpdir, 'origin')], stdout=subprocess.PIPE, check=True).stdout.decode()
            except subprocess.CalledProcessError as e:
                print("Program generated by csmith throws a exceptions.")
                return True

            # Run the llvm2c output executable and get the output
            llvm2c_output = subprocess.run([os.path.join(tmpdir, 'llvm2c_output')], stdout=subprocess.PIPE, check=True).stdout.decode()

            # Compare the outputs and print a message if they are different
            if test_output != llvm2c_output:
                print('ERROR: The outputs are different.')
                print(f'Test Output:\n{test_output}')
                print(f'llvm2c Output:\n{llvm2c_output}')
                return False

    # Remove the temporary file on success TODO: and other files e.g. test.ll etc.
    os.remove(tmpfile.name)
    return True

def main(num_tests=5):
    for i in range(num_tests):
        if not generate_test():
            break


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("num_tests", nargs="?", default=5, type=int)
    args = parser.parse_args()

    main(args.num_tests)
