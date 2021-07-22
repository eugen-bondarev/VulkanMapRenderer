#include "descriptor_pool.h"

#include "../device/device.h"

namespace Engine
{
	namespace Vk
	{
		DescriptorPool::DescriptorPool(const std::vector<VkDescriptorPoolSize>& pool_sizes)
		{
			// VkDescriptorPoolSize pool_sizes[] =
			// {
			// 	{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			// 	{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			// 	{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			// 	{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			// 	{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			// 	{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			// 	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			// 	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			// 	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			// 	{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			// 	{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			// 	{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			// };

			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 1000 * pool_sizes.size();
			pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
			pool_info.pPoolSizes = pool_sizes.data();
			VT_CHECK(vkCreateDescriptorPool(Global::device->GetVkDevice(), &pool_info, nullptr, &vkDescriptorPool));
		}

		DescriptorPool::~DescriptorPool()
		{
			vkDestroyDescriptorPool(Global::device->GetVkDevice(), vkDescriptorPool, nullptr);
		}

		VkDescriptorPool& DescriptorPool::GetVkDescriptorPool()
		{
			return vkDescriptorPool;
		}
	}
}