#pragma once

#include "../../../common.h"

namespace Engine
{
	namespace Vk
	{
		struct Vertex
		{
			glm::vec2 Position;
			glm::vec3 Color;
			glm::vec2 TextureCoords;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};
	}
}