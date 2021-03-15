#include "headers/text.h"
#include "headers/opengl.h"
#include "headers/shader.h"
#include "headers/global.h"

/*
 * colorData is just a buffer that contains pairs of integers which are (count, color).
* If 'count' is zero, it will affect all the remaining characters of the text.
* 
* The buffer should end with a pair that has a 'count' of 0 or the sum of all the pairs 'count'
* should be '>= text.len'.
*
* also, colorData can be NULL. Then all the text will be white.
*/
uint text_render(struct Texture* restrict output, string text, const uint* restrict colorData, const struct Texture* restrict font) {
	static Shader shader = 0;
	static Uniform uniformFit;
	static Uniform uniformTexture;
	static Uniform uniformProj;
	
	static void* heapBuffer;
	static usize heapSize;
	static mat4 proj;
	
	static uint vao;
	static uint vbo;
	
	static f32 vertices[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};
	
	// Setup
	if (!shader) {
		shader = shader_load("res/text");
		if (!shader) {
			debug_error("Could not load text shader.\n");
			return 1;
		}
		
		uniformFit = shader_uniform(shader, "uFit");
		uniformTexture = shader_uniform(shader, "uTexture");
		uniformProj = shader_uniform(shader, "uProjection");
		
		glm_mat4_identity(proj);
		glm_ortho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, proj);
		
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		
		// position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(f32) * 2, 0);
		
		// offset
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(struct TextVertexInfo), (void*)(sizeof vertices));
		glVertexAttribDivisor(1, 1);
		
		// color
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(struct TextVertexInfo), (void*)(sizeof(vec2) + sizeof(vertices)));
		glVertexAttribDivisor(2, 1);
		
		// chars
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(struct TextVertexInfo), (void*)(sizeof(vec2) + sizeof(vec3) + sizeof(vertices)));
		glVertexAttribDivisor(3, 1);
	} else {
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindVertexArray(vao);
	}
	
	uint texture;
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// Allocate vertex buffer data
	usize bufferLen = text.len * sizeof(struct TextVertexInfo);
	if (heapSize < bufferLen) {
		void* newbuf = mem_realloc(heapBuffer, bufferLen);
		if (!newbuf) {
			// panic
			debug_error("Could not allocate memory!\nExiting.\n");
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
	
	uint count = 0;
	uint color = 0xFFFFFF; // white
	
	if (colorData) {
		count = colorData[0];
		color = colorData[1];
		colorData += 2;
	}
	
	for (uint i = 0; i < text.len; ++i) {
		infos[i].offset[0] = (f32)x;
		infos[i].offset[1] = (f32)y;
		
		infos[i].color[0] = (f32)((color >> 16) & 0xFF) / 255.0f;
		infos[i].color[1] = (f32)((color >> 8) & 0xFF) / 255.0f;
		infos[i].color[2] = (f32)(color & 0xFF) / 255.0f;
		
		if (count > 0 && --count == 0) {
			count = colorData[0];
			color = colorData[1];
			colorData += 2;
		}
		
		if (text.ptr[i] == '\n') {
			++y;
			infos[i].chr = (f32)' ';
			
			if (x > xRecord)
				xRecord = x;
			
			x = 0;
			
			continue;
		}
		
		infos[i].chr = (f32)text.ptr[i];
		
		++x;
	}
	
	if (x > xRecord)
		xRecord = x;
	
	width = xRecord * font->size[0];
	height = (y + 1) * font->size[1];
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + bufferLen, NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof vertices, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof vertices, bufferLen, infos);
	
	// Create Framebuffer
	uint framebuffer;
	
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	
	uint result = 0;
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		// panic
		debug_error("Could not complete framebuffer for rendering the text.\n");
		result = 1;
		goto __exit;
	}
	
	// Render text to framebuffer
	glViewport(0.0f, 0.0f, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, font->id);
	
	shader_bind(shader);
	glUniform1i(uniformTexture, 0);
	glUniform2f(uniformFit, 1.0f / xRecord, 1.0f / (y + 1));
	glUniformMatrix4fv(uniformProj, 1, false, (f32*)proj);
	
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, text.len);
	shader_unbind();
	
	// Finish
	output->id = texture;
	output->size[0] = width;
	output->size[1] = height;
	output->depth = 0;
	
	__exit:;
	glDeleteFramebuffers(1, &framebuffer);
	
	glBindFramebuffer(GL_FRAMEBUFFER, game.framebufferStack[game.framebufferStackSize-1]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glViewport(0, 0, game.window.width, game.window.height);
	
	return result; // success
}
