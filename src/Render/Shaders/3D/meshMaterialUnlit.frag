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

#define GLOBALS_SET_NUMBER 0
#define SCENE_SET_NUMBER 1
#define OBJECTS_SET_NUMBER 2
#include "./uniforms.glsl"

void main() {
    uint matIndex = material.index[fs_in.objectIndex];
    Material material = materials.pool[matIndex];

    uint diffuseIndex = material.diffuseId;
    vec4 diffuseColor = texture(textures[diffuseIndex], fs_in.texCoords);
    if (diffuseColor.a == 0.0) {
        discard;
    }

    outColor = fs_in.fragColor * diffuseColor;
}
