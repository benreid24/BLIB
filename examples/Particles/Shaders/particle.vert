#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoords;
layout(location = 2) flat out uint fragTextureId;
layout(location = 3) out vec2 fragPos;

layout(set = 1, binding = 0) uniform cam {
    mat4 viewProj;
} camera;

layout(std140, set = 2, binding = 0) readonly buffer pcl {
    vec2 pos[];
} particles;

void main() {
    vec4 worldPos = vec4(particles.pos[gl_InstanceIndex], 0.0, 1.0) + vec4(inPosition, 1.0);
	gl_Position = camera.viewProj * worldPos;
	fragColor = inColor;
	fragTexCoords = inTexCoords;
    fragTextureId = 0; // TODO - add textures to descriptor set
    fragPos = vec2(worldPos.x, worldPos.y);
}
