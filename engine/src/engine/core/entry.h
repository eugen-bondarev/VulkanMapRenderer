#pragma once

#include "app.h"

#include "window/window.h"
#include "window/time.h"
#include "graphics/vk/vk.h"
#include "../assets/assets.h"
#include "../tasks/tasks.h"

#define ENTRY()														\
int main(int amount_of_arguments, char* arguments[])				\
{																	\
	VT_PROFILER_BEGIN("Forgio");									\
	using namespace Engine;											\
	Assets::LocateRoot(amount_of_arguments, arguments);				\
	window = new Window({ 1920, 1080 }, Window::Mode::Borderless);	\
	Vk::Bootstrap();												\
	App* app = GetApp();											\
	app->Init();													\
	while (!window->ShouldClose())									\
	{																\
		Time::BeginMeasurement();									\
		Tasks::ExecuteStack();\
		glfwPollEvents();											\
		app->Update();												\
		Time::EndMeasurement();										\
	}																\
	app->Shutdown();												\
	delete app;														\
	Vk::Shutdown();													\
	delete window;													\
	VT_PROFILER_END();												\
	return 0;														\
}