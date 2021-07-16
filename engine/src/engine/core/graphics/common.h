#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <assert.h>

#include <spdlog/spdlog.h>

#define VK_ASSERT(exp, msg) assert((exp) && msg)

#define LOG_OUT(...)		spdlog::info(__VA_ARGS__)
#define ERR_OUT(...)		spdlog::error(__VA_ARGS__)
#define WARN_OUT(...)		spdlog::warn(__VA_ARGS__)
#define TRACE()				LOG_OUT("[Call]: " + std::string(__func__))

#define THROW(...)					\
	ERR_OUT(__VA_ARGS__);			\
	throw std::runtime_error("")

#define VK_CHECK(x, msg)			\
	if (x != VK_SUCCESS)			\
	{								\
		THROW(msg);					\
	}