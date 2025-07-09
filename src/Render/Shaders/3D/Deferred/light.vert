#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inNormal;

layout(location = 0) out VS_OUT {
    vec2 texCoords;
    flat uint lightIndex;
} vs_out;

#define SCENE_SET_NUMBER 1
#include "3D/Helpers/uniforms.glsl"
#include "3D/Deferred/lightConstants.glsl"

void main() {
    vec3 position = inPosition;
    if (lightType != LIGHT_TYPE_SUN) {
        float radius = 1.0;
        vec3 lightPos = vec3(0.0);
        switch (lightType) {
        case LIGHT_TYPE_SPOT:
            lightPos = lighting.spotLights[gl_InstanceIndex].position.xyz;
            radius = lighting.spotLights[gl_InstanceIndex].attenuation.radius;
            break;
        case LIGHT_TYPE_SPOT_SHADOW:
            lightPos = lighting.spotShadows[gl_InstanceIndex].light.position.xyz;
            radius = lighting.spotShadows[gl_InstanceIndex].light.attenuation.radius;
            break;
        case LIGHT_TYPE_POINT:
            lightPos = lighting.pointLights[gl_InstanceIndex].position.xyz;
            radius = lighting.pointLights[gl_InstanceIndex].attenuation.radius;
            break;
        case LIGHT_TYPE_POINT_SHADOW:
            lightPos = lighting.pointShadows[gl_InstanceIndex].light.position.xyz;
            radius = lighting.pointShadows[gl_InstanceIndex].light.attenuation.radius;
            break;
        }
        position = position * radius + lightPos;
    }

    gl_Position = camera.projection * camera.view * vec4(position, 1.0);
    vs_out.lightIndex = gl_InstanceIndex;
    vs_out.texCoords = (gl_Position.xy + vec2(1.0)) * 0.5; // TODO - is this right?
}
