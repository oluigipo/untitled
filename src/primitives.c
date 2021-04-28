#include "headers/all.h"

internal struct PrimitiveRenderingStuff {
	u32 rectangleVAO;
	
	Shader roundrectShader;
	Uniform roundrectuMatrix;
	Uniform roundrectuRoundness;
	Uniform roundrectuColor;
	Uniform roundrectuScale;
} primitiveRendering;

func void primitive_rendering_init(void) {
	Shader shader;
	
	shader = shader_load("res/roundrect");
	primitiveRendering.roundrectShader = shader;
	primitiveRendering.roundrectuMatrix = shader_uniform(shader, "uMatrix");
	primitiveRendering.roundrectuRoundness = shader_uniform(shader, "uRoundness");
	primitiveRendering.roundrectuColor = shader_uniform(shader, "uColor");
	primitiveRendering.roundrectuScale = shader_uniform(shader, "uScale");
	
	u32 rectVBO = shapes_quad();
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	
	u32 rectVAO;
	glGenVertexArrays(1, &rectVAO);
	glBindVertexArray(rectVAO);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(f32) * 2, (void*)0);
	
	primitiveRendering.rectangleVAO = rectVAO;
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

func void primitive_render_rect(const mat4 where, u32 color) {
	primitive_render_roundrect(where, color, 0.0f, (vec2) { 1.0f, 1.0f });
}

func void primitive_render_roundrect(const mat4 where, u32 color, f32 roundness, vec2 scale) {
	vec4 colorvec;
	argb_to_color(color, colorvec);
	
	glBindVertexArray(primitiveRendering.rectangleVAO);
	
	shader_bind(primitiveRendering.roundrectShader);
	glUniformMatrix4fv(primitiveRendering.roundrectuMatrix, 1, false, (f32*)where);
	glUniform1f(primitiveRendering.roundrectuRoundness, roundness);
	glUniform4fv(primitiveRendering.roundrectuColor, 1, colorvec);
	glUniform2fv(primitiveRendering.roundrectuScale, 1, scale);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	shader_unbind();
	glBindVertexArray(0);
}

func void primitive_render_ellipse(const mat4 where, u32 color) {
	
}

