#include "headers/all.h"

/*

 Game Ideas:
- Turn-based
- Maybe multiplayer
- "War Groove"-kinda style
- "pique bandeirinha"

*/


uint scene_main(void) {
	Shader shader = shader_load("res/shader");
	
	Uniform uniformTex = shader_uniform(shader, "uTexture");
	Uniform uniformView = shader_uniform(shader, "uView");
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
	char myText[512];
	char myTextEnd[] = " \x01\nThis is a sample text.";
	usize myTextEndLen = sizeof myTextEnd;
	
	memcpy(myText, locale_str(TXT_HELLO_WORLD).ptr, locale_str(TXT_HELLO_WORLD).len);
	memcpy(myText + locale_str(TXT_HELLO_WORLD).len, myTextEnd, myTextEndLen);
	
	string str = { .ptr = myText, .len = locale_str(TXT_HELLO_WORLD).len + myTextEndLen };
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
	vec2 position = { 0 };
	vec2 velocity = { 0 };
	
	// Particles
	ParticleManager mgr = { 0 };
	
	// Camera
	struct Camera camera = {
		.pos = { 0, 0 },
		.angle = 0,
		.zoom = 2,
		.speed = 3.0f
	};
	
	// Music
	uint buffer = sound_load_file("assets/test.ogg");
	uint source = sound_make_source();
	
	sound_source_buffer(source, buffer);
	sound_play_source(source);
	sound_source_attenuation(source, 1.0f, 300.0f, 10000.0f);
	
	b32 playingSound = true;
	
	// Game Loop
	while (!glfwWindowShouldClose(game.apiWindow)) {
		engine_begin_frame();
		
		// Update
		if (gamepad_is_pressed(GPAD_BUTTON_Y)) {
			playingSound = !playingSound;
			
			if (playingSound) sound_play_source(source);
			else sound_pause_source(source);
		}
		
		if (gamepad_is_pressed(GPAD_BUTTON_START))
			glfwSetWindowShouldClose(game.apiWindow, true);
		
		f32 targetSpeed = 3.0f + 2.0f * gamepad_axis(GPAD_AXIS_R2);
		velocity[0] = lerpf(velocity[0], gamepad_axis(GPAD_AXIS_LX) * targetSpeed, 0.3f * game.deltaTime);
		velocity[1] = lerpf(velocity[1], gamepad_axis(GPAD_AXIS_LY) * targetSpeed, 0.3f * game.deltaTime);
		
		position[0] += velocity[0] * game.deltaTime;
		position[1] += velocity[1] * game.deltaTime;
		
		if (gamepad_is_down(GPAD_BUTTON_A)) {
			f32 scale = random_f64() * 0.5f + 0.5f;
			
			partmgr_add(&mgr, PART_SIMPLE, &(struct PartSimple) {
							.pos = { position[0], position[1] },
							.scale = { scale, scale },
							.speed = { random_f64() * 18.0 - 8.0f, random_f64() * 16.0f - 8.0f },
							.color = { random_f64(), random_f64(), random_f64() },
							.alpha = 1.0f,
							.angle = random_f64() * TAU
						});
		}
		
		sound_source_position(source, (vec3) { position[0], position[1], -300.0f });
		
		partmgr_update(&mgr);
		
		mat4 view;
		camera_update(&camera);
		camera_matrix(&camera, view);
		
		// Draw
		glClearColor(0.0f, 0.6f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Draw walle
		glBindVertexArray(vao);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, assets_textures[TEX_WALLE].id);
		
		shader_bind(shader);
		
		glm_mat4_identity(object);
		glm_translate(object, (vec3) { position[0], position[1] });
		glm_scale(object, (vec3) { assets_textures[TEX_WALLE].size[0], assets_textures[TEX_WALLE].size[1] });
		glm_translate(object, (vec3) { -0.5f, -0.5f });
		
		glUniform1i(uniformTex, 0);
		glUniformMatrix4fv(uniformView, 1, false, (f32*)view);
		glUniformMatrix4fv(uniformObj, 1, false, (f32*)object);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// Draw text
		glm_mat4_identity(object);
		glm_scale(object, (vec3) { text.size[0] * 2, text.size[1] * 2 });
		glm_translate(object, (vec3) { -0.5f, -0.5f });
		
		glBindTexture(GL_TEXTURE_2D, text.id);
		glUniform1i(uniformTex, 0);
		glUniformMatrix4fv(uniformObj, 1, false, (f32*)object);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		shader_unbind();
		
		partmgr_render(&mgr, view);
		
		engine_end_frame();
	}
	
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	shader_unload(shader);
	texture_free(&text);
	
	sound_delete_source(source);
	sound_unload(buffer);
	
	// Close the game
	game.currentScene = NULL;
	
	return 0;
}
