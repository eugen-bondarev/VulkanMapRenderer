#include "frame.h"

#include "../device/device.h"

namespace Engine
{
	namespace Vk
	{
		Frame::Frame()
		{
			VkSemaphoreCreateInfo semaphore_info{};
			semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fence_info{};
			fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			VK_CHECK(vkCreateSemaphore(Vk::Global::device->GetVkDevice(), &semaphore_info, nullptr, &imageAvailable), "Failed to create semaphore 1");
			VK_CHECK(vkCreateSemaphore(Vk::Global::device->GetVkDevice(), &semaphore_info, nullptr, &renderFinished), "Failed to create semaphore 2");
			VK_CHECK(vkCreateFence(Vk::Global::device->GetVkDevice(), &fence_info, nullptr, &inFlightFence), "Failed to create fence 1");
		}

		Frame::~Frame()
		{
			vkDestroySemaphore(Vk::Global::device->GetVkDevice(), imageAvailable, nullptr);
			vkDestroySemaphore(Vk::Global::device->GetVkDevice(), renderFinished, nullptr);
			vkDestroyFence(Vk::Global::device->GetVkDevice(), inFlightFence, nullptr);
		}

		VkSemaphore& Frame::GetImageAvailableSemaphore()
		{
			return imageAvailable;
		}

		VkSemaphore& Frame::GetRenderFinishedSemaphore()
		{
			return renderFinished;
		}

		VkFence& Frame::GetInFlightFence()
		{
			return inFlightFence;
		}

		FrameManager::FrameManager(int frames_count) : framesCount { frames_count }
		{		
			for (int i = 0; i < frames_count; i++)
			{
				frames.push_back(new Frame());
			}
		}

		FrameManager::~FrameManager()
		{
			for (int i = 0; i < frames.size(); i++)
			{
				delete frames[i];
			}
		}

		void FrameManager::NextFrame()
		{		
			currentFrame = (currentFrame + 1) % framesCount;
		}

		Frame* FrameManager::GetCurrentFrame()
		{
			return frames[currentFrame];
		}

		int FrameManager::GetAmountOfFrames() const
		{
			return framesCount;
		}

		int FrameManager::GetCurrentFrameIndex() const
		{
			return currentFrame;
		}
	}
}