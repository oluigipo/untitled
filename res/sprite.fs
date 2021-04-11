#version 330

in vec2 vTexcoord;
in vec4 vColor;

out vec4 oFragColor;

uniform sampler2D uTexture;

void main() {
	oFragColor = vColor * texture(uTexture, vTexcoord);
}
