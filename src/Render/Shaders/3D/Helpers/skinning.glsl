#ifndef SKINNING_INCLUDED
#define SKINNING_INCLUDED

#include "3D/Helpers/uniforms.glsl"

mat4 computeBoneMatrix(uvec4 indices, vec4 weights) {
    uint base = boneOffsets.boneOffsets[gl_InstanceIndex];
    mat4 transform = bonePool.bones[base + indices[0]] * weights[0];
    transform += bonePool.bones[base + indices[1]] * weights[1];
    transform += bonePool.bones[base + indices[2]] * weights[2];
    transform += bonePool.bones[base + indices[3]] * weights[3];
    return transform;
}

#endif
