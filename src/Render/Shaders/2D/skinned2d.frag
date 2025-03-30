#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;
layout(location = 2) flat in uint textureIndex;
layout(location = 3) in vec2 fragPos;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D textures[4096];
layout(set = 0, binding = 2) uniform rsettings {
    float gamma;
} settings;

void main() {
    vec4 texColor = texture(textures[textureIndex], texCoords);
    if (texColor.a == 0.0) {
        discard;
    }

    outColor = fragColor * texColor;
    outColor.rgb = pow(outColor.rgb, vec3(1.0 / settings.gamma));
}
