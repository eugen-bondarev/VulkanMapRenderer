#pragma once

#include "../../../../common.h"

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

			VkDescriptorImageInfo& GetDescriptor();

		private:
			VkImageView vkImageView;

			VkDescriptorImageInfo descriptor;
			void SetupDefaultDescriptor();

			ImageView(const ImageView&) = delete;
			ImageView& operator=(const ImageView&) = delete;
		};
	}
}