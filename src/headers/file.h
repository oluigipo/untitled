#pragma once
#include "types.h"

usize file_read(const char* restrict fname, char* buffer, usize buffSize);
usize file_dump(const char* restrict fname, char** out);
