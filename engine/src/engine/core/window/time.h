#pragma once

#include "../../common.h"

namespace Engine
{
	namespace Time
	{
		void BeginMeasurement();
		void EndMeasurement();
		float GetDelta();
		float GetFPS();
		float GetAverageFPS();
	}
}