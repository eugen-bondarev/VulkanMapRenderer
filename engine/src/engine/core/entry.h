#pragma once

#include "app.h"

#include "window/window.h"
#include "graphics/vk/vk.h"
#include "../assets/assets.h"

#define ENTRY()													\
int main(int amount_of_arguments, char* arguments[])			\
{																\
	using namespace Engine;										\
	Assets::LocateRoot(amount_of_arguments, arguments);			\
	window = new Window({ 800, 600 }, Window::Mode::Windowed); 	\
	Vk::Bootstrap();											\
	App* app = GetApp();										\
	app->Init();												\
	while (!window->ShouldClose())								\
	{															\
		glfwPollEvents();										\
		app->Update();											\
	}															\
	app->Shutdown();											\
	delete app;													\
	Vk::Shutdown();												\
	delete window;												\
	return 0;													\
}