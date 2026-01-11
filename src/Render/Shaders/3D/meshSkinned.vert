#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inNormal;
layout(location = 5) in uvec4 boneIndices;
layout(location = 6) in vec4 boneWeights;

layout(location = 0) out VS_OUT {
    vec3 fragPos;
    vec4 fragColor;
    vec2 texCoords;
    flat uint objectIndex;
    mat3 TBN;
} vs_out;

#define SCENE_SET_NUMBER 1
#define OBJECTS_SKINNED_SET_NUMBER 2
#include "3D/Helpers/uniforms.glsl"
#include "3D/Helpers/skinning.glsl"

void main() {
    mat4 boneTransform = computeBoneMatrix(boneIndices, boneWeights);
    ModelTransform model = object.model[gl_InstanceIndex];

    vec4 inPos = vec4(inPosition, 1.0);
    vec4 outPos = model.transform * boneTransform * inPos;

	gl_Position = camera.projection * camera.view * outPos;
    vs_out.fragPos = vec3(outPos);
	vs_out.fragColor = inColor;
	vs_out.texCoords = inTexCoords;
    vs_out.objectIndex = gl_InstanceIndex;
    
    mat3 boneNormal = mat3(transpose(inverse(boneTransform)));
    vec3 T = normalize(vec3(mat3(model.transform) * mat3(boneTransform) * inTangent));
    vec3 N = normalize(vec3(model.normal * boneNormal * inNormal));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(T, N);
    vs_out.TBN = mat3(T, B, N);
}
