#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in uint inPlayerIndex;

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

struct AnimationFrame {
    vec2 texCoords[4];
    float opacity;
};

layout(std140, set = 3, binding = 0) readonly buffer animOffset {
    uint frameOffsets[];
} animToOffset;
layout(std140, set = 3, binding = 1) readonly buffer animFrameIndex {
    uint currentFrames[];
} animToFrameIndex;
layout(std140, set = 3, binding = 2) readonly buffer animFrame {
    AnimationFrame frames[];
} animFrames;

void main() {
	gl_Position = camera.viewProj * object.model[gl_InstanceIndex] * vec4(inPosition, 1.0);
	fragColor = inColor;
    fragTextureId = skin.index[gl_InstanceIndex];

    uint coordOffset = animToOffset.frameOffsets[inPlayerIndex];
    uint frameIndex = animToFrameIndex.currentFrames[inPlayerIndex];
    uint vertexIndex = uint(floor(mod(gl_VertexIndex, 4) + 0.5));
    uint index = coordOffset + frameIndex * 4;
    AnimationFrame frame = animFrames.frames[index];
    
    fragTexCoords = frame.texCoords[vertexIndex];
    fragColor.a = fragColor.a * frame.opacity;
}
