#pragma once
#include "types.h"
#include "texture.h"

// NOTE(luigi): should NEVER EVER be zero-initialized!
//              use 'framebuffer_init()' for it!
struct Framebuffer {
	Texture tex;
	uint id;
} typedef Framebuffer;

void framebuffer_init(Framebuffer* fb, const vec2u size);
void framebuffer_deinit(Framebuffer* fb);
Texture framebuffer_extract(Framebuffer* fb);
void framebuffer_bind(Framebuffer* fb);
void framebuffer_unbind(void);
void framebuffer_resize(Framebuffer* fb, const vec2u size);

