#pragma once

#include "../../common.h"

#include <functional>

namespace Engine
{
	namespace Time
	{
		void BeginMeasurement();
		void EndMeasurement();

		void AddTimer(const std::function<void()>& task, float timer, bool repetitive);

		float GetDelta();
		float GetFPS();
		float GetAverageFPS();
	}
}