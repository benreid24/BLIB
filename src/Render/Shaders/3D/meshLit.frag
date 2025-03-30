#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : require

#include "./blinnPhongLighting.glsl.frag"

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
layout(set = 1, binding = 0) uniform cam {
    mat4 viewProj;
    vec3 camPos;
} camera;
layout(set = 1, binding = 1) uniform block_light_info {
    LightInfo info;
} lighting;
layout(set = 1, binding = 2) uniform block_point_lights {
    PointLight lights[MAX_POINT_LIGHTS];
} pointLights;
layout(std140, set = 1, binding = 3) uniform block_spot_lights {
    SpotLight lights[MAX_SPOT_LIGHTS];
} spotLights;

void main() {
    vec3 viewDir = normalize(camera.camPos - fs_in.fragPos);
    vec3 normal = fs_in.TBN[2];

    mat3 lightColors = mat3(vec3(0.0), vec3(lighting.info.globalAmbient), vec3(0.0));
    lightColors += computeSunLight(lighting.info.sun, normal, viewDir, 1.0);

    for (uint i = 0; i < lighting.info.nPointLights; i += 1) {
        lightColors += computePointLight(pointLights.lights[i], normal, fs_in.fragPos, viewDir, 1.0);
    }

    for (uint i = 0; i < lighting.info.nSpotLights; i += 1) {
        lightColors += computeSpotLight(spotLights.lights[i], normal, fs_in.fragPos, viewDir, 1.0);
    }

    vec3 diffuse = vec3(fs_in.fragColor);
    vec3 lightColor = lightColors[0] * diffuse + lightColors[1] * diffuse + lightColors[2] * diffuse;

    outColor = fs_in.fragColor * vec4(lightColor, fs_in.fragColor.w);
    outColor.rgb = pow(outColor.rgb, vec3(1.0 / settings.gamma));
}
