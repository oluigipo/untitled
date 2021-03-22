#pragma once

#include "types.h"
#include "std.h"
#include "texture.h"

struct TextVertexInfo {
	vec2 offset;
	vec3 color;
	f32 chr;
};

uint text_render(struct Texture* restrict output, string text, const uint* restrict colorData, const struct Texture* restrict font);
void text_size(string text, vec2u out, const struct Texture* restrict font);

