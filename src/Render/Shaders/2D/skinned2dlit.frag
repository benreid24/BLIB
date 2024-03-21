#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;
layout(location = 2) flat in uint textureIndex;
layout(location = 3) in vec2 fragPos;

layout(location = 0) out vec4 outColor;

struct Light {
    vec4 color; // w component is radius
    vec2 position;
};

layout(set = 0, binding = 0) uniform sampler2D textures[4096];
layout(std140, set = 1, binding = 1) uniform lb {
    uint count;
    vec3 ambient;
    Light lights[512];
} lighting;

void main() {
    vec4 texColor = texture(textures[textureIndex], texCoords);
    if (texColor.a == 0.0) {
        discard;
    }

    vec3 lightColor = lighting.ambient;
    for (uint i = 0; i < lighting.count; i += 1) {
        Light light = lighting.lights[i];

        // compute distance and skip if too far
        vec2 rawDiff = light.position - fragPos;
        float distSqrd = dot(rawDiff, rawDiff);
        float radius = light.color.w;
        if (distSqrd > radius * radius) {
            continue;
        }

        // determine strength factor and adjusted light color
        float strength = 1.0 - sqrt(distSqrd) / radius;
        vec4 color = lighting.lights[i].color * strength;
        lightColor += color.xyz;
    }

    lightColor.x = min(lightColor.x, 1.1);
    lightColor.y = min(lightColor.y, 1.1);
    lightColor.z = min(lightColor.z, 1.1);

    outColor = fragColor * texColor * vec4(lightColor, 1.0);
}
