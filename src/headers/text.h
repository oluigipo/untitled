#pragma once
#include "types.h"
#include "std.h"
#include "texture.h"

struct TextVertexInfo {
	vec2 offset;
	vec3 color;
	f32 chr;
};

void text_rendering_setup(void);
uint text_render_to_texture(Texture* restrict output, string text, const Texture* restrict font, const uint* restrict colorData);
uint text_render(string text, const mat4 where, const Texture* restrict font);
uint text_render_ext(string text, const mat4 where, const Texture* restrict font, const uint* restrict colorData, u32 flags);
void text_size(string text, vec2u out, const Texture* restrict font);

