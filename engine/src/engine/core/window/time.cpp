#include "time.h"

#include <vector>

namespace Engine
{
	namespace Time
	{
		float lastTime 		{ 0 };
		float currentTime 	{ 0 };
		float deltaTime 	{ 0 };

		struct Task
		{
			std::function<void()> task;
			bool repetitive { false };
			float timer { 0.0f };
			float time { 0.0f };

			Task(const std::function<void()>& task, float timer, bool repetitive) : task { task }, timer { timer }, repetitive { repetitive }
			{

			}
		};

		std::vector<Task> tasks;

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

			for (auto& task : tasks)
			{
				task.time += deltaTime;
				if (task.time >= task.timer)
				{
					task.task();

					if (task.repetitive)
					{
						task.time = 0.0f;
					}
				}
			}
		}

		void AddTimer(const std::function<void()>& task, float timer, bool repetitive)
		{
			tasks.emplace_back(task, timer, repetitive);
		}

		float GetDelta()
		{
			return deltaTime;
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