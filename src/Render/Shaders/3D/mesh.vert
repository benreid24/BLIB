#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inNormal;

layout(location = 0) out VS_OUT {
    vec3 fragPos;
    vec4 fragColor;
    vec2 texCoords;
    flat uint objectIndex;
    mat3 TBN;
} vs_out;

#define SCENE_SET_NUMBER 1
#define OBJECTS_SET_NUMBER 2
#include "3D/Helpers/uniforms.glsl"

void main() {
    ModelTransform model = object.model[gl_InstanceIndex];
    vec4 inPos = vec4(inPosition, 1.0);

	gl_Position = camera.projection * camera.view * model.transform * inPos;
    vs_out.fragPos = vec3(model.transform * inPos);
	vs_out.fragColor = inColor;
	vs_out.texCoords = inTexCoords;
    vs_out.objectIndex = gl_InstanceIndex;
    
    vec3 T = normalize(vec3(model.normal * inTangent));
    vec3 N = normalize(vec3(model.normal * inNormal));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(T, N);
    vs_out.TBN = mat3(T, B, N);
}
