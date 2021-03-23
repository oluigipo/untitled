#include "headers/camera.h"
#include "headers/opengl.h"

void camera_matrix(struct Camera* restrict cam, mat4 out) {
	glm_mat4_identity(out);
	
	glm_rotate(out, cam->angle, (vec3) { 0.0f, 0.0f, 1.0f });
	glm_scale(out, (vec3) { cam->zoom, cam->zoom });
	glm_translate(out, (vec3) { -cam->pos[0], -cam->pos[1] });
	
	glm_mat4_mul(game.projection, out, out);
}

