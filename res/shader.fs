#version 330 core

in vec3 vColor;
in vec2 vTexcoords;

out vec4 oFragColor;

uniform sampler2D uTexture;

void main() {
	oFragColor = vec4(vColor, 1.0) * texture(uTexture, vTexcoords);
}