#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragPos;

layout(location = 0) out vec4 outColor;

struct Light {
    vec4 color; // w component is radius
    vec2 position;
};

layout(std140, set = 0, binding = 1) uniform lb {
    uint count;
    vec3 ambient;
    Light lights[512];
} lighting;

void main() {
    vec3 lightColor = vec3(0.0);
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
    lightColor = lightColor + lighting.ambient;

    outColor = fragColor * vec4(lightColor, 1.0);
}
