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
#include "3D/uniforms.glsl"
#include "3D/blinnPhongLighting.glsl.frag"

vec2 parallaxMap(Material material, vec3 viewDir);

void main() {
    uint matIndex = material.index[fs_in.objectIndex];
    Material material = materials.pool[matIndex];
    vec3 viewDir = normalize(camera.camPos - fs_in.fragPos);

    vec2 texCoords = parallaxMap(material, viewDir);
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

    uint specularIndex = material.specularId;
    vec3 specularColor = vec3(texture(textures[specularIndex], texCoords));

    uint normalIndex = material.normalId;
    vec3 normal = texture(textures[normalIndex], texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(fs_in.TBN * normal);

    vec3 diffuse = vec3(diffuseColor);
    vec3 lightColor = computeLighting(fs_in.fragPos, normal, diffuse, specularColor, material.shininess);

    outColor = fs_in.fragColor * vec4(lightColor, diffuseColor.w);
}

vec2 parallaxMap(Material material, vec3 viewDir) {
    // skip if we do not have a map
    if (material.heightScale == 0.0) {
        return fs_in.texCoords;
    }

    // map viewDir to tangent space
    mat3 invTBN = transpose(fs_in.TBN);
    viewDir = normalize(fs_in.TBN * viewDir);

    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;

    // depth of current layer
    float currentLayerDepth = 0.0;

    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * material.heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = fs_in.texCoords;
    float currentDepthMapValue = texture(textures[material.parallaxId], currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue) {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;

        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(textures[material.parallaxId], currentTexCoords).r;  

        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(textures[material.parallaxId], prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}
