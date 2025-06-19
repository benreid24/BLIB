#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec4 fragPos;

#define LIGHT_CAM_SET_NUMBER 0
#include "3D/uniforms.glsl"

void main() {
    float lightDistance = length(fragPos.xyz - lightCameras.position);
    gl_FragDepth = lightDistance / lightCameras.farPlane;
}
