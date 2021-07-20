#version 450

layout(location = 0) in vec2 fragTexCoords;
layout(location = 1) in vec2 out_Tile;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

#define AMOUNT_OF_TILES_X 18
#define AMOUNT_OF_TILES_Y 12

void main() 
{
    outColor = texture(texSampler, fragTexCoords);	
}