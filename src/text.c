// This is for rendering text

struct TextVertexInfo {
	vec2 offset;
	vec3 color;
};

uint text_render(struct Texture* restrict output, string text, const vec2 dim, u32 font) {
	static uint vao = 0;
	static uint staticVbo;
	static Shader shader;
	static Uniform uniformFit;
	static Uniform uniformTexture;
	
	static void* heapBuffer;
	static usize heapSize;
	
	if (!vao) {
		shader = shader_load("res/text");
		if (!shader) {
			printf("Could not load text shader.\n");
			return 1;
		}
		
		uniformFit = glGetUniformLocation(shader, "uFit");
		uniformTexture = glGetUniformLocation(shader, "uTexture");
		
		f32 vertices[] = {
			0.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 0.0f
		};
		
		glGenBuffers(1, &staticVbo);
		glBindBuffer(GL_ARRAY_BUFFER, staticVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
		
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		
		// position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(f32) * 4, 0);
		
		// texcoords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(f32) * 4, (void*)(sizeof(f32) * 2));
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	} else {
		glBindVertexArray(vao);
	}
	
	uint texture;
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Allocate vertex buffer
	uint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	usize bufferLen = text.len * sizeof(struct TextVertexInfo);
	if (heapSize < bufferLen) {
		void* newbuf = realloc(heapBuffer, bufferLen);
		if (!newbuf) {
			// panic
			printf("Could not allocate memory!\nExiting.\n");
			exit(1);
		}
		
		heapBuffer = newbuf;
		heapSize = bufferLen;
	}
	
	// Write vertex buffer & calculate texture size
	struct TextVertexInfo* infos = heapBuffer;
	uint width = 0;
	uint height = 0;
	
	uint x = 0, y = 0;
	uint xRecord = 0;
	
	for (uint i = 0; i < text.len; ++i) {
		infos[i].offset[0] = (f32)x;
		infos[i].offset[1] = (f32)y;
		
		infos[i].color[0] = 1.0f;
		infos[i].color[1] = 1.0f;
		infos[i].color[2] = 1.0f;
		
		if (text.ptr[i] == '\n') {
			++y;
			
			if (x > xRecord)
				xRecord = x;
			
			x = 0;
			
			continue;
		}
		
		++x;
	}
	
	if (x > xRecord)
		xRecord = x;
	
	width = xRecord * dim[0];
	height = (y + 1) * dim[1];
	
	glBufferData(GL_ARRAY_BUFFER, bufferLen + text.len, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, bufferLen, infos);
	glBufferSubData(GL_ARRAY_BUFFER, bufferLen, text.len, text.ptr);
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(struct TextVertexInfo), (void*)0);
	glVertexAttribDivisor(2, 1);
	
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(struct TextVertexInfo), (void*)sizeof(vec2));
	glVertexAttribDivisor(3, 1);
	
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 1, GL_UNSIGNED_BYTE, 1, (void*)bufferLen);
	glVertexAttribDivisor(4, 1);
	
	// Create Framebuffer
	uint framebuffer;
	
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	
	uint result = 0;
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		// panic
		printf("Could not complete framebuffer for rendering the text.\n");
		result = 1;
		goto __exit;
	}
	
	// Render text to framebuffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, font);
	
	shader_bind(shader);
	glUniform1i(uniformTexture, 0);
	glUniform2f(uniformFit, 1.0f / xRecord, 1.0f / (y + 1));
	
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, text.len);
	
	// Finish
	output->id = texture;
	output->width = width;
	output->height = height;
	
	__exit:;
	glDeleteBuffers(1, &vbo);
	glDeleteFramebuffers(1, &framebuffer);
	
	glBindFramebuffer(GL_FRAMEBUFFER, game.framebufferStack[game.framebufferStackSize-1]);
	shader_unbind();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	return result; // success
}
