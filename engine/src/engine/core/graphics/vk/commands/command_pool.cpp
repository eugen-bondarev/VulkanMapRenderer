#include "command_pool.h"

#include "../device/queue_family.h"
#include "../device/device.h"

namespace Engine
{
	namespace Vk
	{
		namespace Global
		{
			CommandPool *commandPool;
		}

		CommandPool::CommandPool()
		{
			VkCommandPoolCreateInfo pool_info{};
			pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			pool_info.queueFamilyIndex = Global::Queues::indices.graphicsFamily.value();
			pool_info.flags = 0; // Optional

			VT_CHECK(vkCreateCommandPool(Global::device->GetVkDevice(), &pool_info, nullptr, &vkCommandPool));

			VT_TRACE();
		}

		CommandPool::~CommandPool()
		{
			vkDestroyCommandPool(Global::device->GetVkDevice(), vkCommandPool, nullptr);

			VT_TRACE();
		}

		void CommandPool::Reset() const
		{
			VT_CHECK(vkResetCommandPool(Global::device->GetVkDevice(), vkCommandPool, 0));
		}

		VkCommandPool CommandPool::GetVkCommandPool() const
		{
			return vkCommandPool;
		}
	}
}
