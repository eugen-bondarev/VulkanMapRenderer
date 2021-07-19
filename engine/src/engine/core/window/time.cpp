#include "time.h"

namespace Engine
{
	namespace Time
	{
		float lastTime 		{ 0 };
		float currentTime 	{ 0 };
		float deltaTime 	{ 0 };

		struct
		{
			float sum = 0;
			int amount = 0;
		} fps;
	
		void BeginMeasurement()
		{
			currentTime = glfwGetTime();
		}

		void EndMeasurement()
		{
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			fps.sum += GetFPS();
			fps.amount++;
		}

		float GetFPS()
		{
			return 1.0f / deltaTime;
		}
		
		float GetAverageFPS()
		{
			return fps.sum / fps.amount;
		}
	}
}