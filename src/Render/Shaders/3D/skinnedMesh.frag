#version 450
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

struct Material {
    uint diffuseId;
    uint normalId;
    uint specularId;
    float shininess;
};

layout(set = 0, binding = 0) uniform sampler2D textures[4096];
layout(set = 0, binding = 1) uniform mat {
    Material pool[2048];
} materials;

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

layout(std430, set = 2, binding = 1) readonly buffer tex {
    uint index[];
} skin;

void main() {
    uint matIndex = skin.index[fs_in.objectIndex];
    Material material = materials.pool[matIndex];

    uint diffuseIndex = material.diffuseId;
    vec4 diffuseColor = texture(textures[diffuseIndex], fs_in.texCoords);
    if (diffuseColor.a == 0.0) {
        discard;
    }

    uint specularIndex = material.specularId;
    vec3 specularColor = vec3(texture(textures[specularIndex], fs_in.texCoords));

    vec3 viewDir = normalize(fs_in.fragPos - camera.camPos);
    vec3 normal = fs_in.TBN[2]; // TODO - normal map

    mat3 lightColors = mat3(vec3(0.0), vec3(lighting.info.globalAmbient), vec3(0.0));
    lightColors += computeSunLight(lighting.info.sun, normal, viewDir, material.shininess);

    for (uint i = 0; i < lighting.info.nPointLights; i += 1) {
        lightColors += computePointLight(pointLights.lights[i], normal, fs_in.fragPos, viewDir, material.shininess);
    }

    for (uint i = 0; i < lighting.info.nSpotLights; i += 1) {
        lightColors += computeSpotLight(spotLights.lights[i], normal, fs_in.fragPos, viewDir, material.shininess);
    }

    vec3 diffuse = vec3(diffuseColor);
    vec3 lightColor = lightColors[0] * diffuse + lightColors[1] * diffuse + lightColors[2] * specularColor;

    outColor = fs_in.fragColor * vec4(lightColor, diffuseColor.w);
}
