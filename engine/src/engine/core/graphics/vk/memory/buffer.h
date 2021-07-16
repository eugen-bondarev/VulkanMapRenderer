#pragma once

#include "../../common.h"

#include "../commands/command_buffer.h"
#include "../commands/command_pool.h"

namespace Engine
{
	namespace Vk
	{
		namespace Util
		{
			void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
			void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		}

		class Buffer
		{
		public:
			Buffer(
				uint32_t size_of_element, 
				uint32_t amount_of_elements = 0, 
				const void* data = nullptr, 
				VkBufferUsageFlags usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VkMemoryPropertyFlags property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			Buffer(Buffer* buffer, VkBufferUsageFlags usage_flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
			~Buffer();

			void Update(const void* data) const;

			uint32_t GetSize() const;
			uint32_t GetSizeOfElement() const;
			uint32_t GetAmountOfElements() const;

			VkBuffer& GetVkBuffer();
			VkDeviceMemory& GetVkMemory();

		private:
			VkBuffer vkBuffer;
			VkDeviceMemory vkMemory;

			uint32_t sizeOfElement;
			uint32_t amountOfElements;

			Buffer(const Buffer&) = delete;
			Buffer& operator=(const Buffer&) = delete;
		};
	}
}