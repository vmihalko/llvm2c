#!/bin/sh

# Check that two arguments were provided
#if [ $# -ne 2 ]; then
#  echo "Usage: $0 <llvm2c_executable> <reduce_string>"
#  exit 1
#fi

if [ ! -e toreduce.c ]; then
    echo "toreduce.c must exist in this directory."
    exit 1
fi

# Assign arguments to variables

llvm2c_executable="/home/vmihalko/DIPLO/llvm2c/build/llvm2c"
echo $llvm2c_executable
if [ ! -x "$llvm2c_executable" ]; then
  echo "$llvm2c_executable  no such file or not executable"
  exit 1
fi

reduce_string="to parameter of incompatible type"
if [ ${#reduce_string} -lt 2 ]; then
  echo "String length must be at least 2 characters $reduce_string"
  exit 1
fi

clang_flags="-S -emit-llvm -Xclang -disable-O0-optnone -g"

clang $clang_flags toreduce.c -o toreduce.ll &>/dev/null
if [ ! -e toreduce.ll ]; then
    echo "clang failed during generation toreduce.ll file."
    exit 1
fi

"$llvm2c_executable" "-add-includes" toreduce.ll -o toreduce-decmpld.c &>/dev/null

if [ ! -e toreduce-decmpld.c ]; then
    echo "llvm2c_executable failed during creation of toreduce-decmpld.c file."
    exit 1
fi

# from LLVM2C
../decomp-fix-csmi.sh toreduce-decmpld.c
clang toreduce-decmpld.c 2>&1 | grep "$reduce_string"

#TODO
# in LLVM2C
# FROM LLVM2C 

