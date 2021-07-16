#include "queue_family.h"

#include "../surface/surface.h"

namespace Engine
{
	namespace Vk
	{
		namespace Global
		{
			namespace Queues
			{			
				QueueFamilyIndices indices = {};

				VkQueue graphicsQueue = VK_NULL_HANDLE;
				VkQueue presentQueue = VK_NULL_HANDLE;

				bool QueueFamilyIndices::IsComplete() const
				{
					return graphicsFamily.has_value() && presentFamily.has_value();
				}

				QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
				{
					QueueFamilyIndices newIndices;

					uint32_t queueFamilyCount = 0;
					vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

					std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
					vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

					int i = 0;
					for (const auto &queueFamily : queueFamilies)
					{
						if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
						{
							newIndices.graphicsFamily = i;

							VkBool32 presentSupport = false;
							vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface->GetVkSurface(), &presentSupport);

							if (presentSupport)
							{
								newIndices.presentFamily = i;
							}

							if (newIndices.IsComplete())
							{
								break;
							}
						}

						i++;
					}

					return newIndices;
				}
			}
		}
	}
}