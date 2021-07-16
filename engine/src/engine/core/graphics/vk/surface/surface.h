#pragma once

#include "../../common.h"

namespace Engine
{
	namespace Vk
	{
		namespace Global
		{
			class Surface
			{
			public:
				Surface();
				~Surface();

				VkSurfaceKHR GetVkSurface() const;

			private:
				VkSurfaceKHR vkSurface;
			};

			extern Surface *surface;
		}
	}
}