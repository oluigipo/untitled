#version 330

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexcoord;
layout (location = 2) in vec2 aSpriteSize;
layout (location = 3) in vec4 aColor;
layout (location = 4) in mat4 aMatrix;

out vec4 vColor;
out vec2 vTexcoord;

void main() {
	gl_Position = aMatrix * vec4(aPosition, 0.0, 1.0);
	vTexcoord = aTexcoord - aSpriteSize * vec2(aPosition.x, aPosition.y);
	vColor = aColor;
}
