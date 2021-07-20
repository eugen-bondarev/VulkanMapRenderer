#pragma once

#include "image.h"
#include "image_view.h"

namespace Engine
{
	namespace Vk
	{
		class Texture2D
		{
		public:
			Texture2D(glm::vec2 size, int amount_of_channels, const void* data = nullptr, VkImageUsageFlags usage_flags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
			~Texture2D();

			Image* GetImage();
			ImageView* GetImageView();

		protected:
			glm::vec2 size;

		private:
			Image* image;
			ImageView* imageView;

			Texture2D(const Texture2D&) = delete;			
			Texture2D& operator=(const Texture2D&) = delete;			
		};
	}
}