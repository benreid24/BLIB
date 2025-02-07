#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;
layout(location = 5) in vec3 inNormal;

layout(location = 0) out VS_OUTPUT {
    vec3 fragPos;
    vec4 fragColor;
    vec2 texCoords;
    flat uint objectIndex;
    mat3 TBN;
} vs_out;

layout(set = 1, binding = 0) uniform cam {
    mat4 viewProj;
    vec3 camPos;
} camera;

layout(std140, set = 2, binding = 0) readonly buffer obj {
    mat4 model[];
} object;
layout(std140, set = 2, binding = 1) readonly buffer tex {
    uint index[];
} skin;

void main() {
    mat4 model = object.model[gl_InstanceIndex];
    vec4 inPos = vec4(inPosition, 1.0);

	gl_Position = camera.viewProj * model * inPos;
    vs_out.fragPos = vec3(model * inPos);
	vs_out.fragColor = inColor;
	vs_out.texCoords = inTexCoords;
    vs_out.objectIndex = gl_InstanceIndex;

    mat3 normal = transpose(inverse(mat3(model))); // TODO - compute on cpu?
    vec3 T = normalize(vec3(normal * inTangent));
    vec3 N = normalize(vec3(normal * inNormal));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    vs_out.TBN = mat3(T, B, N);
}
