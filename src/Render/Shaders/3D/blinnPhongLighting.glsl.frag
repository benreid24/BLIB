#define MAX_POINT_LIGHTS 128
#define MAX_SPOT_LIGHTS 128

struct Color {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
};

struct Sunlight {
    vec4 direction;
    Color color;
};

struct PointLight {
    vec4 position;
    Attenuation attenuation;
    Color color;
};

struct SpotLight {
    vec4 position;
    float cutoff;
    vec4 direction;
    float outerCutoff;
    Attenuation attenuation;
    Color color;
};

struct LightInfo {
    Sunlight sun;
    uint nPointLights;
    uint nSpotLights;
};

struct Lighting {
    Sunlight sun;
    uint nPointLights;
    uint nSpotLights;
};

struct ResolvedMaterial {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

vec3 computeSunLight(Sunlight light, vec3 normal, vec3 viewDir, ResolvedMaterial material) {
    vec3 lightDir = normalize(vec3(-light.direction));

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    // TODO - don't multiply material color here
    vec3 ambient = vec3(light.color.ambient) * material.diffuse;
    vec3 diffuse = vec3(light.color.diffuse) * diff * material.diffuse;
    vec3 specular = vec3(light.color.specular) * spec * material.specular;
    return (ambient + diffuse + specular);
}

vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, ResolvedMaterial material) {
    vec3 posDiff = vec3(light.position) - fragPos;
    vec3 lightDir = normalize(posDiff);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float dist = length(posDiff);
    float attenuation = 1.0 / (
        light.attenuation.constant +
        light.attenuation.linear * dist +
        light.attenuation.quadratic * (dist * dist)
    );

    // combine results
    // TODO - don't multiply material color here
    vec3 ambient = vec3(light.color.ambient) * material.diffuse;
    vec3 diffuse = vec3(light.color.diffuse) * diff * material.diffuse;
    vec3 specular = vec3(light.color.specular) * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 computeSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, ResolvedMaterial material) {
    vec3 posDiff = vec3(light.position) - fragPos;
    vec3 lightDir = normalize(posDiff);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

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
    vec3 ambient = vec3(light.color.ambient) * material.diffuse;
    vec3 diffuse = vec3(light.color.diffuse) * diff * material.diffuse;
    vec3 specular = vec3(light.color.specular) * spec * material.specular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
