#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inNormal;
layout(location = 5) in ivec4 boneIndices;
layout(location = 6) in vec4 boneWeights;

#define SCENE_SET_NUMBER 0
#define OBJECTS_SET_NUMBER 1
#include "3D/uniforms.glsl"

layout(push_constant) uniform PushConstants {
    float thickness;
} outline;

void main() {
    ModelTransform model = object.model[gl_InstanceIndex];
    vec4 inPos = vec4(inPosition, 1.0);
    vec3 normal = normalize(model.normal * inNormal);

    // TODO - bone info + animation + scale

	vec4 worldPos = model.transform * inPos;
    worldPos += vec4(normal * outline.thickness, 0.0);

	gl_Position = camera.projection * camera.view * worldPos;
}
