#pragma once

#include "../../../../common.h"

#include "image.h"

#include "sampler.h"

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

			void SetupDescriptor(VkImageLayout image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VkSampler sampler = Global::constantInterpolationSampler->GetVkSampler());

		private:
			VkImageView vkImageView;

			VkDescriptorImageInfo descriptor;
			void SetupDefaultDescriptor();

			ImageView(const ImageView&) = delete;
			ImageView& operator=(const ImageView&) = delete;
		};
	}
}