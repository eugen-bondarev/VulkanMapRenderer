#pragma once

#include "../../common.h"

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

		void Close() const;

		bool ShouldClose() const;
		glm::vec2 GetSize() const;

		GLFWwindow* GetGLFWWindow();

	private:
		GLFWwindow* glfwWindow;

		glm::vec2 size;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
	};

	extern Window* window;
}