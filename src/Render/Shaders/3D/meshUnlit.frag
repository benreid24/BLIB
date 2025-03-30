#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in FS_IN {
    vec3 fragPos;
    vec4 fragColor;
    vec2 texCoords;
    flat uint objectIndex;
    mat3 TBN;
} fs_in;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform rsettings {
    float gamma;
} settings;

void main() {
    outColor = fs_in.fragColor;
    outColor.rgb = pow(outColor.rgb, vec3(1.0 / settings.gamma));
}
