#include "headers/shapes.h"
#include "headers/opengl.h"

func uint shapes_quad(void) {
	static uint vbo = 0;
	
	if (!vbo) {
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		
		f32 data[] = {
			// position
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f
		};
		
		glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
	}
	
	return vbo;
}