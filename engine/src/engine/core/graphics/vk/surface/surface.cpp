#include "surface.h"

#include "../../../window/window.h"
#include "../instance/instance.h"

namespace Engine
{
	namespace Vk
	{
		namespace Global
		{
			Surface* surface;

			Surface::Surface()
			{
				VK_CHECK(glfwCreateWindowSurface(instance->GetVkInstance(), window->GetGLFWWindow(), nullptr, &vkSurface), "Failed to create window surface.");

				TRACE();
			}

			Surface::~Surface()
			{
				vkDestroySurfaceKHR(instance->GetVkInstance(), vkSurface, nullptr);

				TRACE();
			}

			VkSurfaceKHR Surface::GetVkSurface() const
			{
				return vkSurface;
			}
		}
	}
}