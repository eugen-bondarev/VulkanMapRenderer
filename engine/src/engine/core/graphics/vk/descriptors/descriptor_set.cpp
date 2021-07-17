#include "descriptor_set.h"

#include "../device/device.h"

namespace Engine
{
	namespace Vk
	{
		VkWriteDescriptorSet CreateWriteDescriptorSet(DescriptorSet* descriptor_set, uint32_t binding, VkDescriptorType descriptor_type, const VkDescriptorBufferInfo* descriptor_buffer_info)
		{
			VkWriteDescriptorSet descriptor_write{};
			descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_write.dstSet = descriptor_set->GetVkDescriptorSet();
			descriptor_write.dstBinding = binding;
			descriptor_write.dstArrayElement = 0;
			descriptor_write.descriptorType = descriptor_type;
			descriptor_write.descriptorCount = 1;
			descriptor_write.pBufferInfo = descriptor_buffer_info;

			return descriptor_write;
		}

		DescriptorSet::DescriptorSet(DescriptorPool* descriptor_pool, const std::vector<VkDescriptorSetLayout>& layouts)
		{
        	VkDescriptorSetAllocateInfo alloc_info{};
			alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			alloc_info.descriptorPool = descriptor_pool->GetVkDescriptorPool();			
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = layouts.data();

			VK_CHECK(vkAllocateDescriptorSets(Global::device->GetVkDevice(), &alloc_info, &vkDescriptorSet), "Failed to allocate descriptor sets.");
		}

		DescriptorSet::~DescriptorSet()
		{
		}

		void DescriptorSet::Update(const std::vector<VkWriteDescriptorSet>& write_descriptor_sets)
		{
			vkUpdateDescriptorSets(Vk::Global::device->GetVkDevice(), static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);
		}

		VkDescriptorSet& DescriptorSet::GetVkDescriptorSet()
		{
			return vkDescriptorSet;
		}
	}
}