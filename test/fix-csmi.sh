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
sed -i -e 's/#include "csmith.h"/#include <string.h> \
#include <stdint.h>\n \
void crc32_gentab (void); \
void transparent_crc (uint64_t val, char* vname, int flag); \
void platform_main_begin(void); \
void platform_main_end(uint32_t x, int flag); \
extern uint32_t crc32_context;/g' "$1"
