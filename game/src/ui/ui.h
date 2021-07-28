#pragma once

#include <functional>

#include "imgui/imgui.h"

namespace UI
{
	void ClearStack();
	void Push(const std::function<void()>& function);
	void ExecuteStack();
}

#define UI_PUSH(x)\
	::UI::Push([&]()\
	{\
		x\
	})