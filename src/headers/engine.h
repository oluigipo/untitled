#pragma once

#include "types.h"
#include "global.h"

func uint engine_init(const struct GameArgs* restrict args);
func void engine_begin_frame(void);
func void engine_end_frame(void);
func void engine_deinit(void);
