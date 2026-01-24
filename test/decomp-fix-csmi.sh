#!/bin/bash

# check if file path is provided
if [[ -z "$1" ]]; then
  echo "Please provide a file path"
  exit 1
fi

# check if file exists
if [[ ! -f "$1" ]]; then
  echo "File $1 does not exist"
  exit 1
fi


# perform the replacement using sed
sed -i '1 i\#include "csmith.h"' "$1"
sed -i 's/extern void transparent_crc(\(.*\)unsigned long\(.*\)unsigned char\(.*\)unsigned int\(.*\));/extern void transparent_crc(\1uint64_t\2char\3int\4);/g' "$1"
sed -i 's/extern void platform_main_end(\(.*\)unsigned int\(.*\),\(.*\)unsigned int\(.*\));/extern void platform_main_end(\1uint32_t\2, int\4);/g' "$1"
