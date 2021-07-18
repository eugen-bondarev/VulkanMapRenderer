#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in vec2 perInstancePos;

layout(location = 0) out vec2 fragTexCoords;

void main() 
{
    gl_Position = ubo.proj * ubo.view * vec4(inPosition + perInstancePos, 0.0, 1.0);
	fragTexCoords = inTexCoords;
}