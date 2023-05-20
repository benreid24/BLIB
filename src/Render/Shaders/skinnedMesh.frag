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
    vec4 texColor = texture(textures[skin.index], texCoords);
    if (texColor.a == 0.0) {
        discard;
    }

    outColor = fragColor * texColor;
}
