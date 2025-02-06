#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;
layout(location = 2) flat in uint objectIndex;

layout(location = 0) out vec4 outColor;

struct Material {
    uint diffuseId;
    uint normalId;
    uint uvId;
    uint padding;
};

layout(set = 0, binding = 0) uniform sampler2D textures[4096];
layout(set = 0, binding = 1) uniform mat {
    Material pool[2048];
} materials;
layout(std140, set = 2, binding = 1) readonly buffer tex {
    uint index[];
} skin;

void main() {
    uint textureIndex = materials.pool[objectIndex].diffuseId;
    vec4 texColor = texture(textures[textureIndex], texCoords);
    if (texColor.a == 0.0) {
        discard;
    }

    outColor = fragColor * texColor;
}
