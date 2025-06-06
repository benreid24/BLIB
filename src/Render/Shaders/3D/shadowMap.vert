#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inNormal;

#define LIGHT_CAM_SET_NUMBER 0
#define OBJECTS_SET_NUMBER 1
#include "./uniforms.glsl"

void main() {
    ModelTransform model = object.model[gl_InstanceIndex];
	gl_Position = lightCameras.viewProj[0] * model.transform * vec4(inPosition, 1.0);
}
