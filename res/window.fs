#version 330

in vec2 vTexcoord;

out vec4 vFragColor;

uniform sampler2D uTexture;

void main() {
	vFragColor = texture(uTexture, vTexcoord);
}