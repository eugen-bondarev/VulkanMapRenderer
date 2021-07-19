#include "time.h"

namespace Engine
{
	namespace Time
	{
		float lastTime 		{ 0 };
		float currentTime 	{ 0 };
		float deltaTime 	{ 0 };

		std::vector<float> fpsMeasurements;
		float sum = 0;
		int amount = 0;
	
		void BeginMeasurement()
		{
			currentTime = glfwGetTime();
		}

		void EndMeasurement()
		{
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			sum += GetFPS();
			amount++;
		}

		float GetFPS()
		{
			return 1.0f / deltaTime;
		}
		
		float GetAverageFPS()
		{
			return sum / amount;
		}
	}
}