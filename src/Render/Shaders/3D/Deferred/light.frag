#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in FS_IN {
    flat uint lightIndex;
} fs_in;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D albedo;
layout(set = 0, binding = 1) uniform sampler2D specular;
layout(set = 0, binding = 2) uniform sampler2D positions;
layout(set = 0, binding = 3) uniform sampler2D normals;

#define SCENE_SET_NUMBER 1
#include "3D/Helpers/uniforms.glsl"
#include "3D/Helpers/blinnPhongLighting.glsl.frag"
#include "3D/Deferred/lightConstants.glsl"

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(albedo, 0));
    vec4 fragColor = texture(albedo, uv);
    vec4 specularAndShiny = texture(specular, uv);
    vec4 posAndLighting = texture(positions, uv);
    vec3 position = posAndLighting.xyz;
    bool lightingEnabled = posAndLighting.w != 0;
    vec3 normal = texture(normals, uv).xyz;
    vec3 viewDir = normalize(camera.camPos - position);
    vec3 specular = specularAndShiny.xyz;
    float shininess = specularAndShiny.w;

    if (lightingEnabled) {
        mat3 lightColors = mat3(0);
        float shadow = 1.0;
        switch (lightType) {
        case LIGHT_TYPE_SUN:
            lightColors = computeSunLight(lighting.info.sun, normal, position, viewDir, shininess);
            break;
        case LIGHT_TYPE_SPOT_SHADOW:
            shadow = computeSpotLightShadow(fs_in.lightIndex, position);
            lightColors = computeSpotLight(
                lighting.spotShadows[fs_in.lightIndex].light,
                normal,
                position,
                viewDir,
                shininess,
                shadow
            );
            break;
        case LIGHT_TYPE_SPOT:
            lightColors = computeSpotLight(
                lighting.spotLights[fs_in.lightIndex],
                normal,
                position,
                viewDir,
                shininess,
                1.0
            );
            break;
        case LIGHT_TYPE_POINT_SHADOW:
            shadow = computePointLightShadow(fs_in.lightIndex, position);
            lightColors = computePointLight(
                lighting.pointShadows[fs_in.lightIndex].light,
                normal,
                position,
                viewDir,
                shininess,
                shadow
            );
            break;
        case LIGHT_TYPE_POINT:
            lightColors = computePointLight(
                lighting.pointLights[fs_in.lightIndex],
                normal,
                position,
                viewDir,
                shininess,
                1.0
            );
            break;
        }
        vec3 litColor = synthesizeLightColor(lightColors, fragColor.xyz, specular);
        outColor = vec4(litColor, fragColor.w);
    }
    else {
        outColor = fragColor;
    }
}
