#version 330 core
#define MAX_PARTICLE_COUNT 128

layout (location = 0) in vec2 aPosition;

out vec2 vTexcoords;
flat out int vInstance;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uMatrices[MAX_PARTICLE_COUNT];

void main() {
	gl_Position = uProjection * uView * uMatrices[gl_InstanceID] * vec4(aPosition, 0.0, 1.0);
	vTexcoords = vec2(aPosition.x, 1.0 - aPosition.y);
	vInstance = gl_InstanceID;
}

