#pragma once

#include "../../common.h"

namespace Engine
{
	namespace Time
	{
		extern float deltaTime;

		void BeginMeasurement();
		void EndMeasurement();
		float GetFPS();
		float GetAverageFPS();
	}
}