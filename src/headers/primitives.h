#pragma once
#include "types.h"

void primitive_rendering_init(void);
void primitive_render_rect(const mat4 where, u32 color);
void primitive_render_roundrect(const mat4 where, u32 color, f32 roundness, vec2 scale);
void primitive_render_ellipse(const mat4 where, u32 color);

