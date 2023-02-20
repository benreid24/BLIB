#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D textures[4096];
layout(push_constant) uniform constants
{
    layout(offset = 64) uint index;
} PushConstants;

void main() {
    outColor = fragColor * texture(textures[PushConstants.index], texCoords);
}
