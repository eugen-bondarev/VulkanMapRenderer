#version 450

layout(location = 0) in vec2 fragTexCoords;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler;
layout(binding = 1) uniform sampler2D lightPassResult;

void main() 
{
	vec4 pureColor = texture(texSampler, fragTexCoords);
	float light = texture(lightPassResult, fragTexCoords).r;
	float acc = pow(light, 1);
	outColor = vec4(pureColor.xyz * acc, pureColor.a);
}