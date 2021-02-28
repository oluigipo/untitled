#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aOffset;
layout (location = 2) in vec3 aColor;
layout (location = 3) in float aChar;

out vec2 vTexcoord;
out vec3 vColor;
out float vChar;

uniform mat4 uFit;
uniform mat4 uProjection;

void main() {
	gl_Position = uProjection * uFit * vec4(aPosition + aOffset, 0.0, 1.0);
	vTexcoord = vec2(aPosition.x, 1.0 - aPosition.y);
	vColor = aColor;
	vChar = float(aChar);
}
