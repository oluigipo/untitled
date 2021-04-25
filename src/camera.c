#include "headers/camera.h"
#include "headers/opengl.h"

void camera_update(struct Camera* restrict camera) {
	vec2 cameraTargetVel = {
		camera->targetPos[0] - camera->pos[0],
		camera->targetPos[1] - camera->pos[1]
	};
	
	glm_vec2_scale(cameraTargetVel, camera->speed * game.deltaTime, cameraTargetVel);
	glm_vec2_add(camera->pos, cameraTargetVel, camera->pos);
}

void camera_matrix(struct Camera* restrict cam, mat4 out) {
	glm_mat4_identity(out);
	
	glm_rotate(out, cam->angle, (vec3) { 0.0f, 0.0f, 1.0f });
	glm_scale(out, (vec3) { cam->zoom, cam->zoom });
	glm_translate(out, (vec3) { -(int)cam->pos[0], -(int)cam->pos[1] });
	
	glm_mat4_mul(game.projection, out, out);
}

void camera_mouse_pos(struct Camera* restrict cam, vec2 out) {
	// TODO(luigi): make it work with rotation
	
	f32 x = mouse.pos[0] - game.window.width / 2;
	f32 y = mouse.pos[1] - game.window.height / 2;
	
	out[0] = x / (cam->zoom / SCREEN_SCALE) + cam->pos[0];
	out[1] = y / (cam->zoom / SCREEN_SCALE) + cam->pos[1];
}
