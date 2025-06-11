#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in FS_IN {
    vec3 texCoords;
    flat uint objectIndex;
} fs_in;

layout(location = 0) out vec4 outColor;

#define GLOBALS_SET_NUMBER 0
#define SCENE_SET_NUMBER 1
#define OBJECTS_SET_NUMBER 2
#include "3D/uniforms.glsl"

void main() {
    uint matIndex = material.index[fs_in.objectIndex];
    Material material = materials.pool[matIndex];
    uint diffuseIndex = material.diffuseId;
    outColor = texture(cubemaps[diffuseIndex], fs_in.texCoords);
}
