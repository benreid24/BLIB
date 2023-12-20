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
    // array alignment is jank :(
    vec2 texCoord0;
    vec2 texCoord1;
    vec2 texCoord2;
    vec2 texCoord3;
    float opacity;
};

layout(std140, set = 3, binding = 0) readonly buffer animOffset {
    uint frameOffsets[];
} animToOffset;
layout(std140, set = 3, binding = 1) readonly buffer animTexture {
    uint textureIds[];
} animToTexture;
layout(std140, set = 3, binding = 2) readonly buffer animFrameIndex {
    uint currentFrames[];
} animToFrameIndex;
layout(std140, set = 3, binding = 3) readonly buffer animFrame {
    AnimationFrame frames[];
} animFrames;

void main() {
	gl_Position = camera.viewProj * object.model[gl_InstanceIndex] * vec4(inPosition, 1.0);
	fragColor = inColor;
    fragTextureId = animToTexture.textureIds[inPlayerIndex];

    uint coordOffset = animToOffset.frameOffsets[inPlayerIndex];
    uint frameIndex = animToFrameIndex.currentFrames[inPlayerIndex];
    uint vertexIndex = uint(floor(mod(gl_VertexIndex, 4) + 0.5));
    uint index = coordOffset + frameIndex;
    AnimationFrame frame = animFrames.frames[index];
    
    vec2 texCoords[4] = {frame.texCoord0, frame.texCoord1, frame.texCoord2, frame.texCoord3};
    fragTexCoords = texCoords[vertexIndex];
    fragColor.a = fragColor.a * frame.opacity;
}
