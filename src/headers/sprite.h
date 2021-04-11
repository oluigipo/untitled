#pragma once
#include "types.h"
#include "texture.h"

void sprite_rendering_init(void);

struct SpriteBase {
	Texture* texture;
	vec2u offset;
	vec2u size;
};

struct Sprite {
	Texture* texture;
	vec2 texcoords;
	vec2 size;
} typedef Sprite;

Sprite sprite_craft(const struct SpriteBase* base);
void sprite_render(const Sprite* spr, const mat4 where);
void sprite_render_ext(const Sprite* spr, const mat4 where, u32 blending, u32 frame, u32 flags);

struct SpriteBatchElement {
	vec2 texcoords;
	vec2 size;
	vec4 blending;
	mat4 where;
};

struct SpriteBatch {
	struct SpriteBatchElement* elements;
	usize len;
	usize cap;
	Texture* texture;
} typedef SpriteBatch;

void sprite_batch_init(SpriteBatch* batch, usize cap, Texture* tex);
void sprite_batch_reserve_more(SpriteBatch* batch, usize count);
void sprite_batch_add(SpriteBatch* batch, const Sprite* spr, const mat4 where, u32 blending, u32 frame, u32 flags);
void sprite_batch_flush(SpriteBatch* batch);
void sprite_batch_render(const SpriteBatch* batch);
void sprite_batch_done(SpriteBatch* batch);

