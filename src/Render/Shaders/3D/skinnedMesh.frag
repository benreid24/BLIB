#version 450
#extension GL_GOOGLE_include_directive : require

#include "./blinnPhongLighting.glsl.frag"

layout(location = 0) in FS_IN {
    vec3 fragPos;
    vec4 fragColor;
    vec2 texCoords;
    flat uint objectIndex;
    mat3 TBN;
} fs_in;

layout(location = 0) out vec4 outColor;

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

layout(set = 0, binding = 0) uniform sampler2D textures[4096];
layout(set = 0, binding = 1) uniform mat {
    Material pool[2048];
} materials;

layout(set = 1, binding = 0) uniform cam {
    mat4 viewProj;
    vec3 camPos;
} camera;
layout(set = 1, binding = 1) uniform block_light_info {
    LightInfo info;
} lighting;
layout(set = 1, binding = 2) uniform block_point_lights {
    PointLight lights[MAX_POINT_LIGHTS];
} pointLights;
layout(std140, set = 1, binding = 3) uniform block_spot_lights {
    SpotLight lights[MAX_SPOT_LIGHTS];
} spotLights;

layout(std430, set = 2, binding = 1) readonly buffer tex {
    uint index[];
} material;

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

    mat3 lightColors = mat3(vec3(0.0), vec3(lighting.info.globalAmbient), vec3(0.0));
    lightColors += computeSunLight(lighting.info.sun, normal, viewDir, material.shininess);

    for (uint i = 0; i < lighting.info.nPointLights; i += 1) {
        lightColors += computePointLight(pointLights.lights[i], normal, fs_in.fragPos, viewDir, material.shininess);
    }

    for (uint i = 0; i < lighting.info.nSpotLights; i += 1) {
        lightColors += computeSpotLight(spotLights.lights[i], normal, fs_in.fragPos, viewDir, material.shininess);
    }

    vec3 diffuse = vec3(diffuseColor);
    vec3 lightColor = lightColors[0] * diffuse + lightColors[1] * diffuse + lightColors[2] * specularColor;

    outColor = fs_in.fragColor * vec4(lightColor, diffuseColor.w);
}

vec2 parallaxMap(Material material, vec3 viewDir) {
    // skip if we do not have a map
    if (material.heightScale == 0.0) {
        return fs_in.texCoords;
    }

    // map viewDir to tangent space
    mat3 invTBN = transpose(fs_in.TBN);
    viewDir = viewDir * invTBN;

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
