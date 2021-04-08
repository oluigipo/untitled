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
	
	// Allocate vertex buffer data
	usize bufferLen = text.len * sizeof(struct TextVertexInfo);
	void* heapBuffer = stack_push(&game.frameStack, bufferLen);
	
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
	Framebuffer target;
	framebuffer_init(&target, (vec2u) { width, height });
	
	// Render text to framebuffer
	framebuffer_bind(&target);
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
	framebuffer_unbind();
	*output = framebuffer_extract(&target);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	stack_pop(&game.frameStack);
	
	return 0; // success
}

void text_size(string text, vec2u out, const struct Texture* restrict font) {
	uint x = 0, y = 0;
	uint xRecord = 0;
	
	for (uint i = 0; i < text.len; ++i) {
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
	
	out[0] = xRecord * font->size[0];
	out[1] = (y + 1) * font->size[1];
	
}
