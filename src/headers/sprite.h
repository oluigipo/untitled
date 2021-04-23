#pragma once
#include "types.h"
#include "texture.h"

void sprite_rendering_init(void);

struct Sprite {
	Texture* texture;
	vec2u offset;
	vec2u size;
} typedef Sprite;

struct SpriteRenderable {
	Texture* texture;
	vec2 texcoords;
	vec2 size;
} typedef SpriteRenderable;

SpriteRenderable sprite_craft(const Sprite* base);
void sprite_render(const Sprite* spr, const mat4 where);
void sprite_render_ext(const Sprite* spr, const mat4 where, u32 blending, u32 frame, u32 flags);

struct SpriteBatchElement {
	vec2 texcoords;
	vec2 size;
	vec4 blending;
	mat4 where;
};

// Can be safely zero-initialized
struct SpriteBatch {
	struct SpriteBatchElement* elements;
	usize len;
	usize cap;
	Texture* texture;
} typedef SpriteBatch;

// NOTE(luigi): all the 'batch' parameters of those functions can be NULL. If it's NULL,
//              then they are going to affect the global sprite batch.
//
//              The global sprite batch is flushed and done'd when 'engine_end_frame()'
//              is called.
void sprite_batch_init(SpriteBatch* batch, usize cap);
void sprite_batch_reserve_more(SpriteBatch* batch, usize count);
void sprite_batch_add(SpriteBatch* batch, const Sprite* spr, const mat4 where, u32 blending, u32 frame, u32 flags);
void sprite_batch_flush(SpriteBatch* batch);
void sprite_batch_render(const SpriteBatch* batch);
void sprite_batch_done(SpriteBatch* batch);

