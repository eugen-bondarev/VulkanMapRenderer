#pragma once

#include "../../../../common.h"

namespace Engine
{
	namespace Vk
	{
		class CommandPool
		{
		public:
			CommandPool();
			~CommandPool();

			void Reset() const;

			VkCommandPool GetVkCommandPool() const;

		private:
			VkCommandPool vkCommandPool;

			CommandPool(const CommandPool &) = delete;
			CommandPool &operator=(const CommandPool &) = delete;
		};

		namespace Global
		{
			extern CommandPool *commandPool;
		}
	}
}