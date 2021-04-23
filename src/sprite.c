#include "headers/all.h"

internal struct SpriteRenderingStuff {
	Shader shader;
	Uniform uTexture;
} spriteRendering;

internal SpriteBatch spriteGlobalBatch = { 0 };

void sprite_rendering_init(void) {
	spriteRendering.shader = shader_load("res/sprite");
	spriteRendering.uTexture = shader_uniform(spriteRendering.shader, "uTexture");
}

//~ Simple Sprites
SpriteRenderable sprite_craft(const Sprite* base) {
	return (SpriteRenderable) {
		.texture = base->texture,
		.texcoords = {
			(f32)base->offset[0] / (f32)base->texture->size[0],
			1.0 - (f32)base->offset[1] / (f32)base->texture->size[1]
		},
		.size = {
			(f32)base->size[0] / (f32)base->texture->size[0],
			(f32)base->size[1] / (f32)base->texture->size[1]
		}
	};
}

void sprite_render(const Sprite* spr, const mat4 where) {
	sprite_render_ext(spr, where, 0xFFFFFFFF, 0, ALIGNMENT_NONE);
}

void sprite_render_ext(const Sprite* spr_, const mat4 where, u32 blending, u32 frame, u32 flags) {
	uint vbo, vao;
	uint quadVbo = shapes_quad();
	
	SpriteRenderable spr = sprite_craft(spr_);
	struct SpriteBatchElement element = {
		.texcoords = { spr.texcoords[0], spr.texcoords[1] },
		.size = { spr.size[0], spr.size[1] },
	};
	
	argb_to_color(blending, element.blending);
	
	glm_mat4_identity(element.where);
	glm_scale(element.where, (vec3) { spr.texture->size[0] * element.size[0], spr.texture->size[1] * element.size[1] });
	
	vec3 offset = { 0 };
	switch (flags & ALIGNMENT_XAXIS) {
		case ALIGNMENT_LEFT: offset[0] = 0.0f; break;
		case ALIGNMENT_RIGHT: offset[0] = -1.0f; break;
		case ALIGNMENT_CENTER: offset[0] = -0.5f; break;
	}
	
	switch (flags & ALIGNMENT_YAXIS) {
		case ALIGNMENT_TOP: offset[1] = 0.0f; break;
		case ALIGNMENT_BOTTOM: offset[1] = -1.0f; break;
		case ALIGNMENT_MIDDLE: offset[1] = -0.5f; break;
	}
	
	glm_translate(element.where, offset);
	glm_mat4_mul(where, element.where, element.where);
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	// Quad Data
	glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(f32) * 2, 0);
	
	// Sprites Data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof element, &element, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof element, offsetof(struct SpriteBatchElement, texcoords));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof element, (void*)offsetof(struct SpriteBatchElement, size));
	
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof element, (void*)offsetof(struct SpriteBatchElement, blending));
	
	for (uint i = 0; i < 4; ++i) {
		uint index = i + 4;
		glEnableVertexAttribArray(index);
		glVertexAttribDivisor(index, 1);
		glVertexAttribPointer(index, 4, GL_FLOAT, false, sizeof element, (char*)offsetof(struct SpriteBatchElement, where) + i * sizeof(vec4));
	}
	
	shader_bind(spriteRendering.shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, spr.texture->id);
	glUniform1i(spriteRendering.uTexture, 0);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	shader_unbind();
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

//~ Sprite Batching
void sprite_batch_init(SpriteBatch* batch, usize cap) {
	if (!batch)
		batch = &spriteGlobalBatch;
	
	batch->cap = cap;
	batch->len = 0;
	batch->texture = NULL;
	batch->elements = stack_alloc(&game.frameStack, sizeof(batch->elements[0]) * cap);
}

void sprite_batch_reserve_more(SpriteBatch* batch, usize count) {
	if (!batch)
		batch = &spriteGlobalBatch;
	
	usize newcap = batch->cap + count;
	void* newptr = stack_alloc(&game.frameStack, sizeof(batch->elements[0]) * newcap);
	
	if (batch->elements) {
		memcpy(newptr, batch->elements, sizeof(batch->elements[0]) * batch->len);
		stack_free(&game.frameStack, batch->elements);
	}
	
	batch->elements = newptr;
	batch->cap = newcap;
}

