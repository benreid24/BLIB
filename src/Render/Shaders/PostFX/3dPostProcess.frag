#version 450

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D inColorAttachment;

layout(set = 1, binding = 2) uniform rsettings {
    float gamma;
} settings;

void main() {
    vec4 srcColor = texture(inColorAttachment, texCoords);
    outColor = srcColor;

    // TODO - 3d post processing effects

    outColor.rgb = pow(outColor.rgb, vec3(1.0 / settings.gamma));
}
