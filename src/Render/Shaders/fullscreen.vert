#version 450

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec2 fragTexCoords;

void main() {
	gl_Position = vec4(inPosition, 1.0);
	fragTexCoords = (inPosition.xy + 1.0) / 2.0;
}
