#include "image_view.h"

#include "../device/device.h"

namespace Engine
{
	namespace Vk
	{
		ImageView::ImageView(Image* image)
		{
			VkImageViewCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.image = image->GetVkImage();
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			// create_info.format = VK_FORMAT_R8G8B8A8_SRGB;
			create_info.format = VK_FORMAT_R8G8B8A8_UNORM;
			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;

			VK_CHECK(vkCreateImageView(Global::device->GetVkDevice(), &create_info, nullptr, &vkImageView), "Failed to create texture image view.");

			SetupDefaultDescriptor();

			TRACE();
		}

		ImageView::~ImageView()
		{
			vkDestroyImageView(Global::device->GetVkDevice(), vkImageView, nullptr);
			TRACE();
		}

		VkImageView& ImageView::GetVkImageView()
		{
			return vkImageView;
		}

		void ImageView::SetupDefaultDescriptor()
		{
			descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptor.imageView = vkImageView;
			descriptor.sampler = Global::constantInterpolationSampler->GetVkSampler();
		}

		void ImageView::SetupDescriptor(VkImageLayout image_layout, VkSampler sampler)
		{
			descriptor.imageLayout = image_layout;
			descriptor.sampler = sampler;
		}

		VkDescriptorImageInfo& ImageView::GetDescriptor()
		{
			return descriptor;
		}
	}
}