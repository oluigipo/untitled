#version 330 core
#define MAX_PARTICLE_COUNT 128

in vec2 vTexcoords;
flat in int vInstance;

out vec4 oFragColor;

uniform sampler2DArray uTexture;
uniform float uSpriteIndex;
uniform vec4 uColors[MAX_PARTICLE_COUNT];

void main() {
	oFragColor = uColors[vInstance] * texture(uTexture, vec3(vTexcoords, uSpriteIndex));
}

