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
				VT_CHECK(glfwCreateWindowSurface(instance->GetVkInstance(), window->GetGLFWWindow(), nullptr, &vkSurface));

				VT_TRACE();
			}

			Surface::~Surface()
			{
				vkDestroySurfaceKHR(instance->GetVkInstance(), vkSurface, nullptr);

				VT_TRACE();
			}

			VkSurfaceKHR Surface::GetVkSurface() const
			{
				return vkSurface;
			}
		}
	}
}