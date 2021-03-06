#pragma once

#include "types.h"
#include "opengl.h"

struct TextureLoadInfo {
	const char* restrict path;
	
	union {
		struct {
#if 0
			______something;
#endif
			uint tileWidth, tileHeight;
		};
		vec2u tileSize;
	};
};

typedef struct Texture Texture;
struct Texture {
	uint id;
	vec2u size;
	uint depth;
};

#define _(name, ...) TEX_##name,
enum {
#include "texture_list.h"
	TEXTURE_COUNT
};
#undef _

global Texture assets_textures[TEXTURE_COUNT];

void texture_load_assets(void);
void texture_free_assets(void);
void texture_free(Texture* restrict texture);

