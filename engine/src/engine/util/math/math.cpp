#include "math.h"

namespace Engine
{
	namespace Util
	{
		namespace Math
		{
			glm::vec2 ExtentToVec2(VkExtent2D extent)
			{
				return { extent.width, extent.height };
			}

			VkExtent2D ExtentToVec2(glm::vec2 vec)
			{
				return { static_cast<uint32_t>(vec.x), static_cast<uint32_t>(vec.y) };
			}
		}
	}
}