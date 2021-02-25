struct Camera {
	vec3 pos;
	vec3 vel;
	f32 speed;
	
	
};

void camera_setup(struct Camera* restrict cam) {
	cam = (struct Camera) {
		.speed = 0.5f
	};
}

void camera_update(struct Camera* restrict cam) {
	// ...
}

void camera_matrix(const struct Camera* restrict cam, mat4 out) {
	// ...
	
	glm_translate(out, cam->pos);
	// glm_rotate(out, );
}

