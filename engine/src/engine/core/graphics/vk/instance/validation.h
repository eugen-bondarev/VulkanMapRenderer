#pragma once

#include "../../../../common.h"

namespace Engine
{
	namespace Vk
	{
		namespace Validation
		{

			const std::vector<const char *> validationLayers =
				{
					"VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
			constexpr bool enableValidationLayers = false;
#else
			constexpr bool enableValidationLayers = true;
#endif

			bool CheckValidationSupport();

			std::vector<const char *> GetRequestedExtensions();

			static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData);

			extern VkDebugUtilsMessengerEXT debugMessenger;

			void SetupDebugMessenger(VkInstance instance);
			VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
			void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);
			void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
			void Destroy(VkInstance instance);

		}
	}
}