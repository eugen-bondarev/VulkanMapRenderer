#include "swap_chain.h"

#include "../../../window/window.h"
#include "../device/device.h"
#include "../device/queue_family.h"
#include "support_details.h"

namespace Engine
{
	namespace Vk
	{
		namespace Global
		{
			SwapChain *swapChain;

			SwapChain::SwapChain()
			{
				SupportDetails support_details = QuerySwapChainSupport(device->GetVkPhysicalDevice());

				surfaceFormat = ChooseSurfaceFormat(support_details.formats);
				// surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
				presentMode = ChoosePresentMode(support_details.presentModes);
				extent = ChooseExtent(support_details.capabilities);

				uint32_t image_count = support_details.capabilities.minImageCount + 1;

				if (support_details.capabilities.maxImageCount > 0 && image_count > support_details.capabilities.maxImageCount)
				{
					image_count = support_details.capabilities.maxImageCount;
				}

				VkSwapchainCreateInfoKHR create_info{};
				create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				create_info.surface = surface->GetVkSurface();
				create_info.minImageCount = image_count;
				create_info.imageFormat = surfaceFormat.format;
				create_info.imageColorSpace = surfaceFormat.colorSpace;
				create_info.imageExtent = extent;
				create_info.imageArrayLayers = 1;
				create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

				std::array<uint32_t, 2> queueFamilyIndices =
				{
					Queues::indices.graphicsFamily.value(), Queues::indices.presentFamily.value()
				};

				if (Queues::indices.graphicsFamily != Queues::indices.presentFamily)
				{
					create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
					create_info.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
					create_info.pQueueFamilyIndices = queueFamilyIndices.data();
				}
				else
				{
					create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
					create_info.queueFamilyIndexCount = 0;	  // Optional
					create_info.pQueueFamilyIndices = nullptr; // Optional
				}

				create_info.preTransform = support_details.capabilities.currentTransform;
				create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
				create_info.presentMode = presentMode;
				create_info.clipped = VK_TRUE;
				create_info.oldSwapchain = VK_NULL_HANDLE;

				VT_CHECK(vkCreateSwapchainKHR(device->GetVkDevice(), &create_info, nullptr, &vkSwapChain));

				vkGetSwapchainImagesKHR(device->GetVkDevice(), vkSwapChain, &image_count, nullptr);
				images.resize(image_count);
				vkGetSwapchainImagesKHR(device->GetVkDevice(), vkSwapChain, &image_count, images.data());

				imageFormat = surfaceFormat.format;

				CreateImageViews();

				VT_TRACE();
			}

			SwapChain::~SwapChain()
			{
				DestroyImageViews();

				vkDestroySwapchainKHR(device->GetVkDevice(), vkSwapChain, nullptr);

				for (auto& framebuffer : framebuffers)
					delete framebuffer;

				VT_TRACE();
			}

			Vk::Framebuffer* SwapChain::GetCurrentScreenFramebuffer()
			{
				return framebuffers[imageIndex];
			}

			void SwapChain::InitFramebuffers(VkRenderPass& render_pass)
			{
				for (auto& framebuffer : framebuffers)
					delete framebuffer;
				framebuffers.clear();

				glm::vec2 viewport_size = { extent.width, extent.height };
				for (const VkImageView& image_view : Vk::Global::swapChain->GetImageViews())
					framebuffers.push_back(new Vk::Framebuffer(image_view, render_pass, viewport_size));
			}

			uint32_t SwapChain::AcquireImage(VkSemaphore semaphore)
			{
				vkAcquireNextImageKHR(device->GetVkDevice(), vkSwapChain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &imageIndex);

				return imageIndex;
			}

			VkResult SwapChain::Present(VkSemaphore* wait_semaphores, uint32_t wait_semaphore_count)
			{
				VkPresentInfoKHR present_info{};
				present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

				present_info.waitSemaphoreCount = wait_semaphore_count;
				present_info.pWaitSemaphores = wait_semaphores;

				present_info.swapchainCount = 1;
				present_info.pSwapchains = &vkSwapChain;
				present_info.pImageIndices = &imageIndex;
				present_info.pResults = nullptr;

				return vkQueuePresentKHR(Vk::Global::Queues::presentQueue, &present_info);
			}

			uint32_t SwapChain::GetCurrentImageIndex() const
			{
				return imageIndex;
			}

