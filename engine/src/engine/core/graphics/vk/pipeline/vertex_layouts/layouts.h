#pragma once

#include "../../../../../common.h"

namespace Engine
{
	namespace Vk
	{
		struct Vertex
		{
			glm::vec2 Position;
			glm::vec2 TextureCoords;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		struct PerInstanceVertex
		{
			glm::vec4 Position;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		struct PerInstanceVertex2D
		{
			glm::vec2 Position;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};
	}
}