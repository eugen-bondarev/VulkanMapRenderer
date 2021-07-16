#include "support_details.h"

namespace Engine
{
	namespace Vk
	{		
		SupportDetails QuerySwapChainSupport(VkPhysicalDevice device)
		{
			SupportDetails details;

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, Global::surface->GetVkSurface(), &details.capabilities);

			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, Global::surface->GetVkSurface(), &formatCount, nullptr);

			if (formatCount != 0)
			{
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, Global::surface->GetVkSurface(), &formatCount, details.formats.data());
			}

			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, Global::surface->GetVkSurface(), &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, Global::surface->GetVkSurface(), &presentModeCount, details.presentModes.data());
			}

			return details;
		}
	}
}