#ifndef LIGHT_CONSTANTS_INCLUDED
#define LIGHT_CONSTANTS_INCLUDED

#define LIGHT_TYPE_SUN 0
#define LIGHT_TYPE_SPOT_SHADOW 1
#define LIGHT_TYPE_SPOT 2
#define LIGHT_TYPE_POINT_SHADOW 3
#define LIGHT_TYPE_POINT 4

layout(constant_id = 0) const uint lightType = LIGHT_TYPE_SUN;

#endif
