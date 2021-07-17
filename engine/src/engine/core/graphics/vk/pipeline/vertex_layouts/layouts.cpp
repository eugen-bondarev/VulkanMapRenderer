#include "layouts.h"

namespace Engine
{
	namespace Vk
	{
		std::vector<VkVertexInputBindingDescription> Vertex::GetBindingDescriptions()
		{
			std::vector<VkVertexInputBindingDescription> binding_descriptions;

			VkVertexInputBindingDescription binding_description = {};
			binding_description.binding = 0;
			binding_description.stride = sizeof(Vertex);
			binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			
			binding_descriptions.push_back(binding_description);

			return binding_descriptions;
		}

		std::vector<VkVertexInputAttributeDescription> Vertex::GetAttributeDescriptions() 
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
			attributeDescriptions.resize(3, {});

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, Position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, Color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, TextureCoords);

			return attributeDescriptions;
		}
	}
}