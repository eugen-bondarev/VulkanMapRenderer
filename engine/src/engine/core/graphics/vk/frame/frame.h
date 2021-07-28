#pragma once

#include "../../../../common.h"

namespace Engine
{
	namespace Vk
	{
		class Frame
		{
		friend class FrameManager;

		public:
			Frame(int first_semaphore, int last_semaphore, int amount_of_semaphores);
			~Frame();

			VkSemaphore& GetSemaphore(int semaphore_id);

			VkFence& GetInFlightFence();

			std::vector<VkSemaphore> semaphores;

		private:
			int firstSemaphore;
			int lastSemaphore;

			VkFence inFlightFence;

			Frame(const Frame&) = delete;
			Frame& operator=(const Frame&) = delete;
		};

		class FrameManager
		{
		public:
			FrameManager(int first_semaphore, int last_semaphore, int amount_of_semaphores_per_frame, int frames_count);
			~FrameManager();

			void NextFrame();
			Frame* GetCurrentFrame();

			int GetAmountOfFrames() const;
			int GetCurrentFrameIndex() const;

			uint32_t AcquireSwapChainImage();
			void Present();

			std::vector<VkFence> imagesInFlight;
			
		private:
			int framesCount = 0;
			int currentFrame = 0;
			std::vector<Frame*> frames;

			FrameManager(const FrameManager&) = delete;
			FrameManager& operator=(const FrameManager&) = delete;
		};
	}
}