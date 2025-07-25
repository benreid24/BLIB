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

#define GLOBALS_SET_NUMBER 0
#define SCENE_SET_NUMBER 1
#include "3D/Helpers/uniforms.glsl"

#include "3D/Helpers/constants.glsl"
#include "3D/Helpers/blinnPhongLighting.glsl.frag"

void main() {
    if (lightingEnabled == 1) {
        vec3 normal = fs_in.TBN[2];
        vec3 diffuse = fs_in.fragColor.xyz;
        float ssao = texture(ssaoBuffer, gl_FragCoord.xy / textureSize(ssaoBuffer, 0)).r;
        vec3 lightColor = computeLighting(fs_in.fragPos, normal, diffuse, diffuse, 1.0, ssao);
        outColor = fs_in.fragColor * vec4(lightColor, 1.0);
    }
    else {
        outColor = fs_in.fragColor;
    }
}
