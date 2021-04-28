#pragma once
#include "types.h"

func void primitive_rendering_init(void);
func void primitive_render_rect(const mat4 where, u32 color);
func void primitive_render_roundrect(const mat4 where, u32 color, f32 roundness, vec2 scale);
func void primitive_render_ellipse(const mat4 where, u32 color);

