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
#include "./uniforms.glsl"
#include "./blinnPhongLighting.glsl.frag"

void main() {
    vec3 viewDir = normalize(camera.camPos - fs_in.fragPos);
    vec3 normal = fs_in.TBN[2];

    mat3 lightColors = mat3(vec3(0.0), vec3(lighting.info.globalAmbient), vec3(0.0));
    lightColors += computeSunLight(lighting.info.sun, normal, viewDir, 1.0);

    for (uint i = 0; i < lighting.info.nPointShadows; i += 1) {
        // TODO - shadowing
        lightColors += computePointLight(pointLights.lights[i], normal, fs_in.fragPos, viewDir, 1.0);
    }
    for (uint i = MAX_POINT_SHADOWS; i < MAX_POINT_SHADOWS + lighting.info.nPointLights; i += 1) {
        lightColors += computePointLight(pointLights.lights[i], normal, fs_in.fragPos, viewDir, 1.0);
    }

    for (uint i = 0; i < lighting.info.nSpotShadows; i += 1) {
        // TODO - shadowing
        lightColors += computeSpotLight(spotLights.lights[i], normal, fs_in.fragPos, viewDir, 1.0);
    }
    for (uint i = MAX_SPOT_SHADOWS; i < MAX_SPOT_SHADOWS + lighting.info.nSpotLights; i += 1) {
        lightColors += computeSpotLight(spotLights.lights[i], normal, fs_in.fragPos, viewDir, 1.0);
    }

    vec3 diffuse = vec3(fs_in.fragColor);
    vec3 lightColor = lightColors[0] * diffuse + lightColors[1] * diffuse + lightColors[2] * diffuse;

    outColor = fs_in.fragColor * vec4(lightColor, fs_in.fragColor.w);
}
