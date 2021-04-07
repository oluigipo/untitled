#pragma once

#include "types.h"

void os_message_box(const char* restrict title, const char* restrict str);
void os_assertion_failure(const char* restrict what);
void os_switch_software_rendering(void);

