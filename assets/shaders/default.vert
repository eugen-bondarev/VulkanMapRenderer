#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 proj;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in vec4 perInstancePos;

layout(location = 0) out vec2 fragTexCoords;
layout(location = 1) out vec2 out_Tile;

void main() 
{
    gl_Position = ubo.proj * vec4(inPosition + perInstancePos.xy, 0.0, 1.0);
	fragTexCoords = inTexCoords;
	out_Tile = perInstancePos.zw;
}