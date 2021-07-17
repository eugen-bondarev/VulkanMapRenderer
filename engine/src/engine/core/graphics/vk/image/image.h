#pragma once

#include "../../common.h"

#include "../memory/buffer.h"

namespace Engine
{
	namespace Vk
	{
		namespace Util
		{
			void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
			void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		}

		class Image
		{
		public:
			Image(Buffer* buffer, glm::vec2 size, int amount_of_channels);
			~Image();

			VkImage& GetVkImage();
			VkDeviceMemory& GetVkDeviceMemory();

		private:
			VkImage vkImage;
			VkDeviceMemory vkMemory;

			Image(const Image&) = delete;
			Image& operator=(const Image&) = delete;
		};
	}
}