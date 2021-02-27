#version 330 core

in vec2 vTexcoord;
in vec3 vColor;
in float vChar;

out vec4 oFragColor;

uniform sampler2DArray uTexture;

void main() {
	oFragColor = vec4(vColor, 1.0) * texture(uTexture, vec3(vTexcoord, vChar));
}