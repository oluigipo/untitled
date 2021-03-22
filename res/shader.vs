#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexcoords;

out vec3 vColor;
out vec2 vTexcoords;

uniform mat4 uView;
uniform mat4 uObject;

void main() {
	gl_Position = uView * uObject * vec4(aPosition, 1.0);
	vColor = aColor;
	vTexcoords = aTexcoords;
}

