#pragma once

#include "../../common.h"

namespace Engine
{
	namespace Vk
	{
		class Sampler
		{
		public:
			Sampler();
			~Sampler();

			VkSampler& GetVkSampler();

		private:
			VkSampler vkSampler;

			Sampler(const Sampler&) = delete;
			Sampler& operator=(const Sampler&) = delete;
		};
	}
}