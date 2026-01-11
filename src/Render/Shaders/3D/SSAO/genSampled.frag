#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec2 inUV;
layout(location = 0) out float outColor;

#define GBUFFER_MSAA_SET_NUMBER 0
#define SCENE_SET_NUMBER 1
#define SSAO_SET_NUMBER 2
#include "3D/Helpers/uniforms.glsl"

void main() {
    vec2 fbSize = textureSize(positions);
    ivec2 uv = ivec2(inUV * fbSize);
    vec3 pos = texelFetch(positions, uv, gl_SampleID).xyz;
    vec3 normal = texelFetch(normals, uv, gl_SampleID).xyz;
    vec3 noise = ssaoParams.randomRotations[uint(gl_FragCoord.x) % 4][uint(gl_FragCoord.y) % 4].xyz;
    vec3 viewNormal = normalize((camera.view * vec4(normal, 0.0)).xyz);
    vec3 viewNoise = normalize((camera.view * vec4(noise, 0.0)).xyz);
    vec3 tangent = normalize(viewNoise - viewNormal * dot(viewNoise, viewNormal));
    vec3 bitangent = cross(viewNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, viewNormal);
    vec3 posView = vec3(camera.view * vec4(pos, 1.0));

    float occlusion = 0.0;
    for (uint i = 0; i < ssaoParams.sampleCount; ++i) {
        vec3 sampleOffset = TBN * ssaoParams.samples[i].xyz;
        vec3 samplePosView = posView + sampleOffset * ssaoParams.radius;

        vec4 ncdSamplePos = camera.projection * vec4(samplePosView, 1.0);
        ncdSamplePos.xyz /= ncdSamplePos.w;
        vec2 sampleUv = ncdSamplePos.xy * 0.5 + 0.5;

        // skip out of bounds
        if (any(lessThan(sampleUv, vec2(0.0))) || any(greaterThan(sampleUv, vec2(1.0)))) {
            continue;
        }

        vec4 checkPos = vec4(texelFetch(positions, ivec2(sampleUv * fbSize), gl_SampleID).xyz, 1.0);
        checkPos = camera.view * checkPos;
        float localOcclusion = (checkPos.z >= samplePosView.z + ssaoParams.bias) ? 1.0 : 0.0;
        float rangeCheck = smoothstep(0.0, 1.0, ssaoParams.radius / abs(posView.z - checkPos.z));
        occlusion += localOcclusion * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / float(ssaoParams.sampleCount));
    occlusion = pow(occlusion, ssaoParams.exponent);
    outColor = occlusion;
}
