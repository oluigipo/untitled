#pragma once
#include "types.h"

struct Tilemap {
	Texture* texture;
	
	vec2u size;
	u16* data;
};

func void tilemap_render(const struct Tilemap* tilemap, const mat4 where);

