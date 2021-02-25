// Textures Paths
const char* const assets_textures_names[] = {
#define _(name, path) path
#include "texture_list.h"
#undef _
};

// Textures Names
#define _(name, path) TEX_##name
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
	const char* const* names = assets_textures_names;
	
	for (; texture != end; ++texture, ++names) {
		int width, height, c;
		
		u8* data = stbi_load(*names, &width, &height, &c, 4);
		if (!data) {
			printf("Failed to load texture '%s', index %zu\n", *names, (usize)(texture - assets_textures) / sizeof(uint));
			
			continue;
		}
		
		glBindTexture(GL_TEXTURE_2D, *texture);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		free(data);
	}
}

void texture_free_assets(void) {
	glDeleteTextures(TEXTURE_COUNT, assets_textures);
}

