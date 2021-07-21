#include "image.h"

#include "../device/device.h"
#include "../device/queue_family.h"

#include "../commands/command_pool.h"
#include "../commands/command_buffer.h"

namespace Engine
{
	namespace Vk
	{
		namespace Util
		{			
			void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
			{
				CommandBuffer command_buffer(Global::commandPool);
				
				command_buffer.Begin();

				VkImageMemoryBarrier barrier{};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.oldLayout = oldLayout;
				barrier.newLayout = newLayout;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.image = image;
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				barrier.subresourceRange.baseMipLevel = 0;
				barrier.subresourceRange.levelCount = 1;
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.layerCount = 1;

				// barrier.srcAccessMask = 0; // TODO
				// barrier.dstAccessMask = 0; // TODO

				VkPipelineStageFlags src_stage;
				VkPipelineStageFlags dst_stage;

				if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
				{
					barrier.srcAccessMask = 0;
					barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

					src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				}
				else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
				{
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

					src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
					dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				} 
				else if (oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) 
				{
					barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
					barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

					src_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
					dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				} 
				else 
				{
					THROW("Unsupported layout transition.");
				}

				vkCmdPipelineBarrier(
					command_buffer.GetVkCommandBuffer(),
					src_stage, dst_stage,
					0,
					0, nullptr,
					0, nullptr,
					1, &barrier
				);

				command_buffer.End();
				command_buffer.SubmitToQueue(Global::Queues::graphicsQueue);
    			vkQueueWaitIdle(Global::Queues::graphicsQueue);
			}

			void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
			{
				CommandBuffer command_buffer(Global::commandPool);
				
				command_buffer.Begin();

				VkBufferImageCopy region{};
				region.bufferOffset = 0;
				region.bufferRowLength = 0;
				region.bufferImageHeight = 0;

				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.mipLevel = 0;
				region.imageSubresource.baseArrayLayer = 0;
				region.imageSubresource.layerCount = 1;

				region.imageOffset = {0, 0, 0};
				region.imageExtent = {
					width,
					height,
					1
				};

				vkCmdCopyBufferToImage(
					command_buffer.GetVkCommandBuffer(),
					buffer,
					image,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&region
				);

				command_buffer.End();
				command_buffer.SubmitToQueue(Global::Queues::graphicsQueue);
    			vkQueueWaitIdle(Global::Queues::graphicsQueue);
			}
		}

		Image::Image(Buffer* buffer, glm::vec2 size, int amount_of_channels, VkImageUsageFlags usage_flags)
		{
			VkImageCreateInfo image_info{};
			image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_info.imageType = VK_IMAGE_TYPE_2D;
			image_info.extent.width = static_cast<uint32_t>(size.x);
			image_info.extent.height = static_cast<uint32_t>(size.y);
			image_info.extent.depth = 1;
			image_info.mipLevels = 1;
			image_info.arrayLayers = 1;
			// image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
			image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
			image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
			image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			image_info.usage = usage_flags;
			image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			image_info.samples = VK_SAMPLE_COUNT_1_BIT;
			image_info.flags = 0; // Optional
			VK_CHECK(vkCreateImage(Global::device->GetVkDevice(), &image_info, nullptr, &vkImage), "Failed to create image.");

			VkMemoryRequirements mem_requirements;
			vkGetImageMemoryRequirements(Global::device->GetVkDevice(), vkImage, &mem_requirements);

			VkMemoryAllocateInfo alloc_info{};
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = mem_requirements.size;
			alloc_info.memoryTypeIndex = Global::device->FindMemoryType(mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VK_CHECK(vkAllocateMemory(Global::device->GetVkDevice(), &alloc_info, nullptr, &vkMemory), "Failed to allocate image memory.");

			vkBindImageMemory(Global::device->GetVkDevice(), vkImage, vkMemory, 0);

			Util::TransitionImageLayout(vkImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			Util::CopyBufferToImage(buffer->GetVkBuffer(), vkImage, static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y));
			Util::TransitionImageLayout(vkImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			TRACE();
		}

		Image::~Image()
		{
			vkDestroyImage(Global::device->GetVkDevice(), vkImage, nullptr);
			vkFreeMemory(Global::device->GetVkDevice(), vkMemory, nullptr);
			TRACE();
		}

		VkImage& Image::GetVkImage()
		{
			return vkImage;
		}

		VkDeviceMemory& Image::GetVkDeviceMemory()
		{
			return vkMemory;
		}
	}
}