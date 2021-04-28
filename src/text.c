#include "headers/text.h"
#include "headers/opengl.h"
#include "headers/shader.h"
#include "headers/global.h"

internal struct TextRenderingStuff {
	Shader shader;
	Uniform uFit;
	Uniform uTexture;
	Uniform uProj;
	mat4 proj;
	uint vao;
	uint vbo;
} textRendering;

internal f32 textVertices[] = {
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f
};

func void text_rendering_setup(void) {
	textRendering.shader = shader_load("res/text");
	if (!textRendering.shader) {
		debug_error("Could not load text shader.\n");
		return;
	}
	
	textRendering.uFit = shader_uniform(textRendering.shader, "uFit");
	textRendering.uTexture = shader_uniform(textRendering.shader, "uTexture");
	textRendering.uProj = shader_uniform(textRendering.shader, "uProjection");
	
	glm_mat4_identity(textRendering.proj);
	glm_ortho(0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, textRendering.proj);
	
	glGenBuffers(1, &textRendering.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, textRendering.vbo);
	
	glGenVertexArrays(1, &textRendering.vao);
	glBindVertexArray(textRendering.vao);
	
	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(f32) * 2, 0);
	
	// offset
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(struct TextVertexInfo), (void*)(sizeof textVertices));
	glVertexAttribDivisor(1, 1);
	
	// color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(struct TextVertexInfo), (void*)(sizeof(vec2) + sizeof(textVertices)));
	glVertexAttribDivisor(2, 1);
	
	// chars
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(struct TextVertexInfo), (void*)(sizeof(vec2) + sizeof(vec3) + sizeof(textVertices)));
	glVertexAttribDivisor(3, 1);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*
 * colorData is just a buffer that contains pairs of integers which are (count, color).
* If 'count' is zero, it will affect all the remaining characters of the text.
* 
* The buffer should end with a pair that has a 'count' of 0 or the sum of all the pairs 'count'
* should be '>= text.len'.
*
* also, colorData can be NULL. Then all the text will be white.
*/
func uint text_render_to_texture(Texture* restrict output, string text, const Texture* restrict font, const uint* restrict colorData) {
	
	glBindBuffer(GL_ARRAY_BUFFER, textRendering.vbo);
	glBindVertexArray(textRendering.vao);
	
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
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(textVertices) + bufferLen, NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof textVertices, textVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof textVertices, bufferLen, infos);
	
	// Create Framebuffer
	Framebuffer target;
	framebuffer_init(&target, (vec2u) { width, height });
	
	// Render text to framebuffer
	framebuffer_bind(&target);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, font->id);
	
	shader_bind(textRendering.shader);
	glUniform1i(textRendering.uTexture, 0);
	glUniform2f(textRendering.uFit, 1.0f / xRecord, 1.0f / (y + 1));
	glUniformMatrix4fv(textRendering.uProj, 1, false, (f32*)textRendering.proj);
	
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

func uint text_render(string text, const mat4 where, const Texture* restrict font) {
	return text_render_ext(text, where, font, NULL, ALIGNMENT_NONE);
}

func uint text_render_ext(string text, const mat4 where, const Texture* restrict font, const uint* restrict colorData, u32 flags) {
	glBindBuffer(GL_ARRAY_BUFFER, textRendering.vbo);
	glBindVertexArray(textRendering.vao);
	
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
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(textVertices) + bufferLen, NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof textVertices, textVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof textVertices, bufferLen, infos);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, font->id);
	
	mat4 end;
	glm_mat4_identity(end);
	glm_scale(end, (vec3) { width, height });
	
	vec3 offset = { 0 };
	switch (flags & ALIGNMENT_XAXIS) {
		case ALIGNMENT_LEFT: offset[0] = 0.0f; break;
		case ALIGNMENT_RIGHT: offset[0] = -1.0f; break;
		case ALIGNMENT_CENTER: offset[0] = -0.5f; break;
	}
	
	switch (flags & ALIGNMENT_YAXIS) {
		case ALIGNMENT_TOP: offset[1] = 0.0f; break;
		case ALIGNMENT_BOTTOM: offset[1] = -1.0f; break;
		case ALIGNMENT_MIDDLE: offset[1] = -0.5f; break;
	}
	
	glm_translate(end, offset);
	glm_mat4_mul(where, end, end);
	
	shader_bind(textRendering.shader);
	glUniform1i(textRendering.uTexture, 0);
	glUniform2f(textRendering.uFit, 1.0f / xRecord, 1.0f / (y + 1));
	glUniformMatrix4fv(textRendering.uProj, 1, false, (f32*)end);
	
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, text.len);
	shader_unbind();
	
	// Finish
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	stack_free(&game.frameStack, heapBuffer);
	
	return 0; // success
}

func void text_size(string text, vec2u out, const Texture* restrict font) {
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
