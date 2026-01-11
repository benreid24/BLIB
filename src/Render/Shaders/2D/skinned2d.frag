#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;
layout(location = 2) flat in uint textureIndex;
layout(location = 3) in vec2 fragPos;

layout(location = 0) out vec4 outColor;

#define GLOBALS_SET_NUMBER 0
#include <uniforms.glsl>

void main() {
    vec4 texColor = texture(textures[textureIndex], texCoords);
    if (texColor.a == 0.0) {
        discard;
    }

    outColor = fragColor * texColor;
    outColor.rgb = pow(outColor.rgb, vec3(1.0 / settings.gamma));
}
