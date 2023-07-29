#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoords;

layout(set = 0, binding = 0) uniform cam {
    mat4 viewProj;
} camera;

layout(set = 1, binding = 0) uniform obj {
    mat4 model;
} object;

void main() {
	gl_Position = camera.viewProj * object.model * vec4(inPosition, 1.0);
	fragColor = inColor;
	fragTexCoords = inTexCoords;
}
