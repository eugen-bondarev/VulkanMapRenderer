#include "command_buffer.h"

#include "../device/device.h"
#include "../memory/buffer.h"

namespace Engine
{
	namespace Vk
	{
		CommandBuffer::CommandBuffer(CommandPool *command_pool) : commandPool{command_pool}
		{
			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = command_pool->GetVkCommandPool();
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = 1;

			VK_CHECK(vkAllocateCommandBuffers(Global::device->GetVkDevice(), &allocInfo, &vkCommandBuffer), "Failed to allocate command buffers.");

			TRACE();
		}

		CommandBuffer::~CommandBuffer()
		{
			Free();

			TRACE();
		}

		void CommandBuffer::Begin(VkCommandBufferUsageFlags flags) const
		{
			VkCommandBufferBeginInfo begin_info{};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = flags;			   // Optional
			begin_info.pInheritanceInfo = nullptr; // Optional

			VK_CHECK(vkBeginCommandBuffer(vkCommandBuffer, &begin_info), "Failed to begin recording command buffer.");
		}

		void CommandBuffer::End() const
		{
			VK_CHECK(vkEndCommandBuffer(vkCommandBuffer), "Failed to record command buffer.");
		}

		void CommandBuffer::SubmitToQueue(const VkQueue &queue, VkSemaphore *wait_semaphore, const VkSemaphore *signal_semaphore, VkFence fence) const
		{
			VkSubmitInfo submit_info = {};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &vkCommandBuffer;

			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			if (wait_semaphore)
			{
				submit_info.waitSemaphoreCount = 1;
				submit_info.pWaitSemaphores = wait_semaphore;
				submit_info.pWaitDstStageMask = &wait_stage;
			}

			if (signal_semaphore)
			{
				submit_info.signalSemaphoreCount = 1;
				submit_info.pSignalSemaphores = signal_semaphore;
			}

			VK_CHECK(vkQueueSubmit(queue, 1, &submit_info, fence), "Failed to submit queue.");
		}

		void CommandBuffer::BeginRenderPass(RenderPass *render_pass, Framebuffer *framebuffer) const
		{
			VkRenderPassBeginInfo submit_info = {};
			submit_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			submit_info.renderPass = render_pass->GetVkRenderPass();
			submit_info.framebuffer = framebuffer->GetVkFramebuffer();
			submit_info.renderArea.extent = {static_cast<uint32_t>(framebuffer->GetSize().x), static_cast<uint32_t>(framebuffer->GetSize().y)};
			submit_info.clearValueCount = 1;
			VkClearValue clear_color = { 145.0f / 255.0f, 206.0f / 255.0f, 255.0f / 255.0f, 1.0f };
			// VkClearValue clear_color = { 0.0f, 0.0f, 0.0f, 0.0f };
			submit_info.pClearValues = &clear_color;
			
			vkCmdBeginRenderPass(vkCommandBuffer, &submit_info, VK_SUBPASS_CONTENTS_INLINE);
		}

		void CommandBuffer::EndRenderPass() const
		{
			vkCmdEndRenderPass(vkCommandBuffer);
		}

		void CommandBuffer::BindPipeline(const Pipeline *pipeline) const
		{
			vkCmdBindPipeline(vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetVkPipeline());
		}

		void CommandBuffer::BindVertexBuffers(const std::vector<Buffer *> &buffers, const std::vector<VkDeviceSize> &offsets) const
		{
			VK_ASSERT(buffers.size() == offsets.size());

			std::vector<VkBuffer> vkBuffers(buffers.size());
			for (int i = 0; i < buffers.size(); i++)
			{
				vkBuffers[i] = buffers[i]->GetVkBuffer();
			}
			vkCmdBindVertexBuffers(
				vkCommandBuffer, 0,
				static_cast<uint32_t>(vkBuffers.size()),
				vkBuffers.data(),
				offsets.data());
		}

		void CommandBuffer::BindIndexBuffer(Buffer *index_buffer, VkIndexType index_type) const
		{
			vkCmdBindIndexBuffer(vkCommandBuffer, index_buffer->GetVkBuffer(), 0, index_type);
		}

		void CommandBuffer::BindDescriptorSets(const Pipeline *pipeline, uint32_t amount_of_descriptor_sets, VkDescriptorSet *descriptor_sets, uint32_t amount_of_offsets, uint32_t* offsets) const
		{
			vkCmdBindDescriptorSets(
				vkCommandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipeline->GetVkPipelineLayout(), 0,
				amount_of_descriptor_sets, descriptor_sets, amount_of_offsets, offsets);
		}

		void CommandBuffer::Free() const
		{
			vkFreeCommandBuffers(Global::device->GetVkDevice(), commandPool->GetVkCommandPool(), 1, &vkCommandBuffer);
		}

		VkCommandBuffer &CommandBuffer::GetVkCommandBuffer()
		{
			return vkCommandBuffer;
		}
	}
}