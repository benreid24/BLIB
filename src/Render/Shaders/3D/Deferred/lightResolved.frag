#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in FS_IN {
    flat uint lightIndex;
} fs_in;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2DMS albedo;
layout(set = 0, binding = 1) uniform sampler2DMS specular;
layout(set = 0, binding = 2) uniform sampler2DMS positions;
layout(set = 0, binding = 3) uniform sampler2DMS normals;

#define SCENE_SET_NUMBER 1
#include "3D/Helpers/uniforms.glsl"
#include "3D/Helpers/blinnPhongLighting.glsl.frag"
#include "3D/Deferred/lightConstants.glsl"
#include "3D/Deferred/lightCommon.glsl"

void main() {
    float ssao = texture(ssaoBuffer, gl_FragCoord.xy / textureSize(ssaoBuffer, 0)).r;
    vec4 colorSum = vec4(0.0);
    for (int si = 0; si < sampleCount; ++si) {
        ivec2 uv = ivec2(gl_FragCoord.xy);
        vec4 fragColor = texelFetch(albedo, uv, si);
        vec4 specularAndShiny = texelFetch(specular, uv, si);
        vec4 posAndLighting = texelFetch(positions, uv, si);
        vec3 position = posAndLighting.xyz;
        bool lightingEnabled = posAndLighting.w != 0;
        vec3 normal = texelFetch(normals, uv, si).xyz;
        vec3 viewDir = normalize(camera.camPos - position);
        vec3 specular = specularAndShiny.xyz;
        float shininess = specularAndShiny.w;

        if (lightingEnabled) {
            colorSum += computeLightColor(
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
            colorSum += fragColor;
        }
    }
    outColor = colorSum / float(sampleCount);
}
