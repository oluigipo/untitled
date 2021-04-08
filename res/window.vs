#version 330

layout (location = 0) in vec2 aPosition;

out vec2 vTexcoord;

void main() {
	gl_Position = vec4(aPosition * 2.0 - 1.0, 0.0, 1.0);
	vTexcoord = vec2(aPosition.x, aPosition.y);
}

