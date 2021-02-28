// Textures Paths
struct TextureLoadInfo {
	const char* restrict path;
	vec2 texarrayDim; // width + height
};

struct Texture {
	uint width, height;
	uint id;
};

const struct TextureLoadInfo assets_textures_info[] = {
#define _(name, ...) { __VA_ARGS__ },
#include "texture_list.h"
#undef _
};

// Textures Names
#define _(name, ...) TEX_##name,
enum {
#include "texture_list.h"
	TEXTURE_COUNT
};
#undef _

uint assets_textures[TEXTURE_COUNT];

void texture_load_assets(void) {
	glGenTextures(TEXTURE_COUNT, assets_textures);
	
	uint* texture = assets_textures;
	uint* const end = assets_textures + TEXTURE_COUNT;
	const struct TextureLoadInfo* info = assets_textures_info;
	
	for (; texture != end; ++texture, ++info) {
		int width, height, c;
		
		u8* data = stbi_load(info->path, &width, &height, &c, 4);
		if (!data) {
			printf("Failed to load texture '%s', index %zu\n", info->path, (usize)(texture - assets_textures) / sizeof(uint));
			
			continue;
		}
		
		// The field 'texarrayDim' will be 0 if it wasn't emmited in the initializer.
		// Only checking the width (index 0) is enough.
		if (info->texarrayDim[0] != 0) {
			glBindTexture(GL_TEXTURE_2D_ARRAY, *texture);
			
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
			uint subwidth = width / (uint)info->texarrayDim[0];
			uint subheight = height / (uint)info->texarrayDim[1];
			uint depth = subwidth * subheight;
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, info->texarrayDim[0], info->texarrayDim[1], depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			
			glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
			glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, height);
			
			for (int y = 0; y < subheight; ++y) {
				for (uint x = 0; x < subwidth; ++x) {
					// printf("p: %u, %u\n", x, y);
					glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
									0, 0, x + subwidth * (subheight - y - 1),
									(uint)info->texarrayDim[0], (uint)info->texarrayDim[1], 1,
									GL_RGBA, GL_UNSIGNED_BYTE,
									data + ((y * (uint)info->texarrayDim[1] * width + x * (uint)info->texarrayDim[0]) * 4));
				}
			}
			
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
		} else {
			glBindTexture(GL_TEXTURE_2D, *texture);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		
		free(data);
	}
}

void texture_free_assets(void) {
	glDeleteTextures(TEXTURE_COUNT, assets_textures);
}

