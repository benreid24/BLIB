#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in vec3 normal;

layout(location = 0) out VS_OUT {
    vec4 fragColor;
    vec2 texCoords;
    mat3 TBN;
} vs_out;

layout(set = 0, binding = 0) uniform cam {
    mat4 viewProj;
    vec3 camPos;
} camera;

layout(set = 1, binding = 0) readonly buffer obj {
    mat4 model[];
} object;

void main() {
	gl_Position = camera.viewProj * object.model[gl_InstanceIndex] * vec4(inPosition, 1.0);
	vs_out.fragColor = inColor;
	vs_out.texCoords = inTexCoords;
    vs_out.TBN = mat3(tangent, bitangent, normal);
}
