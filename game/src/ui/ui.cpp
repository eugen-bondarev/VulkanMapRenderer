#include "ui.h"

namespace UI
{
	std::vector<std::function<void()>> stack;

	void ClearStack()
	{
		stack.clear();
	}

	void AddToStack(const std::function<void()>& function)
	{
		stack.push_back(function);
	}

	void ExecuteStack()
	{
		for (auto& function : stack)
			function();
	}
}