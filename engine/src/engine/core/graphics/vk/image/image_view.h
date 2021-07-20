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
			void SetupDescriptor(VkImageLayout image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VkSampler sampler = Global::constantInterpolationSampler->GetVkSampler());

			ImageView(const ImageView&) = delete;
			ImageView& operator=(const ImageView&) = delete;
		};
	}
}