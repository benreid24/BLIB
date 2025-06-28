#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 inPosition;

#define OBJECTS_SET_NUMBER 1
#include "3D/uniforms.glsl"

void main() {
    ModelTransform model = object.model[gl_InstanceIndex];
	gl_Position = model.transform * vec4(inPosition, 1.0);
}
