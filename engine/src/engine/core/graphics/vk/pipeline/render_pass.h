#pragma once

#include "../../../../common.h"

#include "../framebuffer/framebuffer.h"

namespace Engine
{
	namespace Vk
	{
		class RenderPass
		{
		public:
			RenderPass(VkFormat format, bool offscreen = false);
			~RenderPass();

			VkRenderPass& GetVkRenderPass();

		private:
			VkRenderPass vkRenderPass;

			RenderPass(const RenderPass &) = delete;
			RenderPass &operator=(const RenderPass &) = delete;
		};
	}
}