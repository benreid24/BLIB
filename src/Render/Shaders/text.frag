#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D textures[4096];
layout(set = 2, binding = 1) uniform tex {
    uint index;
} skin;

void main() {
    // workaround for trash unnormalized sampler support in Vulkan
    vec2 size = textureSize(textures[skin.index], 0);
    vec2 normCoords = vec2(texCoords.x / size.x, texCoords.y / size.y);

    vec4 texColor = texture(textures[skin.index], normCoords);
    if (texColor.a == 0.0) {
        discard;
    }

    outColor = fragColor * texColor;
}
