#pragma once

#include "app.h"

#include "window/window.h"
#include "graphics/vk/vk.h"

#define ENTRY()													\
int main()														\
{																\
	using namespace Engine;										\
	window = new Window({ 800, 600 }, Window::Mode::Windowed); \
	Vk::Bootstrap();											\
	App* app = GetApp();										\
	app->Init();												\
																\
	while (!window->ShouldClose())								\
	{															\
		glfwPollEvents();										\
		app->Update();											\
	}															\
	Vk::Shutdown();												\
																\
	return 0;													\
}