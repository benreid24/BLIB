#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoords;
layout(location = 2) flat out uint fragTextureId;

layout(set = 1, binding = 0) uniform cam {
    mat4 viewProj;
} camera;

layout(std140, set = 2, binding = 0) readonly buffer obj {
    mat4 model[];
} object;
layout(std140, set = 2, binding = 1) readonly buffer tex {
    uint index[];
} skin;

void main() {
	gl_Position = camera.viewProj * object.model[gl_InstanceIndex] * vec4(inPosition, 1.0);
	fragColor = inColor;
	fragTexCoords = inTexCoords;
    fragTextureId = skin.index[gl_InstanceIndex];
}
