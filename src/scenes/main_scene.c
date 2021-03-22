#include "headers/all.h"

uint scene_main(void) {
	Shader shader = shader_load("res/shader");
	
	Uniform uniformTex = shader_uniform(shader, "uTexture");
	Uniform uniformProj = shader_uniform(shader, "uProjection");
	Uniform uniformObj = shader_uniform(shader, "uObject");
	
	mat4 object;
	
	uint vbo, vao;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	f32 vertices[] = {
		// positions           color                texcoords
		0.0f, 1.0f, 0.0f,      1.0f, 1.0f, 1.0f,    0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,      1.0f, 1.0f, 1.0f,    1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,    1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,    1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,    0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,      1.0f, 1.0f, 1.0f,    0.0f, 0.0f,
	};
	
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(f32) * 8, 0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(f32) * 8, (void*)(sizeof(f32) * 3));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(f32) * 8, (void*)(sizeof(f32) * 6));
	
	Texture text;
	string str = strlit("Hello, World! \x01\nThis is a sample text.");
	uint colorData[] = {
		5, 0xFF0000,
		8, 0x00FFFF,
		0, 0xFFFFFF
	};
	
	if (0 != text_render(&text, str, colorData, &assets_textures[TEX_DEFAULT_FONT])) {
		debug_error("Failed.\n");
		return 1;
	}
	
	// Walle things
	f64 angle = 0.0f;
	
	vec2 position = { game.window.width / 2, game.window.height / 2 };
	vec2 velocity = { 0 };
	
	// Particles
	ParticleManager mgr = { 0 };
	
	// Game Loop
	while (!glfwWindowShouldClose(game.apiWindow)) {
		engine_begin_frame();
		
		// Update
		if (keyboard_is_pressed(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(game.apiWindow, true);
		
		f32 targetSpeed = game.deltaTime * 5.0f + (gamepad_is_down(GPAD_BUTTON_A) * 3.0f);
		velocity[0] = lerpf(velocity[0], gamepad.state.axes[GPAD_AXIS_LX] * targetSpeed, 0.3f);
		velocity[1] = lerpf(velocity[1], gamepad.state.axes[GPAD_AXIS_LY] * targetSpeed, 0.3f);
		
		position[0] += velocity[0];
		position[1] += velocity[1];
		
		angle += gamepad.state.axes[GPAD_AXIS_RX] * game.deltaTime * 0.05f;
		
		glm_mat4_identity(object);
		glm_translate(object, (vec3) { position[0], position[1] });
		glm_scale(object, (vec3) { 256.0f, 256.0f });
		glm_rotate(object, angle, (vec3) { 0.0f, 0.0f, 1.0f });
		glm_translate(object, (vec3) { -0.5f, -0.5f });
		
		if (gamepad_is_down(GPAD_BUTTON_B)) {
			partmgr_add(&mgr, PART_SIMPLE, &(struct PartSimple) {
							.pos = { position[0], position[1] },
							.speed = { random_f64() * 18.0 - 8.0f, random_f64() * 16.0f - 8.0f },
							.color = { random_f64(), random_f64(), random_f64() },
							.alpha = 1.0f,
							.angle = random_f64() * TAU
						});
		}
		
		partmgr_update(&mgr);
		
		// Draw
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Draw walle
		glBindVertexArray(vao);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, assets_textures[TEX_WALLE].id);
		
		shader_bind(shader);
		
		glUniform1i(uniformTex, 0);
		glUniformMatrix4fv(uniformProj, 1, false, (f32*)game.projection);
		glUniformMatrix4fv(uniformObj, 1, false, (f32*)object);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// Draw text
		glm_mat4_identity(object);
		glm_translate(object, (vec3) { game.window.width / 2, game.window.height / 2 });
		glm_scale(object, (vec3) { text.size[0] * 5.0f, text.size[1] * 5.0f });
		glm_translate(object, (vec3) { -0.5f, -0.5f });
		
		glBindTexture(GL_TEXTURE_2D, text.id);
		glUniform1i(uniformTex, 0);
		glUniformMatrix4fv(uniformProj, 1, false, (f32*)game.projection);
		glUniformMatrix4fv(uniformObj, 1, false, (f32*)object);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		shader_unbind();
		
		mat4 view;
		glm_mat4_identity(view);
		partmgr_render(&mgr, view);
		
		engine_end_frame();
	}
	
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	shader_unload(shader);
	texture_free(&text);
	
	// Close the game
	game.currentScene = NULL;
	
	return 0;
}
