#version 450
#extension GL_GOOGLE_include_directive : require
#extension GL_ARB_sample_shading : require

layout(location = 0) in FS_IN {
    flat uint lightIndex;
} fs_in;
layout(location = 0) out vec4 outColor;

#define GBUFFER_MSAA_SET_NUMBER 0
#define SCENE_SET_NUMBER 1
#include "3D/Helpers/uniforms.glsl"
#include "3D/Helpers/blinnPhongLighting.glsl.frag"
#include "3D/Deferred/lightConstants.glsl"
#include "3D/Deferred/lightCommon.glsl"

void main() {
    ivec2 uv = ivec2(gl_FragCoord.xy);
    vec4 fragColor = texelFetch(albedo, uv, gl_SampleID);
    vec4 specularAndShiny = texelFetch(specular, uv, gl_SampleID);
    vec4 posAndLighting = texelFetch(positions, uv, gl_SampleID);
    vec3 position = posAndLighting.xyz;
    bool lightingEnabled = posAndLighting.w != 0;
    vec3 normal = texelFetch(normals, uv, gl_SampleID).xyz;
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
