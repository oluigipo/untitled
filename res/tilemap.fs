#version 330

in vec3 vTexcoord;

out vec4 oFragColor;

uniform sampler2DArray uTexture;

void main() {
	oFragColor = texture(uTexture, vTexcoord);
}

