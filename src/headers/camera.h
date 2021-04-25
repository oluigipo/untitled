#pragma once
#include "types.h"

struct Camera {
	vec2 pos;
	vec2 targetPos;
	f32 angle;
	f32 zoom;
	f32 speed;
};

void camera_update(struct Camera* restrict camera);
void camera_matrix(struct Camera* restrict cam, mat4 out);
void camera_mouse_pos(struct Camera* restrict cam, vec2 out);

