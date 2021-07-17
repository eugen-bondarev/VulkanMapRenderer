#pragma once

#include "../graphics/common.h"

namespace Engine
{
	namespace Time
	{
		extern float lastTime;
		extern float currentTime;
		extern float deltaTime;

		void BeginMeasurement();
		void EndMeasurement();
		float GetFPS();
	}
}