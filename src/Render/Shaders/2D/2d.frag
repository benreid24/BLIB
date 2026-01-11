#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

#define GLOBALS_SET_NUMBER 0
#include <uniforms.glsl>

void main() {
    outColor = fragColor;
    outColor.rgb = pow(outColor.rgb, vec3(1.0 / settings.gamma));
}
