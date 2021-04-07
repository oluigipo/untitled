#include "headers/camera.h"
#include "headers/opengl.h"

void camera_update(struct Camera* restrict camera) {
	camera->angle += gamepad_axis(GPAD_AXIS_RX) * game.deltaTime * 0.05f;
	
	vec2 cameraTargetVel = {
		gamepad_is_down(GPAD_BUTTON_RIGHT) - gamepad_is_down(GPAD_BUTTON_LEFT),
		gamepad_is_down(GPAD_BUTTON_DOWN) - gamepad_is_down(GPAD_BUTTON_UP)
	};
	
	if (cameraTargetVel[0] != 0.0f || cameraTargetVel[1] != 0.0f) {
		glm_vec2_normalize(cameraTargetVel);
		glm_vec2_rotate(cameraTargetVel, -camera->angle, cameraTargetVel);
		glm_vec2_scale(cameraTargetVel, camera->speed * game.deltaTime, cameraTargetVel);
	}
	
	glm_vec2_lerp(camera->vel, cameraTargetVel, 0.3f * game.deltaTime, camera->vel);
	glm_vec2_add(camera->pos, camera->vel, camera->pos);
}

void camera_matrix(struct Camera* restrict cam, mat4 out) {
	glm_mat4_identity(out);
	
	glm_rotate(out, cam->angle, (vec3) { 0.0f, 0.0f, 1.0f });
	glm_scale(out, (vec3) { cam->zoom, cam->zoom });
	glm_translate(out, (vec3) { -cam->pos[0], -cam->pos[1] });
	
	glm_mat4_mul(game.projection, out, out);
}

