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
struct HdrSettings {
    float targetBrightness;
    float minExposure;
    float maxExposure;
    float convergeRate;
};

layout(set = GLOBALS_SET_NUMBER, binding = 0) uniform sampler2D textures[4096];
layout(set = GLOBALS_SET_NUMBER, binding = 1) uniform mat {
    Material pool[2048];
} materials;
layout(set = GLOBALS_SET_NUMBER, binding = 2) uniform samplerCube cubemaps[16];
layout(set = GLOBALS_SET_NUMBER, binding = 3) uniform framedata {
    float dt;
    float realDt;
    float residual;
    float realResidual;
    float frameDt;
    float realFrameDt;
} frameData;
layout(set = GLOBALS_SET_NUMBER, binding = 4) uniform rsettings {
    HdrSettings hdrSettings;
    float gamma;
} settings;
#ifndef GLOBALS_WRITABLE_SETTINGS
layout(set = GLOBALS_SET_NUMBER, binding = 5) readonly buffer drsettings {
    float currentHdrExposure;
} dynamicSettings;
#else
layout(set = GLOBALS_SET_NUMBER, binding = 5) buffer drsettings {
    float currentHdrExposure;
};
#endif
#endif // GLOBALS_SET_NUMBER

#endif