void sprite_batch_add(SpriteBatch* batch, const Sprite* spr_, const mat4 where, u32 blending, u32 frame, u32 flags) {
	if (!batch)
		batch = &spriteGlobalBatch;
	
	// Check for texture
	if (!batch->texture) {
		batch->texture = spr_->texture;
	} else {
		if (batch->texture->id != spr_->texture->id) {
			
#if 1 // TODO(luigi): decide which version is better: flush batch and begin batching sprites with this new texture OR just render the sprite with different texture. Using first option for now
			
			sprite_batch_flush(batch);
			debug_log("Warning: batch break!\n");
#else
			sprite_render_ext(spr_, where, blending, frame, flags);
			debug_log("Warning: trying to render sprite with texture %u on a batch with texture %u! Fallback to sprite_render_ext()\n", spr_->texture->id, batch->texture->id);
			return;
#endif
		}
	}
	
	// Reserve
	if (batch->len + 1 > batch->cap) sprite_batch_reserve_more(batch, 5);
	
	struct SpriteBatchElement* element = &batch->elements[batch->len++];
	SpriteRenderable spr = sprite_craft(spr_);
	
	// Set values
	argb_to_color(blending, element->blending);
	
	glm_vec2_copy(spr.texcoords, element->texcoords);
	glm_vec2_copy(spr.size, element->size);
	element->texcoords[0] += element->size[0] * frame;
	
	// Matrix Magic
	glm_mat4_identity(element->where);
	glm_scale(element->where, (vec3) { spr.texture->size[0] * element->size[0], spr.texture->size[1] * element->size[1] });
	
	vec3 offset = { 0 };
	switch (flags & ALIGNMENT_XAXIS) {
		case ALIGNMENT_LEFT: offset[0] = 0.0f; break;
		case ALIGNMENT_RIGHT: offset[0] = -1.0f; break;
		case ALIGNMENT_CENTER: offset[0] = -0.5f; break;
	}
	
	switch (flags & ALIGNMENT_YAXIS) {
		case ALIGNMENT_TOP: offset[1] = 0.0f; break;
		case ALIGNMENT_BOTTOM: offset[1] = -1.0f; break;
		case ALIGNMENT_MIDDLE: offset[1] = -0.5f; break;
	}
	
	glm_translate(element->where, offset);
	glm_mat4_mul(where, element->where, element->where);
}

void sprite_batch_flush(SpriteBatch* batch) {
	if (!batch)
		batch = &spriteGlobalBatch;
	
	if (batch->len > 0)
		sprite_batch_render(batch);
	
	batch->texture = NULL;
	batch->len = 0;
}

void sprite_batch_render(const SpriteBatch* batch) {
	if (!batch)
		batch = &spriteGlobalBatch;
	
	uint vbo, vao;
	uint quadVbo = shapes_quad();
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	// Quad Data
	glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(f32) * 2, 0);
	
	// Sprites Data
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(batch->elements[0]) * batch->len, batch->elements, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof batch->elements[0], offsetof(struct SpriteBatchElement, texcoords));
	
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof batch->elements[0], (void*)offsetof(struct SpriteBatchElement, size));
	
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof batch->elements[0], (void*)offsetof(struct SpriteBatchElement, blending));
	
	for (uint i = 0; i < 4; ++i) {
		uint index = i + 4;
		glEnableVertexAttribArray(index);
		glVertexAttribDivisor(index, 1);
		glVertexAttribPointer(index, 4, GL_FLOAT, false, sizeof batch->elements[0], (char*)offsetof(struct SpriteBatchElement, where) + i * sizeof(vec4));
	}
	
	shader_bind(spriteRendering.shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, batch->texture->id);
	glUniform1i(spriteRendering.uTexture, 0);
	
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, batch->len);
	
	shader_unbind();
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void sprite_batch_done(SpriteBatch* batch) {
	if (!batch)
		batch = &spriteGlobalBatch;
	
	if (batch->elements)
		stack_free(&game.frameStack, batch->elements);
	
	batch->elements = NULL;
	batch->len = batch->cap = 0;
	batch->texture = NULL;
}

