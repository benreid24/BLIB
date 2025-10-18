#ifndef UNIFORMS_3D_INCLUDED
#define UNIFORMS_3D_INCLUDED

#define MAX_POINT_LIGHTS 128
#define MAX_SPOT_LIGHTS 128
#define MAX_SPOT_SHADOWS 16
#define MAX_POINT_SHADOWS 16

#include <uniforms.glsl>

#ifdef OBJECTS_SET_NUMBER
struct ModelTransform {
    mat4 transform;
    mat3 normal;
};

layout(set = OBJECTS_SET_NUMBER, binding = 0) readonly buffer obj {
    ModelTransform model[];
} object;
layout(std430, set = OBJECTS_SET_NUMBER, binding = 1) readonly buffer tex {
    uint index[];
} material;
#endif // OBJECTS_SET_NUMBER

#ifdef SCENE_SET_NUMBER

struct Color {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
    float radius;
};

struct Sunlight {
    vec4 direction;
    Color color;
    mat4 viewProj;
};

struct PointLight {
    vec4 position;
    Attenuation attenuation;
    Color color;
};

struct PointLightCaster {
    PointLight light;
    mat4 viewProjMatrices[6];
    float farPlane;
    float nearPlane;
    vec2 padding;
};

struct SpotLight {
    vec4 position;
    float cutoff;
    vec4 direction;
    float outerCutoff;
    Attenuation attenuation;
    Color color;
};

struct SpotLightCaster {
    SpotLight light;
    mat4 viewProj;
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

    SpotLight spotLights[MAX_SPOT_LIGHTS];
    SpotLightCaster spotShadows[MAX_SPOT_SHADOWS];

    PointLight pointLights[MAX_POINT_LIGHTS];
    PointLightCaster pointShadows[MAX_POINT_SHADOWS];
} lighting;
layout(set = SCENE_SET_NUMBER, binding = 2) uniform sampler2DShadow spotShadowMaps[MAX_SPOT_SHADOWS];
layout(set = SCENE_SET_NUMBER, binding = 3) uniform samplerCubeShadow pointShadowMaps[MAX_POINT_SHADOWS];
layout(set = SCENE_SET_NUMBER, binding = 4) uniform sampler2D ssaoBuffer;
#endif // SCENE_SET_NUMBER

#ifdef LIGHT_CAM_SET_NUMBER
layout(set = LIGHT_CAM_SET_NUMBER, binding = 0) uniform block_light_cameras {
    mat4 viewProj[6];
    vec3 position;
    float farPlane;
} lightCameras;
#endif // LIGHT_CAM_SET_NUMBER

#ifdef GBUFFER_SET_NUMBER
layout(set = GBUFFER_SET_NUMBER, binding = 0) uniform sampler2D albedo;
layout(set = GBUFFER_SET_NUMBER, binding = 1) uniform sampler2D specular;
layout(set = GBUFFER_SET_NUMBER, binding = 2) uniform sampler2D positions;
layout(set = GBUFFER_SET_NUMBER, binding = 3) uniform sampler2D normals;
#endif // GBUFFER_SET_NUMBER

#ifdef GBUFFER_MSAA_SET_NUMBER
layout(set = GBUFFER_MSAA_SET_NUMBER, binding = 0) uniform sampler2DMS albedo;
layout(set = GBUFFER_MSAA_SET_NUMBER, binding = 1) uniform sampler2DMS specular;
layout(set = GBUFFER_MSAA_SET_NUMBER, binding = 2) uniform sampler2DMS positions;
layout(set = GBUFFER_MSAA_SET_NUMBER, binding = 3) uniform sampler2DMS normals;
#endif // GBUFFER_MSAA_SET_NUMBER

#ifdef SSAO_SET_NUMBER
layout(set = SSAO_SET_NUMBER, binding = 0) uniform ssao_block {
    vec4 randomRotations[4][4];
    vec4 samples[128];
    uint sampleCount;
    float radius;
    float bias;
    float exponent;
} ssaoParams;
#endif // SSAO_SET_NUMBER

#endif
