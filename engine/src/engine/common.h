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
#include <memory>
#include <assert.h>

#include <spdlog/spdlog.h>

#include "util/profiling/profiling.h"
#include "util/memory/aligned.h"
#include "util/vector/vector.h"
#include "util/system/cpu.h"

#define VK_ASSERT(exp) assert((exp))

#define VOID_ASSEMBLY	(void(0))

#ifdef NDEBUG
#	define RELEASE
#else
#	define DEBUG
#endif

#define LOG_OUT(...)		spdlog::info(__VA_ARGS__)
#define ERR_OUT(...)		spdlog::error(__VA_ARGS__)
#define WARN_OUT(...)		spdlog::warn(__VA_ARGS__)
#define VAR_OUT(VAR)		spdlog::info(#VAR ": {0}", VAR)

#ifdef DEBUG
#	define TRACE()				LOG_OUT("[Call]: " + std::string(__func__))

#	define THROW(...)					\
		ERR_OUT(__VA_ARGS__);			\
		throw std::runtime_error("")

// #	define VK_CHECK(x, msg)				\
// 		if (x != VK_SUCCESS)			\
// 		{								\
// 			THROW(msg);					\
// 		}
#	define VK_CHECK(x, msg)				\
		assert(x == VK_SUCCESS)						
#else
#	define TRACE()				VOID_ASSEMBLY
#	define THROW(...)			VOID_ASSEMBLY
#	define VK_CHECK(x, msg)		x
// #	define VK_CHECK(x, msg)		x
#endif

inline static glm::vec2 ExtentToVec2(VkExtent2D extent)
{
	return { extent.width, extent.height };
}

inline static VkExtent2D ExtentToVec2(glm::vec2 vec)
{
	return { static_cast<uint32_t>(vec.x), static_cast<uint32_t>(vec.y) };
}

#define ENUM_USE_FOR_FLAGS(T)\
inline T operator~ (T a) 			{ return static_cast<T>(~static_cast<int>(a)); }\
inline T operator| (T a, T b) 		{ return static_cast<T>(static_cast<int>(a) | static_cast<int>(b)); }\
inline T operator& (T a, T b) 		{ return static_cast<T>(static_cast<int>(a) & static_cast<int>(b)); }\
inline T operator^ (T a, T b) 		{ return static_cast<T>(static_cast<int>(a) ^ static_cast<int>(b)); }\
inline T& operator|= (T& a, T b) 	{ return reinterpret_cast<T&>(reinterpret_cast<int&>(a) |= static_cast<int>(b)); }\
inline T& operator&= (T& a, T b) 	{ return reinterpret_cast<T&>(reinterpret_cast<int&>(a) &= static_cast<int>(b)); }\
inline T& operator^= (T& a, T b) 	{ return reinterpret_cast<T&>(reinterpret_cast<int&>(a) ^= static_cast<int>(b)); }