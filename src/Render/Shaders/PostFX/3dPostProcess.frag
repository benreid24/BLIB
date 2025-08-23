#version 450

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(constant_id = 0) const uint hdrEnabled = 1;
layout(constant_id = 1) const uint bloomEnabled = 1;

layout(set = 0, binding = 0) uniform sampler2D inColorAttachment;
layout(set = 1, binding = 0) uniform sampler2D inBloomAttachment;

layout(set = 2, binding = 2) uniform rsettings {
    float gamma;
    float exposure;
} settings;

void main() {
    // scene render color
    vec4 color = texture(inColorAttachment, texCoords);
    if (color.w == 0) {
        discard;
    }
    vec3 result = color.rgb;

    // bloom
    if (bloomEnabled == 1) {
        vec3 bloom = texture(inBloomAttachment, texCoords).rgb;
        result += bloom;
    }

    // hdr tone map
    if (hdrEnabled == 1) {
        result = vec3(1.0) - exp(-result * settings.exposure);
    }

    // gamma correction
    result = pow(result, vec3(1.0 / settings.gamma));

    // write output
    outColor = vec4(result, 1.0);
}
