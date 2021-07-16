#include "window.h"

#include <iostream>

namespace Engine
{
	Window* window;

	Window::Window(glm::vec2 size, Mode mode, const std::string& title)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		const auto &monitor = glfwGetPrimaryMonitor();

		if (mode == Mode::Borderless)
		{
			const auto &vm = glfwGetVideoMode(monitor);
			glfwWindowHint(GLFW_RED_BITS, vm->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, vm->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, vm->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, vm->refreshRate);
		}

		glfwWindow = glfwCreateWindow(size.x, size.y, "Vulkan Tutorial", (mode == Mode::Borderless || mode == Mode::Fullscreen) ? monitor : nullptr, nullptr);

		// int w, h;
		// glfwGetFramebufferSize(glfwWindow, &w, &h);
		// size = { static_cast<float>(w), static_cast<float>(h) };
		// lastSize = size;

		// glfwSetWindowUserPointer(glfwWindow, &size);

		// glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* window, int w, int h) 
		// {
		// 	glm::vec2& size = *(glm::vec2*)glfwGetWindowUserPointer(window);
		// 	size = { static_cast<float>(w), static_cast<float>(h) };
		// });
	}

	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(glfwWindow);
	}

	Window::~Window()
	{
		std::cout << "Window destroyed.\n";
	}

	GLFWwindow* Window::GetGLFWWindow()
	{
		return glfwWindow;
	}
}