#version 330

layout (location = 0) in vec2 aPosition;
layout (location = 1) in uint aIndex;

out vec3 vTexcoord;

uniform mat4 uMatrix;
uniform ivec2 uSize;

void main() {
	vTexcoord = vec3(aPosition.x, 1.0 - aPosition.y, aIndex);
	
	vec2 offset;
	offset.x = float(gl_InstanceID % uSize.x);
	offset.y = float(gl_InstanceID / uSize.y);
	
	gl_Position = uMatrix * vec4(aPosition + offset, 0.0, 1.0);
}

