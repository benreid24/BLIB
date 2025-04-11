#version 450

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D inColorAttachment;

layout (push_constant) uniform constants {
    uint horizontal;
    float threshold;
    uint filterSize;
    float filterWeights[20];
} config;

void main() {
    vec3 srcColor = texture(inColorAttachment, texCoords).rgb;
    float brightness = dot(srcColor, vec3(0.2126, 0.7152, 0.0722));
    if (brightness >= config.threshold) {
        outColor = vec4(srcColor, 1.0);
    }
    else {
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
