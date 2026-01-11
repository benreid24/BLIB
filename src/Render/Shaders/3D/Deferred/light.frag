#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in FS_IN {
    flat uint lightIndex;
} fs_in;
layout(location = 0) out vec4 outColor;

#define GBUFFER_SET_NUMBER 0
#define SCENE_SET_NUMBER 1
#include "3D/Helpers/uniforms.glsl"
#include "3D/Helpers/blinnPhongLighting.glsl.frag"
#include "3D/Deferred/lightConstants.glsl"
#include "3D/Deferred/lightCommon.glsl"

void main() {
    vec2 uv = gl_FragCoord.xy / textureSize(albedo, 0);
    vec4 fragColor = texture(albedo, uv);
    vec4 specularAndShiny = texture(specular, uv);
    vec4 posAndLighting = texture(positions, uv);
    vec3 position = posAndLighting.xyz;
    bool lightingEnabled = posAndLighting.w != 0;
    vec3 normal = texture(normals, uv).xyz;
    vec3 viewDir = normalize(camera.camPos - position);
    vec3 specular = specularAndShiny.xyz;
    float shininess = specularAndShiny.w;
    float ssao = texture(ssaoBuffer, uv).r;

    if (lightingEnabled) {
        outColor = computeLightColor(
            fs_in.lightIndex,
            fragColor,
            specular,
            position,
            normal,
            viewDir,
            shininess,
            ssao
        );
    }
    else {
        outColor = fragColor;
    }
}
