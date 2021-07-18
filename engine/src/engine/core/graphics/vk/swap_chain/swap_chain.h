#pragma once

#include "../../../../common.h"

#include "../framebuffer/framebuffer.h"

namespace Engine
{
	namespace Vk
	{
		namespace Global
		{
			class SwapChain
			{
			public:
				SwapChain();
				~SwapChain();

				uint32_t AcquireImage(VkSemaphore semaphore);

				VkResult Present(VkSemaphore* wait_semaphores, uint32_t wait_semaphore_count);

				uint32_t GetCurrentImageIndex() const;

				VkSwapchainKHR GetVkSwapChain() const;
				VkFormat GetImageFormat() const;
				VkSurfaceFormatKHR GetSurfaceFormat() const;
				VkExtent2D GetExtent() const;

				void InitFramebuffers(VkRenderPass& render_pass);
				Vk::Framebuffer* GetCurrentScreenFramebuffer();

				const std::vector<VkImage> &GetImages() const;
				const std::vector<VkImageView> &GetImageViews() const;

			private:
				uint32_t imageIndex;

				std::vector<Vk::Framebuffer*> framebuffers;

				VkSwapchainKHR vkSwapChain;

				VkFormat imageFormat;
				VkSurfaceFormatKHR surfaceFormat;
				VkPresentModeKHR presentMode;
				VkExtent2D extent;

				std::vector<VkImage> images;
				std::vector<VkImageView> imageViews;

				VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
				VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
				VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR &capabilities);

				void CreateImageViews();
				void DestroyImageViews();
			};

			extern SwapChain *swapChain;
		}
	}
}