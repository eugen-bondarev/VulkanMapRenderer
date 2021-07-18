#pragma once

#include "../../../../common.h"

namespace Engine
{
	namespace Vk
	{
		VkDescriptorSetLayoutBinding CreateBinding(uint32_t index, VkDescriptorType type, VkShaderStageFlags stage_flags = VK_SHADER_STAGE_VERTEX_BIT);

		class DescriptorSetLayout
		{
		public:
			DescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
			~DescriptorSetLayout();

			VkDescriptorSetLayout& GetVkDescriptorSetLayout();

		private:
			VkDescriptorSetLayout vkDescriptorSetLayout;

			DescriptorSetLayout(const DescriptorSetLayout&) = delete;
			DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
		};
	}
}