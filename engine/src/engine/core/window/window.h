#pragma once

#include "../graphics/common.h"

namespace Engine
{
	class Window
	{
	public:
		enum class Mode
		{
			Borderless = 0,
			Fullscreen,
			Windowed
		};

	public:
		Window(glm::vec2 size, Mode mode, const std::string& title = "NaturaForge");
		~Window();

		bool ShouldClose() const;

		GLFWwindow* GetGLFWWindow();

	private:
		GLFWwindow* glfwWindow;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
	};
}