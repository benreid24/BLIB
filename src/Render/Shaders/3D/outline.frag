#version 450

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    layout(offset = 16) vec4 color;
} outline;

void main() {
    outColor = outline.color;
}
