#version 450
#extension GL_GOOGLE_include_directive : require

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
    uint parallaxId;
    float shininess;
    float heightScale;
    float padding0;
    float padding1;
};

layout(set = 0, binding = 0) uniform sampler2D textures[4096];
layout(set = 0, binding = 2) uniform rsettings {
    float gamma;
} settings;
layout(set = 0, binding = 1) uniform mat {
    Material pool[2048];
} materials;

layout(set = 1, binding = 0) uniform cam {
    mat4 viewProj;
    vec3 camPos;
} camera;

layout(std430, set = 2, binding = 1) readonly buffer tex {
    uint index[];
} material;

void main() {
    uint matIndex = material.index[fs_in.objectIndex];
    Material material = materials.pool[matIndex];

    uint diffuseIndex = material.diffuseId;
    vec4 diffuseColor = texture(textures[diffuseIndex], fs_in.texCoords);
    if (diffuseColor.a == 0.0) {
        discard;
    }

    outColor = fs_in.fragColor * diffuseColor;
    outColor.rgb = pow(outColor.rgb, vec3(1.0 / settings.gamma));
}
