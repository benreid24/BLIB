#ifndef BLIB_TYPES_INCLUDED
#define BLIB_TYPES_INCLUDED

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

#endif
