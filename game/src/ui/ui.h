#pragma once

#include <functional>

namespace UI
{
	void ClearStack();
	void AddToStack(const std::function<void()>& function);
	void ExecuteStack();
}