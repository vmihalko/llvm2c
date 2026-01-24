//===----------------------------------------------------------------------===//
// InsertVLADecls Pass – emit variable-length array declarations immediately
// before their first use in the generated C.
//===----------------------------------------------------------------------===//
#pragma once
#include "../core/Program.h"

// Top-level driver
void insertVLADecls(Program &P);
