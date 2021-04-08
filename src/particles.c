#include "headers/particles.h"
#include "headers/opengl.h"

#define partmgr_add_template(T, field) do {\
u32 index = mgr->field.count < MAX_PARTICLE_COUNT ? mgr->field.count++ : (random_u32() & (MAX_PARTICLE_COUNT-1));\
mgr->field.data[index] = partData ? *(T*)partData : (T) { 0 };\
} while (0)

void partmgr_add(ParticleManager* mgr, ParticleKind kind, void* partData) {
	switch (kind) {
		case PART_SIMPLE: partmgr_add_template(struct PartSimple, simple); break;
		default: debug_error("Unknown 'enum ParticleKind' entry %i.\n", kind); break;
	}
}

#undef partmgr_add_template

#define partmgr_remove(field, i) do {\
--mgr->field.count;\
if (mgr->field.count > i)\
mgr->field.data[i] = mgr->field.data[mgr->field.count];\
notdeleted = false;\
} while (0)

void partmgr_update(ParticleManager* mgr) {
	b32 notdeleted = true;
	
	// PART_SIMPLE
	assert(mgr->simple.count <= MAX_PARTICLE_COUNT);
	for (u32 i = 0; i < mgr->simple.count; i += notdeleted, notdeleted = true) {
		struct PartSimple* part = &mgr->simple.data[i];
		
		glm_vec2_lerp(part->speed, (vec2) { 0 }, 0.1f * game.deltaTime, part->speed);
		vec2 speedToAdd = { part->speed[0] * game.deltaTime, part->speed[1] * game.deltaTime };
		glm_vec2_add(part->pos, speedToAdd, part->pos);
		part->alpha -= 0.01f * game.deltaTime;
		part->angle += 0.05 * game.deltaTime;
		
		if (part->alpha <= 0.0f) {
			partmgr_remove(simple, i);
		}
	}
	
	// ...
}

#undef partmgr_remove

internal struct ParticleManagerRenderingData {
	uint vbo, vao;
	
	Shader shader;
	Uniform view;
	
	Uniform matrices;
	Uniform spriteIndex;
	Uniform texture;
	Uniform colors;
} partmgrRenderingData;

void partmgr_setup_rendering(void) {
	// Just a simple quad. TexCoords are calculated using 'vec2(position.x, 1.0 - position.y)'.
	f32 vertices[] = {
		// position
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};
	
	glGenBuffers(1, &partmgrRenderingData.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, partmgrRenderingData.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &partmgrRenderingData.vao);
	glBindVertexArray(partmgrRenderingData.vao);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(f32) * 2, 0);
	
	Shader shader = shader_load("res/particles");
	partmgrRenderingData.shader = shader;
	partmgrRenderingData.view = shader_uniform(shader, "uView");
	partmgrRenderingData.spriteIndex = shader_uniform(shader, "uSpriteIndex");
	partmgrRenderingData.texture = shader_uniform(shader, "uTexture");
	partmgrRenderingData.matrices = shader_uniform(shader, "uMatrices");
	partmgrRenderingData.colors = shader_uniform(shader, "uColors");
}

void partmgr_render(ParticleManager* mgr, mat4 view) {
	glBindVertexArray(partmgrRenderingData.vao);
	
	shader_bind(partmgrRenderingData.shader);
	glUniformMatrix4fv(partmgrRenderingData.view, 1, false, (f32*)view);
	glUniform1i(partmgrRenderingData.texture, 0);
	glUniform1f(partmgrRenderingData.spriteIndex, 0.0f);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, assets_textures[TEX_PARTICLES].id);
	
	mat4 objects[MAX_PARTICLE_COUNT];
	vec4 colors[MAX_PARTICLE_COUNT];
	
	// Render PartSimple
	for (u32 i = 0; i < mgr->simple.count; ++i) {
		struct PartSimple* part = &mgr->simple.data[i];
		
		glm_mat4_identity(objects[i]);
		glm_translate(objects[i], (vec3) { part->pos[0], part->pos[1] });
		glm_scale(objects[i], (vec3) {
					  assets_textures[TEX_PARTICLES].size[0] * 2 * part->scale[0],
					  assets_textures[TEX_PARTICLES].size[1] * 2 * part->scale[1]
				  });
		glm_rotate(objects[i], part->angle, (vec3) { 0.0f, 0.0f, 1.0f });
		glm_translate(objects[i], (vec3) { -0.5f, -0.5f });
		
		glm_vec4(part->color, part->alpha, colors[i]);
	}
	
	glUniformMatrix4fv(partmgrRenderingData.matrices, mgr->simple.count, false, (f32*)objects);
	glUniform4fv(partmgrRenderingData.colors, mgr->simple.count, (f32*)colors);
	
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, mgr->simple.count);
	
	shader_unbind();
}


