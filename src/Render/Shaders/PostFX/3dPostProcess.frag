#version 450

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D inColorAttachment;
layout(set = 1, binding = 0) uniform sampler2D inBloomAttachment;

layout(set = 2, binding = 2) uniform rsettings {
    float gamma;
    float exposure;
} settings;

void main() {
    // scene render color
    vec3 result = texture(inColorAttachment, texCoords).rgb;

    // bloom
    vec3 bloom = texture(inBloomAttachment, texCoords).rgb;
    result += bloom;

    // hdr tone map
    result = vec3(1.0) - exp(-result * settings.exposure);

    // gamma correction
    result = pow(result, vec3(1.0 / settings.gamma));

    // write output
    outColor = vec4(result, 1.0);
}
