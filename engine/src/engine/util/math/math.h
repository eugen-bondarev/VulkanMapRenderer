#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
	namespace Util
	{
		namespace Math
		{
			glm::vec2 ExtentToVec2(VkExtent2D extent);
			VkExtent2D ExtentToVec2(glm::vec2 vec);
		}
	}
}