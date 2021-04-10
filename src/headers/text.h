#pragma once
#include "types.h"
#include "std.h"
#include "texture.h"

struct TextVertexInfo {
	vec2 offset;
	vec3 color;
	f32 chr;
};

enum {
	TEXTRENDER_NONE = 0,
	TEXTRENDER_LEFT = 1,
	TEXTRENDER_RIGHT = 2,
	TEXTRENDER_CENTER = 3,
	TEXTRENDER_XALIGN = 3,
	
	TEXTRENDER_TOP = 4,
	TEXTRENDER_BOTTOM = 8,
	TEXTRENDER_MIDDLE = 12,
	TEXTRENDER_YALIGN = 12,
};

void text_rendering_setup(void);
uint text_render_to_texture(Texture* restrict output, string text, const Texture* restrict font, const uint* restrict colorData);
uint text_render(string text, const mat4 where, const Texture* restrict font);
uint text_render_ext(string text, const mat4 where, const Texture* restrict font, const uint* restrict colorData, u32 flags);
void text_size(string text, vec2u out, const Texture* restrict font);

