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

#define GLOBALS_SET_NUMBER 0
#define SCENE_SET_NUMBER 1
#define OBJECTS_SET_NUMBER 2
#include "3D/Helpers/uniforms.glsl"

#include "3D/Helpers/constants.glsl"
#include "3D/Helpers/blinnPhongLighting.glsl.frag"
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

    if (lightingEnabled == 1) {
        uint specularIndex = material.specularId;
        vec3 specularColor = vec3(texture(textures[specularIndex], texCoords));

        uint normalIndex = material.normalId;
        vec3 normal = texture(textures[normalIndex], texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(fs_in.TBN * normal);

        vec3 diffuse = vec3(diffuseColor);
        float ssao = texture(ssaoBuffer, gl_FragCoord.xy / textureSize(ssaoBuffer, 0)).r;
        vec3 lightColor = computeLighting(fs_in.fragPos, normal, diffuse, specularColor, material.shininess, ssao);
        diffuseColor = vec4(lightColor, diffuseColor.w);
    }

    outColor = fs_in.fragColor * diffuseColor;
}
