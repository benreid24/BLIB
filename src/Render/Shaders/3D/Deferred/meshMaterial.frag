#version 450
#extension GL_GOOGLE_include_directive : require

layout(location = 0) in FS_IN {
    vec3 fragPos;
    vec4 fragColor;
    vec2 texCoords;
    flat uint objectIndex;
    mat3 TBN;
} fs_in;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outSpecular;
layout(location = 2) out vec4 outPosition;
layout(location = 3) out vec4 outNormal;

#define GLOBALS_SET_NUMBER 0
#define SCENE_SET_NUMBER 1
#define OBJECTS_SET_NUMBER 2
#include "3D/Helpers/uniforms.glsl"

#include "3D/Helpers/constants.glsl"
#include "3D/Helpers/parallaxMap.glsl"

void main() {
    uint matIndex = material.index[fs_in.objectIndex];
    Material material = materials.pool[matIndex];
    vec3 viewDir = normalize(camera.camPos - fs_in.fragPos);

    vec2 texCoords = parallaxMap(material, viewDir, fs_in.TBN);
    if (material.heightScale > 0.0) {
        if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) {
            discard;
        }
    }

    uint diffuseIndex = material.diffuseId;
    vec4 diffuseColor = texture(textures[diffuseIndex], texCoords);
    if (diffuseColor.a == 0.0) {
        discard;
    }

    vec3 specular = texture(textures[material.specularId], texCoords).xyz;
    uint normalIndex = material.normalId;
    vec3 normal = texture(textures[normalIndex], texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    outColor = fs_in.fragColor * diffuseColor;
    outSpecular = vec4(fs_in.fragColor.xyz * specular, material.shininess);
    outPosition = vec4(fs_in.fragPos, float(lightingEnabled));
    outNormal.xyz = normalize(fs_in.TBN * normal);
}
