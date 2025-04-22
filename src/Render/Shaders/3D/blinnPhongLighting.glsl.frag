#ifndef BLINNPHONGLIGHTING_INCLUDED
#define BLINNPHONGLIGHTING_INCLUDED

#include "./uniforms.glsl"

mat3 computeSunLight(Sunlight light, vec3 normal, vec3 viewDir, float shininess) {
    vec3 lightDir = normalize(vec3(-light.direction));

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = diff > 0.0 ? pow(max(dot(normal, halfwayDir), 0.0), shininess) : 0.0;

    // combine results
    vec3 ambient = vec3(light.color.ambient);
    vec3 diffuse = vec3(light.color.diffuse) * diff;
    vec3 specular = vec3(light.color.specular) * spec;
    return mat3(ambient, diffuse, specular);
}

mat3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shininess) {
    vec3 posDiff = vec3(light.position) - fragPos;
    vec3 lightDir = normalize(posDiff);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = diff > 0.0 ? pow(max(dot(normal, halfwayDir), 0.0), shininess) : 0.0;

    // attenuation
    float dist = length(posDiff);
    float attenuation = 1.0 / (
        light.attenuation.constant +
        light.attenuation.linear * dist +
        light.attenuation.quadratic * (dist * dist)
    );

    // combine results
    vec3 ambient = vec3(light.color.ambient);
    vec3 diffuse = vec3(light.color.diffuse) * diff;
    vec3 specular = vec3(light.color.specular) * spec;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return mat3(ambient, diffuse, specular);
}

mat3 computeSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shininess) {
    vec3 posDiff = vec3(light.position) - fragPos;
    vec3 lightDir = normalize(posDiff);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = diff > 0.0 ? pow(max(dot(normal, halfwayDir), 0.0), shininess) : 0.0;

    // attenuation
    float dist = length(posDiff);
    float attenuation = 1.0 / (
        light.attenuation.constant +
        light.attenuation.linear * dist +
        light.attenuation.quadratic * (dist * dist)
    );

    // spotlight intensity
    float theta = dot(lightDir, normalize(-vec3(light.direction)));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    // combine results
    // TODO - don't multiply material color here
    vec3 ambient = vec3(light.color.ambient);
    vec3 diffuse = vec3(light.color.diffuse) * diff;
    vec3 specular = vec3(light.color.specular) * spec;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return mat3(ambient, diffuse, specular);
}

#endif
