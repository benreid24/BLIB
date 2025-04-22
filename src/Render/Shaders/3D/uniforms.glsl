#ifndef UNIFORMS_INCLUDED
#define UNIFORMS_INCLUDED

struct Material {
    uint diffuseId;
    uint normalId;
    uint specularId;
    uint parallaxId;
    float shininess;
    float heightScale;
    float padding0;
    float padding1;
};

#ifdef GLOBALS_SET_NUMBER
layout(set = GLOBALS_SET_NUMBER, binding = 0) uniform sampler2D textures[4096];
layout(set = GLOBALS_SET_NUMBER, binding = 1) uniform mat {
    Material pool[2048];
} materials;
layout(set = GLOBALS_SET_NUMBER, binding = 2) uniform rsettings {
    float gamma;
} settings;
layout(set = GLOBALS_SET_NUMBER, binding = 3) uniform samplerCube cubemaps[16];
#endif

#ifdef OBJECTS_SET_NUMBER
struct ModelTransform {
    mat4 transform;
    mat3 normal;
};

layout(std430, set = OBJECTS_SET_NUMBER, binding = 1) readonly buffer tex {
    uint index[];
} material;
layout(set = OBJECTS_SET_NUMBER, binding = 0) readonly buffer obj {
    ModelTransform model[];
} object;
#endif

#ifdef SCENE_SET_NUMBER

#define MAX_POINT_LIGHTS 128
#define MAX_SPOT_LIGHTS 128
#define MAX_SPOT_SHADOWS 16
#define MAX_POINT_SHADOWS 16

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
    vec4 globalAmbient;
    uint nPointLights;
    uint nSpotLights;
    uint nPointShadows;
    uint nSpotShadows;
};

layout(set = SCENE_SET_NUMBER, binding = 0) uniform cam {
    mat4 projection;
    mat4 view;
    vec3 camPos;
} camera;
layout(set = SCENE_SET_NUMBER, binding = 1) uniform block_light_info {
    LightInfo info;
} lighting;
layout(set = SCENE_SET_NUMBER, binding = 2) uniform block_point_lights {
    PointLight lights[MAX_POINT_LIGHTS];
} pointLights;
layout(std140, set = SCENE_SET_NUMBER, binding = 3) uniform block_spot_lights {
    SpotLight lights[MAX_SPOT_LIGHTS];
} spotLights;
layout(set = SCENE_SET_NUMBER, binding = 4) uniform sampler2D spotShadowMaps[MAX_SPOT_SHADOWS];
layout(set = SCENE_SET_NUMBER, binding = 5) uniform samplerCube pointShadowMaps[MAX_POINT_SHADOWS];
#endif

#endif
