#pragma once

#include "types.h"
#include "global.h"

uint engine_init(const struct GameArgs* restrict args);
void engine_begin_frame(void);
void engine_end_frame(void);
void engine_deinit(void);
