#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in FS_IN {
    vec4 fragColor;
    vec2 texCoords;
    mat3 TBN;
} fs_in;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = fs_in.fragColor;
}
