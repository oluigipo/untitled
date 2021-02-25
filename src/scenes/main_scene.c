

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
		0.0f, 1.0f, 0.0f,      1.0f, 0.0f, 0.0f,    0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,      0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,      0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,      0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
		0.0f, 0.0f, 0.0f,      1.0f, 0.0f, 0.0f,    0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,      0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
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
	
	// Game Loop
	while (!glfwWindowShouldClose(game.apiWindow)) {
		engine_begin_frame();
		
		// Update
		if (keyboard_is_pressed(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(game.apiWindow, true);
		
		glm_mat4_identity(object);
		glm_translate(object, (vec3) { mouse.pos[0], mouse.pos[1] });
		glm_scale(object, (vec3) { 256.0f, 256.0f });
		glm_translate(object, (vec3) { -0.5f, -0.5f });
		
		// Draw
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, assets_textures[TEX_DEFAULT_FONT]);
		
		glUseProgram(shader);
		
		glUniform1i(uniformTex, 0);
		glUniformMatrix4fv(uniformProj, 1, false, (f32*)game.projection);
		glUniformMatrix4fv(uniformObj, 1, false, (f32*)object);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		engine_end_frame();
	}
	
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	shader_unload(shader);
	
	// Close the game
	game.currentScene = NULL;
	
	return 0;
}
