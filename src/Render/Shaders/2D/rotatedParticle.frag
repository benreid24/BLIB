#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 texCoords;
layout(location = 2) flat in uint textureIndex;
layout(location = 3) in vec2 fragPos;
layout(location = 4) in flat float rotation;

layout(location = 0) out vec4 outColor;

#define GLOBALS_SET_NUMBER 0
#include <uniforms.glsl>

struct Light {
    vec4 color; // w component is radius
    vec2 position;
};

layout(std140, set = 1, binding = 1) uniform lb {
    uint count;
    vec3 ambient;
    Light lights[512];
} lighting;

void main() {
    float s = sin(rotation);
    float c = cos(rotation);
    mat2 rotationMatrix = mat2(c, -s, s, c);
    vec2 center = vec2(0.5, 0.5);
    vec2 texCoords = rotationMatrix * (gl_PointCoord - center) + center;

    // TODO - need to account for atlasing here
    vec4 texColor = texture(textures[textureIndex], texCoords);
    if (texColor.a == 0.0) {
        discard;
    }

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

    outColor = fragColor * texColor * vec4(lightColor, 1.0);
    outColor.rgb = pow(outColor.rgb, vec3(1.0 / settings.gamma));
}
