#pragma once

#include <functional>

namespace Engine
{
	namespace Tasks
	{
		void Push(const std::function<void()>& function);
		void ExecuteStack();
	}
}

#define VT_TASK_START()\
	::Engine::Tasks::Push([&]() { (void(0))

#define VT_TASK_END()\
	})
	
#define VT_TASK_PUSH(x)\
	::Engine::Tasks::Push([&]() { x })