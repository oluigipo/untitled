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

