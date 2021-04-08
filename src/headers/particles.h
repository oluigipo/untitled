#pragma once
#include "types.h"

// NOTE(luigi): this should be a power of two
#define MAX_PARTICLE_COUNT 128

enum ParticleKind {
	PART_SIMPLE,
	
	PART__COUNT
};
typedef enum ParticleKind ParticleKind;

struct PartSimple {
	vec2 pos;
	vec2 scale;
	vec2 speed;
	vec3 color;
	f32 alpha;
	f32 angle;
};

#define _ParticleManager_field(T, name) struct __pmf_##name { u32 count; T data[MAX_PARTICLE_COUNT]; } name

typedef struct ParticleManager {
	_ParticleManager_field(struct PartSimple, simple);
	
	
} ParticleManager;

#undef _ParticleManager_field

void partmgr_setup_rendering(void);
void partmgr_add(ParticleManager* mgr, enum ParticleKind kind, void* partData);
void partmgr_update(ParticleManager* mgr);
void partmgr_render(ParticleManager* mgr, mat4 view);

