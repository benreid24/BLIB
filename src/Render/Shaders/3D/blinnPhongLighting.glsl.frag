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
    float quadradic;
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

vec4 computeLighting(readonly Lighting lighting, vec3 fragPos, vec3 normal, vec3 viewDir) {
    // TODO - compute lighting
    return vec4(1.0);
}
