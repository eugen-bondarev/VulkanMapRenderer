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

enum MenuFrameSemaphore_
{
	MenuFrameSemaphore_ImageAvailable = 0,
	MenuFrameSemaphore_ImGuiRenderFinished = 1,	
	MenuFrameSemaphore_FIRST_STAGE = MenuFrameSemaphore_ImageAvailable,
	MenuFrameSemaphore_LAST_STAGE = MenuFrameSemaphore_ImGuiRenderFinished,	
	MenuFrameSemaphore_COUNT = MenuFrameSemaphore_LAST_STAGE + 1
};

inline static bool menu = true;