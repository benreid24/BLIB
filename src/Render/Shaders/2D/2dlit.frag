#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform tex {
    uint index;
} skin;

void main() {
    outColor = fragColor;
}