#pragma once

#define NOMINMAX

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <algorithm>
#include <assert.h>
#include <string>
#include <vector>
#include <memory>
#include <array>

#include <spdlog/spdlog.h>

#include "util/profiling/profiling.h"
#include "util/memory/aligned.h"
#include "util/vector/vector.h"
#include "util/system/cpu.h"
#include "util/math/math.h"

#define VT_DISABLE_TRACE

#ifdef NDEBUG
#	define VT_RELEASE
#else
#	define VT_DEBUG
#endif

#define VT_ASSERT(exp) 				assert((exp))

#define VT_VOID_ASSEMBLY			(void(0))

#if !defined(VT_DISABLE_LOG_OUT) && !defined(VT_DISABLE_CONSOLE_OUT)
#	define VT_LOG_OUT(...)			spdlog::info(__VA_ARGS__)
#else
#	define VT_LOG_OUT(...)			VT_VOID_ASSEMBLY
#endif

#if !defined(VT_DISABLE_ERR_OUT) && !defined(VT_DISABLE_CONSOLE_OUT)
#	define VT_ERR_OUT(...)			spdlog::error(__VA_ARGS__)
#else
#	define VT_ERR_OUT(...)			VT_VOID_ASSEMBLY
#endif

#if !defined(VT_DISABLE_WARN_OUT) && !defined(VT_DISABLE_CONSOLE_OUT)
#	define VT_WARN_OUT(...)			spdlog::warn(__VA_ARGS__)
#else
#	define VT_WARN_OUT(...)			VT_VOID_ASSEMBLY
#endif

#if !defined(VT_DISABLE_VAR_OUT) && !defined(VT_DISABLE_CONSOLE_OUT)
#	define VT_VAR_OUT(VAR)			spdlog::info(#VAR ": {0}", VAR)
#else
#	define VT_VAR_OUT(VAR)			VT_VOID_ASSEMBLY
#endif

#ifdef VT_DEBUG
#	if !defined(VT_DISABLE_TRACE)
#		define VT_TRACE()				VT_LOG_OUT("[Call]: " + std::string(__func__))
#	else
#		define VT_TRACE()				VT_VOID_ASSEMBLY
#	endif

#	define VT_THROW(...)			\
	VT_ERR_OUT(__VA_ARGS__);		\
	throw std::runtime_error("")

#	define VT_CHECK(x)				\
	assert(x == VK_SUCCESS)						
#else
#	define VT_TRACE()				VT_VOID_ASSEMBLY
#	define VT_THROW(...)			VT_VOID_ASSEMBLY
#	define VT_CHECK(x)				x
#endif

#define ENUM_USE_FOR_FLAGS(T)\
inline T operator~ (T a) 			{ return static_cast<T>(~static_cast<int>(a)); }\
inline T operator| (T a, T b) 		{ return static_cast<T>(static_cast<int>(a) | static_cast<int>(b)); }\
inline T operator& (T a, T b) 		{ return static_cast<T>(static_cast<int>(a) & static_cast<int>(b)); }\
inline T operator^ (T a, T b) 		{ return static_cast<T>(static_cast<int>(a) ^ static_cast<int>(b)); }\
inline T& operator|= (T& a, T b) 	{ return reinterpret_cast<T&>(reinterpret_cast<int&>(a) |= static_cast<int>(b)); }\
inline T& operator&= (T& a, T b) 	{ return reinterpret_cast<T&>(reinterpret_cast<int&>(a) &= static_cast<int>(b)); }\
inline T& operator^= (T& a, T b) 	{ return reinterpret_cast<T&>(reinterpret_cast<int&>(a) ^= static_cast<int>(b)); }