#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler;
layout(push_constant) uniform constants {
    float fadeFactor;
} PC;

void main() {
    vec4 srcColor = texture(texSampler, texCoords);
	outColor = fragColor * srcColor;
    outColor.xyz = outColor.xyz * PC.fadeFactor;
    outColor.w = 1.0;
}
