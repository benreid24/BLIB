#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec2 inUV;
layout(location = 0) out float outColor;

#define GBUFFER_SET_NUMBER 0
#define SCENE_SET_NUMBER 1
#define SSAO_SET_NUMBER 2
#include "3D/Helpers/uniforms.glsl"

void main() {
    vec3 pos = texture(positions, inUV).xyz;
    vec3 normal = texture(normals, inUV).xyz;
    vec3 noise = ssaoParams.randomRotations[uint(gl_FragCoord.x) % 4][uint(gl_FragCoord.y) % 4].xyz;
    vec3 tangent = normalize(noise - normal * dot(noise, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (uint i = 0; i < ssaoParams.sampleCount; ++i) {
        vec3 samplePos = TBN * ssaoParams.samples[i].xyz;
        samplePos = pos + samplePos * ssaoParams.radius;

        vec4 ndcOffset = vec4(samplePos, 1.0);
        ndcOffset = camera.projection * ndcOffset;
        ndcOffset.xyz /= ndcOffset.w;

        float sampleDepth = texture(positions, ndcOffset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, ssaoParams.radius / abs(pos.z - sampleDepth));
        occlusion += ((sampleDepth >= samplePos.z + ssaoParams.bias) ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / float(ssaoParams.sampleCount));
    outColor = occlusion;
}
