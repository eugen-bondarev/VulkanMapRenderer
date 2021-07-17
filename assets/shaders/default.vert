#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 1) uniform UBOInstance {
    mat4 model;
} instance;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() 
{
    gl_Position = ubo.proj * ubo.view * instance.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}