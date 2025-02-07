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
    uint uvId;
    uint padding;
};

struct PackedMaterials {
    Material pack[16];
};

layout(set = 0, binding = 0) uniform sampler2D textures[4096];
layout(set = 0, binding = 1) uniform mat {
    PackedMaterials pool[128];
} materials;

layout(set = 1, binding = 1) uniform block_light_info {
    LightInfo info;
} lighting;
layout(set = 1, binding = 2) uniform block_point_lights {
    PointLight lights[MAX_POINT_LIGHTS];
} pointLights;
layout(std140, set = 1, binding = 3) uniform block_spot_lights {
    SpotLight lights[MAX_SPOT_LIGHTS];
} spotLights;

layout(std140, set = 2, binding = 1) readonly buffer tex {
    uint index[];
} skin;

void main() {
    uint packIndex = fs_in.objectIndex / 16;
    uint matIndex = fs_in.objectIndex % 16;
    Material material = materials.pool[packIndex].pack[matIndex];

    uint textureIndex = material.diffuseId;
    vec4 texColor = texture(textures[textureIndex], fs_in.texCoords);
    if (texColor.a == 0.0) {
        discard;
    }

    outColor = fs_in.fragColor * texColor;
}
