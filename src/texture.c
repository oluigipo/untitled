#include "headers/texture.h"
#include "headers/opengl.h"
#include "headers/debug.h"
#include "headers/memory.h"
#include <stb_image.h>

internal const struct TextureLoadInfo assets_textures_info[] = {
#define _(name, ...) { __VA_ARGS__ },
#include "headers/texture_list.h"
#undef _
};

Texture assets_textures[TEXTURE_COUNT];

void texture_load_assets(void) {
	Texture* texture = assets_textures;
	Texture* const end = assets_textures + TEXTURE_COUNT;
	const struct TextureLoadInfo* info = assets_textures_info;
	
	for (; texture != end; ++texture, ++info) {
		glGenTextures(1, &texture->id);
		int width, height, c;
		
		u8* data = stbi_load(info->path, &width, &height, &c, 4);
		if (!data) {
			debug_error("Failed to load texture '%s', index %zu\n", info->path, (usize)(texture - assets_textures) / sizeof(*texture));
			
			continue;
		}
		
		// The field 'texarrayDim' will be 0 if it wasn't emmited in the initializer.
		// Only checking the width is enough.
		if (info->tileWidth != 0) {
			glBindTexture(GL_TEXTURE_2D_ARRAY, texture->id);
			
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
			uint subwidth = width / info->tileWidth;
			uint subheight = height / info->tileHeight;
			uint depth = subwidth * subheight;
			
			texture->size[0] = info->tileWidth;
			texture->size[1] = info->tileHeight;
			texture->depth = depth;
			
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, info->tileWidth, info->tileHeight, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			
			glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
			glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, height);
			
			for (uint y = 0; y < subheight; ++y) {
				for (uint x = 0; x < subwidth; ++x) {
					// printf("p: %u, %u\n", x, y);
					glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0,
									0, 0, x + subwidth * (subheight - y - 1),
									info->tileWidth, info->tileHeight, 1,
									GL_RGBA, GL_UNSIGNED_BYTE,
									data + ((y * info->tileHeight * width + x * info->tileWidth) * 4));
				}
			}
			
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
		} else {
			texture->size[0] = width;
			texture->size[1] = height;
			
			glBindTexture(GL_TEXTURE_2D, texture->id);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		
		mem_free(data);
	}
}

void texture_free_assets(void) {
	for (uint i = 0; i < TEXTURE_COUNT; ++i)
		glDeleteTextures(1, &assets_textures[i].id);
}

void texture_free(Texture* restrict texture) {
	glDeleteTextures(1, &texture->id);
}

