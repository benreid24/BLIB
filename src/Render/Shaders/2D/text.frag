#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;
layout(location = 2) flat in uint textureIndex;
layout(location = 3) in vec2 fragPos;

layout(location = 0) out vec4 outColor;

#define GLOBALS_SET_NUMBER 0
#include <uniforms.glsl>

layout(std430, set = 2, binding = 1) readonly buffer tex {
    uint index[];
} skin;

void main() {
    // workaround for trash unnormalized sampler support in Vulkan
    vec2 size = textureSize(textures[textureIndex], 0);
    vec2 normCoords = vec2(texCoords.x / size.x, texCoords.y / size.y);

    vec4 texColor = texture(textures[textureIndex], normCoords);
    if (texColor.a == 0.0) {
        discard;
    }

    outColor = fragColor * texColor;
    outColor.rgb = pow(outColor.rgb, vec3(1.0 / settings.gamma));
}
