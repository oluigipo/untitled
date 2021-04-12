#version 330

layout (location = 0) in vec2 aPosition;

out vec2 vPosition;

uniform mat4 uMatrix;

void main() {
	vPosition = aPosition;
	
	gl_Position = uMatrix * vec4(aPosition, 0.0, 1.0);
}
