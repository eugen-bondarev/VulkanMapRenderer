#pragma once

#include <functional>

#include "imgui/imgui.h"

namespace UI
{
	void ClearStack();
	void AddToStack(const std::function<void()>& function);
	void ExecuteStack();
}