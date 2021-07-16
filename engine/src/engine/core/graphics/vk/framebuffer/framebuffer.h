#pragma once

#include "../../common.h"

namespace Engine
{
	namespace Vk
	{
		class Framebuffer
		{
		public:
			Framebuffer(VkImageView image_view, VkRenderPass render_pass, const glm::vec2 &size);
			~Framebuffer();

			VkFramebuffer GetVkFramebuffer() const;
			glm::vec2 GetSize() const;

		private:
			VkFramebuffer vkFramebuffer;
			glm::vec2 size;

			Framebuffer(const Framebuffer &) = delete;
			Framebuffer &operator=(const Framebuffer &) = delete;
		};
	}
}