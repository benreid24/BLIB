#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform constants
{
	mat4 transform;
	// uint index;
} PushConstants;

void main() {
	outColor = vec4(fragColor, 1.0);
}
