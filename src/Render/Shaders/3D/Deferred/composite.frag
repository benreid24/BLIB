#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec2 inTexCoords;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D albedo;
layout(set = 0, binding = 1) uniform sampler2D specular;
layout(set = 0, binding = 2) uniform sampler2D positions;
layout(set = 0, binding = 3) uniform sampler2D normals;

#define SCENE_SET_NUMBER 1
#include "3D/Helpers/uniforms.glsl"
#include "3D/Helpers/blinnPhongLighting.glsl.frag"

void main() {
    vec4 fragColor = texture(albedo, inTexCoords);
    vec4 specularAndShiny = texture(specular, inTexCoords);
    vec4 posAndLighting = texture(positions, inTexCoords);
    vec3 position = posAndLighting.xyz;
    bool lightingEnabled = posAndLighting.w != 0;
    vec3 normal = texture(normals, inTexCoords).xyz;

    if (lightingEnabled) {
        vec3 litColor = computeLighting(position, normal, fragColor.xyz, specularAndShiny.xyz, specularAndShiny.w);
        outColor = vec4(litColor, fragColor.w);
    }
    else {
        outColor = fragColor;
    }
}
