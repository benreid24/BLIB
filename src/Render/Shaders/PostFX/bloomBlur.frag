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
    vec2 texelSize = 1.0 / textureSize(inColorAttachment, 0);
    vec3 result = texture(inColorAttachment, texCoords).rgb * config.filterWeights[0];
    if (config.horizontal != 0)
    {
        for(int i = 1; i < config.filterSize; ++i)
        {
            result += texture(inColorAttachment, texCoords + vec2(texelSize.x * i, 0.0)).rgb * config.filterWeights[i];
            result += texture(inColorAttachment, texCoords - vec2(texelSize.x * i, 0.0)).rgb * config.filterWeights[i];
        }
    }
    else
    {
        for(int i = 1; i < config.filterSize; ++i)
        {
            result += texture(inColorAttachment, texCoords + vec2(0.0, texelSize.y * i)).rgb * config.filterWeights[i];
            result += texture(inColorAttachment, texCoords - vec2(0.0, texelSize.y * i)).rgb * config.filterWeights[i];
        }
    }
    outColor = vec4(result, 1.0);
}
