#version 450

layout(location = 0) in vec2 fragTexCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform sampler2D texSampler;

#define AMOUNT_OF_TILES_X 18
#define AMOUNT_OF_TILES_Y 12

void main() 
{
    // outColor = texture(texSampler, fragTexCoords);	
	vec2 tile = vec2(0, 0);
  	outColor = texture(texSampler, (fragTexCoords + tile) / vec2(AMOUNT_OF_TILES_X, AMOUNT_OF_TILES_Y));
}