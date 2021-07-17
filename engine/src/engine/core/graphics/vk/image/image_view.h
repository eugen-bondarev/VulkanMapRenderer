#pragma once

#include "../../common.h"

#include "image.h"

namespace Engine
{
	namespace Vk
	{
		class ImageView
		{
		public:
			ImageView(Image* image);
			~ImageView();

			VkImageView& GetVkImageView();

		private:
			VkImageView vkImageView;

			ImageView(const ImageView&) = delete;
			ImageView& operator=(const ImageView&) = delete;
		};
	}
}