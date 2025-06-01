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
    vec3 ambient = vec3(light.color.ambient);
    vec3 diffuse = vec3(light.color.diffuse) * diff;
    vec3 specular = vec3(light.color.specular) * spec;
    ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return mat3(ambient, diffuse, specular);
}

// float computeSpotLightShadow(SpotLight light, vec4 fragPos) {
//     //
// }

vec3 computeLighting(vec3 fragPos, vec3 normal, vec3 diffuse, vec3 specular, float shininess) {
    vec3 viewDir = normalize(camera.camPos - fragPos);

    mat3 lightColors = mat3(vec3(0.0), vec3(lighting.info.globalAmbient), vec3(0.0));
    lightColors += computeSunLight(lighting.info.sun, normal, viewDir, shininess);

    for (uint i = 0; i < lighting.info.nPointShadows; i += 1) {
        // TODO - shadowing
        lightColors += computePointLight(lighting.pointShadows[i].light, normal, fragPos, viewDir, shininess);
    }
    for (uint i = 0; i < lighting.info.nPointLights; i += 1) {
        lightColors += computePointLight(lighting.pointLights[i], normal, fragPos, viewDir, shininess);
    }

    for (uint i = 0; i < lighting.info.nSpotShadows; i += 1) {
        // TODO - shadowing
        lightColors += computeSpotLight(lighting.spotShadows[i].light, normal, fragPos, viewDir, shininess);
    }
    for (uint i = 0; i < lighting.info.nSpotLights; i += 1) {
        lightColors += computeSpotLight(lighting.spotLights[i], normal, fragPos, viewDir, shininess);
    }

    return lightColors[0] * diffuse + lightColors[1] * diffuse + lightColors[2] * specular;
}

#endif
