#include "tasks.h"

#include <vector>

namespace Engine
{
	namespace Tasks
	{
		std::vector<std::function<void()>> stack;

		void Push(const std::function<void()>& function)
		{
			stack.push_back(function);
		}

		void ExecuteStack()
		{
			for (auto& task : stack)
				task();

			stack.clear();
		}		
	}
}