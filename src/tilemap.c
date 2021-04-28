#include "headers/all.h"

func void tilemap_render(const struct Tilemap* tilemap, const mat4 where) {
	static Shader shader;
	static Uniform uMatrix;
	static Uniform uSize;
	static Uniform uTexture;
	
	if (!shader) {
		shader = shader_load("res/tilemap");
		uMatrix = shader_uniform(shader, "uMatrix");
		uSize = shader_uniform(shader, "uSize");
		uTexture = shader_uniform(shader, "uTexture");
	}
	
	uint quadVbo = shapes_quad();
	uint dataVbo;
	uint vao;
	uint size = tilemap->size[0] * tilemap->size[1];
	
	// VBOs & VAO
	glGenBuffers(1, &dataVbo);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	// Quad
	glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(f32) * 2, (void*)0);
	
	// Indices
	glBindBuffer(GL_ARRAY_BUFFER, dataVbo);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(u16), tilemap->data, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_SHORT, sizeof(u16), (void*)0);
	glVertexAttribDivisor(1, 1);
	
	// Position
	mat4 end;
	glm_mat4_identity(end);
	glm_scale(end, (vec3) { tilemap->texture->size[0], tilemap->texture->size[1] });
	glm_mat4_mul(where, end, end);
	
	// Draw
	shader_bind(shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tilemap->texture->id);
	
	glUniformMatrix4fv(uMatrix, 1, false, (f32*)end);
	glUniform2i(uSize, tilemap->size[0], tilemap->size[1]);
	glUniform1i(uTexture, 0);
	
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, size);
	
	// Clean-up
	shader_unbind();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &dataVbo);
	glDeleteVertexArrays(1, &vao);
}
