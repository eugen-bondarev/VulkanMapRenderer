#pragma once

#include "../../common.h"

#include <optional>

namespace Engine
{
	namespace Vk
	{
		namespace Global
		{
			namespace Queues
			{
				struct QueueFamilyIndices
				{
					std::optional<uint32_t> graphicsFamily;
					std::optional<uint32_t> presentFamily;

					bool IsComplete() const;
				};

				extern QueueFamilyIndices indices;
				QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

				extern VkQueue graphicsQueue;
				extern VkQueue presentQueue;
			}
		}
	}
}