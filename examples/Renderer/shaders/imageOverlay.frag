#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler;

void main() {
	outColor = fragColor * texture(texSampler, texCoords);
}
