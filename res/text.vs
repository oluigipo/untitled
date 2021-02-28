#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aOffset;
layout (location = 2) in vec3 aColor;
layout (location = 3) in uint aChar;

out vec2 vTexcoord;
out vec3 vColor;
out float vChar;

uniform vec2 uFit;

void main() {
	gl_Position = vec4((aPosition + aOffset) * uFit, 0.0, 1.0);
	vTexcoord = vec2(aPosition.x, 1.0 - aPosition.y);
	vColor = aColor;
	vChar = float(aChar);
}
