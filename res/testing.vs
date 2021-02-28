#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

uniform mat4 uProj;

void main() {
	gl_Position = uProj * vec4(aPos, 1.0);
	vColor = aColor;
}

