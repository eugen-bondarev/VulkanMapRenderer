#pragma once

#include "../../../../common.h"

namespace Engine
{
	namespace Vk
	{
		namespace Global
		{
			class Device
			{
			public:
				Device();
				~Device();

				const std::vector<const char *> deviceExtensions =
				{
					VK_KHR_SWAPCHAIN_EXTENSION_NAME
				};

				bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

				uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

				void PickPhysicalDevice();
				bool IsDeviceSuitable(VkPhysicalDevice device);

				void CreateLogicalDevice();

				void WaitIdle() const;

				VkPhysicalDevice GetVkPhysicalDevice() const;
				VkDevice GetVkDevice() const;

				VkPhysicalDeviceProperties properties;

				template <typename T>
				size_t CalculateDynamicAlignment()
				{
					if (properties.limits.minUniformBufferOffsetAlignment > 0) 
					{
						return (sizeof(T) + properties.limits.minUniformBufferOffsetAlignment - 1) & ~(properties.limits.minUniformBufferOffsetAlignment - 1);
					}

					return sizeof(T);
				}

			private:
				VkPhysicalDevice vkPhysicalDevice;
				VkDevice vkDevice;

				Device(const Device &) = delete;
				Device &operator=(const Device &) = delete;
			};

			extern Device *device;

		}
	}
}