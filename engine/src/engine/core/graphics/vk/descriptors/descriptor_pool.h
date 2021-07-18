#pragma once

#include "../../../../common.h"

namespace Engine
{
	namespace Vk
	{
		class DescriptorPool
		{
		public:
			DescriptorPool(const std::vector<VkDescriptorPoolSize>& pool_sizes);
			~DescriptorPool();

			VkDescriptorPool& GetVkDescriptorPool();

		private:
			VkDescriptorPool vkDescriptorPool;

			DescriptorPool(const DescriptorPool&) = delete;
			DescriptorPool& operator=(const DescriptorPool&) = delete;
		};
	}
}