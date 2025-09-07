#ifndef AUTO_EXPOSURE_LAYOUT
#define AUTO_EXPOSURE_LAYOUT

#ifdef ACCUMULATE
#define WORK_BUFFER_TYPE buffer
#else
#define GLOBALS_WRITABLE_SETTINGS
#define WORK_BUFFER_TYPE readonly buffer
#endif

#define GLOBALS_SET_NUMBER 0
#include "3D/Helpers/uniforms.glsl"

layout(set = 1, binding = 0) uniform sampler2D renderedScene;

layout(set = 2, binding = 0) WORK_BUFFER_TYPE WorkBuffer {
    float totalLuminosity;
};

#endif
