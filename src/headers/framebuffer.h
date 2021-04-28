#pragma once
#include "types.h"
#include "texture.h"

// NOTE(luigi): should NEVER EVER be zero-initialized!
//              use 'framebuffer_init()' for it!
struct Framebuffer {
	Texture tex;
	uint id;
} typedef Framebuffer;

func void framebuffer_init(Framebuffer* fb, const vec2u size);
func void framebuffer_deinit(Framebuffer* fb);
func Texture framebuffer_extract(Framebuffer* fb);
func void framebuffer_bind(Framebuffer* fb);
func void framebuffer_unbind(void);
func void framebuffer_resize(Framebuffer* fb, const vec2u size);

