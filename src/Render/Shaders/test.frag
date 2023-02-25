#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform constants
{
    uint index;
} PushConstants;

void main() {
    outColor = fragColor;
}