			VkSurfaceFormatKHR SwapChain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availalbe_formats)
			{				
				VkFormat colorFormat;
				VkColorSpaceKHR colorSpace;
				VkSurfaceFormatKHR format;

				// Get list of supported surface formats
				uint32_t formatCount;
				vkGetPhysicalDeviceSurfaceFormatsKHR(Global::device->GetVkPhysicalDevice(), Global::surface->GetVkSurface(), &formatCount, NULL);				
				assert(formatCount > 0);

				std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(Global::device->GetVkPhysicalDevice(), Global::surface->GetVkSurface(), &formatCount, surfaceFormats.data());

				// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
				// there is no preferred format, so we assume VK_FORMAT_B8G8R8A8_UNORM
				if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
				{
					colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
					colorSpace = surfaceFormats[0].colorSpace;
				}
				else
				{
					// iterate over the list of available surface format and
					// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
					bool found_B8G8R8A8_UNORM = false;
					for (auto&& surfaceFormat : surfaceFormats)
					{
						if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
						{
							colorFormat = surfaceFormat.format;
							colorSpace = surfaceFormat.colorSpace;
							found_B8G8R8A8_UNORM = true;
							break;
						}
					}

					// in case VK_FORMAT_B8G8R8A8_UNORM is not available
					// select the first available color format
					if (!found_B8G8R8A8_UNORM)
					{
						colorFormat = surfaceFormats[0].format;
						colorSpace = surfaceFormats[0].colorSpace;
					}
				}

				format.colorSpace = colorSpace;
				format.format = colorFormat;

				return format;

				// for (const auto &available_format : availalbe_formats)
				// {
				// 	if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				// 	{
				// 		return available_format;
				// 	}
				// }

				// return availalbe_formats[0];
			}

			VkPresentModeKHR SwapChain::ChoosePresentMode(const std::vector<VkPresentModeKHR> &available_present_modes)
			{
				for (const auto &available_present_mode : available_present_modes)
				{
					if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
					{
						return available_present_mode;
					}
				}

				return VK_PRESENT_MODE_FIFO_KHR;
			}

			VkExtent2D SwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR &capabilities)
			{
				if (capabilities.currentExtent.width != UINT32_MAX)
				{
					return capabilities.currentExtent;
				}
				else
				{
					int width, height;
					glfwGetFramebufferSize(window->GetGLFWWindow(), &width, &height);

					VkExtent2D actual_extent =
					{
						static_cast<uint32_t>(width),
						static_cast<uint32_t>(height)
					};

					actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
					actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

					return actual_extent;
				}
			}

			void SwapChain::CreateImageViews()
			{
				imageViews.resize(images.size());

				for (size_t i = 0; i < images.size(); i++)
				{
					VkImageViewCreateInfo createInfo{};
					createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					createInfo.image = images[i];
					createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
					createInfo.format = imageFormat;
					createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
					createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
					createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
					createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
					createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					createInfo.subresourceRange.baseMipLevel = 0;
					createInfo.subresourceRange.levelCount = 1;
					createInfo.subresourceRange.baseArrayLayer = 0;
					createInfo.subresourceRange.layerCount = 1;

					VT_CHECK(vkCreateImageView(device->GetVkDevice(), &createInfo, nullptr, &imageViews[i]));
				}
			}

			void SwapChain::DestroyImageViews()
			{
				for (const auto &imageView : imageViews)
				{
					vkDestroyImageView(device->GetVkDevice(), imageView, nullptr);
				}
			}

			VkSwapchainKHR SwapChain::GetVkSwapChain() const
			{
				return vkSwapChain;
			}

			VkFormat SwapChain::GetImageFormat() const
			{
				return imageFormat;
			}

			VkSurfaceFormatKHR SwapChain::GetSurfaceFormat() const
			{
				return surfaceFormat;
			}

			VkExtent2D SwapChain::GetExtent() const
			{
				return extent;
			}

			std::vector<Vk::Framebuffer*>& SwapChain::GetFramebuffers()
			{
				return framebuffers;
			}

			const std::vector<VkImage> &SwapChain::GetImages() const
			{
				return images;
			}

			const std::vector<VkImageView> &SwapChain::GetImageViews() const
			{
				return imageViews;
			}
		}
	}
}