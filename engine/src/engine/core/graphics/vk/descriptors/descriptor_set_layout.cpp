#include "descriptor_set_layout.h"

#include "../device/device.h"

namespace Engine
{
	namespace Vk
	{		
		VkDescriptorSetLayoutBinding CreateBinding(uint32_t index, VkDescriptorType type, VkShaderStageFlags stage_flags)
		{
			VkDescriptorSetLayoutBinding binding{};		
			binding.binding = index;
			binding.descriptorType = type;
			binding.descriptorCount = 1;
			binding.stageFlags = stage_flags;
			binding.pImmutableSamplers = nullptr; // Optional

			return binding;
		}

		DescriptorSetLayout::DescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
		{
			VkDescriptorSetLayoutCreateInfo layout_info{};
			layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
			layout_info.pBindings = bindings.data();

			VK_CHECK(vkCreateDescriptorSetLayout(Global::device->GetVkDevice(), &layout_info, nullptr, &vkDescriptorSetLayout), "Failed to create descriptor set layout.");
		}

		DescriptorSetLayout::~DescriptorSetLayout()
		{
			vkDestroyDescriptorSetLayout(Global::device->GetVkDevice(), vkDescriptorSetLayout, nullptr);
		}

		VkDescriptorSetLayout& DescriptorSetLayout::GetVkDescriptorSetLayout()
		{
			return vkDescriptorSetLayout;
		}
	}
}