#pragma once

#include "../../common.h"

namespace Engine
{
	namespace Vk
	{
		class Frame
		{
		public:
			Frame();
			~Frame();

			VkSemaphore& GetImageAvailableSemaphore();
			VkSemaphore& GetRenderFinishedSemaphore();
			VkFence& GetInFlightFence();

		private:
			VkSemaphore imageAvailable;
			VkSemaphore renderFinished;
			VkFence inFlightFence;

			Frame(const Frame&) = delete;
			Frame& operator=(const Frame&) = delete;
		};

		class FrameManager
		{
		public:
			FrameManager(int frames_count = 2);
			~FrameManager();

			void NextFrame();
			Frame* GetCurrentFrame();

			int GetAmountOfFrames() const;
			int GetCurrentFrameIndex() const;
			
		private:
			int framesCount = 0;
			int currentFrame = 0;
			std::vector<Frame*> frames;

			FrameManager(const FrameManager&) = delete;
			FrameManager& operator=(const FrameManager&) = delete;
		};
	}
}