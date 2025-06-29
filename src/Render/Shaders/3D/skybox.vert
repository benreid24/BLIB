#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inNormal;

layout(location = 0) out VS_OUT {
    vec3 texCoords;
    flat uint objectIndex;
} vs_out;

#define SCENE_SET_NUMBER 1
#include "3D/Helpers/uniforms.glsl"

void main() {
    vs_out.texCoords = inPosition;
    vs_out.objectIndex = gl_InstanceIndex;
    mat4 view = mat4(mat3(camera.view));
    vec4 pos = camera.projection * view * vec4(inPosition, 1.0);
    gl_Position = pos.xyww;
}
