#pragma once
#include "types.h"

func usize file_read(const char* restrict fname, char* buffer, usize buffSize);
func usize file_dump(const char* restrict fname, char** out);
