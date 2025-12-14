#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inNormal;
layout(location = 5) in ivec4 boneIndices;
layout(location = 6) in vec4 boneWeights;

#define LIGHT_CAM_SET_NUMBER 0
#define OBJECTS_SKINNED_SET_NUMBER 1
#include "3D/Helpers/uniforms.glsl"
#include "3D/Helpers/skinning.glsl"

void main() {
    mat4 boneTransform = computeBoneMatrix(boneIndices, boneWeights);
    ModelTransform model = object.model[gl_InstanceIndex];
	gl_Position = lightCameras.viewProj[0] * model.transform * boneTransform * vec4(inPosition, 1.0);
}
