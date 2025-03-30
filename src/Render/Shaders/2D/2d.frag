#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform rsettings {
    float gamma;
} settings;

void main() {
    outColor = fragColor;
    outColor.rgb = pow(outColor.rgb, vec3(1.0 / settings.gamma));
}
