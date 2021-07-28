#pragma once

enum FrameSemaphore_
{
	FrameSemaphore_ImageAvailable = 0,
	FrameSemaphore_MapRenderFinished = 1,
	FrameSemaphore_ImGuiRenderFinished = 2,
	
	FrameSemaphore_FIRST_STAGE = FrameSemaphore_ImageAvailable,
	FrameSemaphore_LAST_STAGE = FrameSemaphore_ImGuiRenderFinished,
	
	FrameSemaphore_COUNT = FrameSemaphore_LAST_STAGE + 1
};