#pragma once

#include "types.h"
#include <glad/glad.h>
#include <glfw3.h>
#include <cglm/cglm.h>
#include <cglm/vec4.h>

typedef uint vec2u[2];
typedef int vec2i[2];

enum {
	ALIGNMENT_NONE = 0,
	ALIGNMENT_LEFT = 1,
	ALIGNMENT_RIGHT = 2,
	ALIGNMENT_CENTER = 3,
	ALIGNMENT_XAXIS = 3,
	
	ALIGNMENT_TOP = 4,
	ALIGNMENT_BOTTOM = 8,
	ALIGNMENT_MIDDLE = 12,
	ALIGNMENT_YAXIS = 12,
};

internal void argb_to_color(u32 blending, vec4 output) {
	output[0] = (f32)((blending >> 16) & 0xFF) / 255.0f; // R
	output[1] = (f32)((blending >> 8 ) & 0xFF) / 255.0f; // G
	output[2] = (f32)((blending >> 0 ) & 0xFF) / 255.0f; // B
	output[3] = (f32)((blending >> 24) & 0xFF) / 255.0f; // A
}

