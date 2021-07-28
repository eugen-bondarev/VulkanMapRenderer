#include "frame.h"

#include "../device/device.h"
#include "../swap_chain/swap_chain.h"

namespace Engine
{
	namespace Vk
	{
		Frame::Frame(int first_semaphore, int last_semaphore, int amount_of_semaphores) : firstSemaphore { first_semaphore }, lastSemaphore { last_semaphore }
		{
			VkSemaphoreCreateInfo semaphore_info{};
			semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			VkFenceCreateInfo fence_info{};
			fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			semaphores.resize(amount_of_semaphores);

			for (int i = 0; i < amount_of_semaphores; i++)
			{
				VT_CHECK(vkCreateSemaphore(Global::device->GetVkDevice(), &semaphore_info, nullptr, &semaphores[i]));
			}
			
			VT_CHECK(vkCreateFence(Global::device->GetVkDevice(), &fence_info, nullptr, &inFlightFence));
		}

		Frame::~Frame()
		{
			for (auto& semaphore : semaphores)
				vkDestroySemaphore(Global::device->GetVkDevice(), semaphore, nullptr);

			vkDestroyFence(Global::device->GetVkDevice(), inFlightFence, nullptr);
		}

		VkSemaphore& Frame::GetSemaphore(int semaphore_id)
		{
			return semaphores[semaphore_id];
		}

		VkFence& Frame::GetInFlightFence()
		{
			return inFlightFence;
		}

		FrameManager::FrameManager(int first_semaphore, int last_semaphore, int amount_of_semaphores_per_frame, int frames_count) : framesCount { frames_count }
		{		
			for (int i = 0; i < frames_count; i++)
			{
				frames.push_back(new Frame(first_semaphore, last_semaphore, amount_of_semaphores_per_frame));
			}

			imagesInFlight.resize(Global::swapChain->GetImageViews().size());
		}

		FrameManager::~FrameManager()
		{
			for (int i = 0; i < frames.size(); i++)
			{
				delete frames[i];
			}
		}

		uint32_t FrameManager::AcquireSwapChainImage()
		{
			Frame* frame = GetCurrentFrame();			
			uint32_t image_index = Global::swapChain->AcquireImage(frame->GetSemaphore(frame->firstSemaphore));

			if (imagesInFlight[image_index] != VK_NULL_HANDLE)
			{
				vkWaitForFences(Global::device->GetVkDevice(), 1, &imagesInFlight[image_index], VK_TRUE, UINT64_MAX);
			}
			
			vkWaitForFences(Global::device->GetVkDevice(), 1, &frame->GetInFlightFence(), VK_TRUE, UINT64_MAX);
			vkResetFences(Global::device->GetVkDevice(), 1, &frame->GetInFlightFence());
			imagesInFlight[image_index] = frame->GetInFlightFence();

			return image_index;
		}

		void FrameManager::Present()
		{			
			Frame* frame = GetCurrentFrame();
			Global::swapChain->Present(&frame->GetSemaphore(frame->lastSemaphore), 1);
			NextFrame();
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