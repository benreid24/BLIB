#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in FS_IN {
    vec3 fragPos;
    vec4 fragColor;
    vec2 texCoords;
    flat uint objectIndex;
    mat3 TBN;
} fs_in;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outSpecular;
layout(location = 2) out vec4 outPosition;
layout(location = 3) out vec4 outNormal;

#include "3D/Helpers/constants.glsl"

void main() {
    outColor = fs_in.fragColor;
    outSpecular = vec4(fs_in.fragColor.xyz, 1.0);
    outPosition = vec4(fs_in.fragPos, float(lightingEnabled));
    outNormal.xyz = fs_in.TBN[2];
}
