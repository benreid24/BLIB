#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragPos;

layout(set = 1, binding = 0) uniform cam {
    mat4 projection;
    mat4 view;
    vec3 camPos;
} camera;

layout(std430, set = 2, binding = 0) readonly buffer obj {
    mat4 model[];
} object;

void main() {
    vec4 worldPos = object.model[gl_InstanceIndex] * vec4(inPosition, 1.0);
	gl_Position = camera.projection * camera.view * worldPos;
	fragColor = inColor;
	fragPos = vec2(worldPos.x, worldPos.y);
}
