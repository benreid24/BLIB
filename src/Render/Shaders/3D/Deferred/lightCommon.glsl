#ifndef LIGHTCOMMON_INCLUDED
#define LIGHTCOMMON_INCLUDED

#include "3D/Deferred/lightConstants.glsl"
#include "3D/Helpers/blinnPhongLighting.glsl.frag"

vec4 computeLightColor(
    uint lightIndex, vec4 fragColor, vec3 specular, vec3 position,
    vec3 normal, vec3 viewDir, float shininess, float ssao
) {
    mat3 lightColors = mat3(0);
    float shadow = 1.0;
    switch (lightType) {
    case LIGHT_TYPE_SUN:
        lightColors = computeSunLight(lighting.info.sun, normal, position, viewDir, shininess);
        break;
    case LIGHT_TYPE_SPOT_SHADOW:
        shadow = computeSpotLightShadow(lightIndex, position);
        lightColors = computeSpotLight(
            lighting.spotShadows[lightIndex].light,
            normal,
            position,
            viewDir,
            shininess,
            shadow
        );
        break;
    case LIGHT_TYPE_SPOT:
        lightColors = computeSpotLight(
            lighting.spotLights[lightIndex],
            normal,
            position,
            viewDir,
            shininess,
            1.0
        );
        break;
    case LIGHT_TYPE_POINT_SHADOW:
        shadow = computePointLightShadow(lightIndex, position);
        lightColors = computePointLight(
            lighting.pointShadows[lightIndex].light,
            normal,
            position,
            viewDir,
            shininess,
            shadow
        );
        break;
    case LIGHT_TYPE_POINT:
        lightColors = computePointLight(
            lighting.pointLights[lightIndex],
            normal,
            position,
            viewDir,
            shininess,
            1.0
        );
        break;
    }
    vec3 litColor = synthesizeLightColor(lightColors, fragColor.xyz, specular, ssao);
    return vec4(litColor, fragColor.w);
}

#endif
