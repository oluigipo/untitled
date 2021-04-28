#include "headers/all.h"

func void framebuffer_init(Framebuffer* fb, const vec2u size) {
	fb->tex.size[0] = size[0];
	fb->tex.size[1] = size[1];
	fb->tex.depth = 0;
	fb->tex.id = 0;
	fb->id = 0;
	
	glGenTextures(1, &fb->tex.id);
	glBindTexture(GL_TEXTURE_2D, fb->tex.id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size[0], size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glGenFramebuffers(1, &fb->id);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->tex.id, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		assert(!"Could not complete framebuffer!");
	}
	
	if (likely(game.framebufferStackSize > 0))
		glBindFramebuffer(GL_FRAMEBUFFER, game.framebufferStack[game.framebufferStackSize-1]->id);
}

func void framebuffer_deinit(Framebuffer* fb) {
	glDeleteTextures(1, &fb->tex.id);
	glDeleteFramebuffers(1, &fb->id);
	
	*fb = (Framebuffer) { 0 };
}

func Texture framebuffer_extract(Framebuffer* fb) {
	Texture tex = fb->tex;
	
	glDeleteFramebuffers(1, &fb->id);
	*fb = (Framebuffer) { 0 };
	return tex;
}

func void framebuffer_bind(Framebuffer* fb) {
	assert(game.framebufferStackSize < sizeof(game.framebufferStack) / sizeof(*game.framebufferStack));
	uint index = game.framebufferStackSize++;
	game.framebufferStack[index] = fb;
	
	glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
	glViewport(0, 0, fb->tex.size[0], fb->tex.size[1]);
}

func void framebuffer_unbind(void) {
	assert(game.framebufferStackSize > 1);
	Framebuffer* fb = game.framebufferStack[--game.framebufferStackSize - 1];
	
	glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
	glViewport(0, 0, fb->tex.size[0], fb->tex.size[1]);
}

func void framebuffer_resize(Framebuffer* fb, const vec2u size) {
	fb->tex.size[0] = size[0];
	fb->tex.size[1] = size[1];
	
	glBindTexture(GL_TEXTURE_2D, fb->tex.id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size[0], size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}
