#include "vk.h"

namespace Engine
{
	namespace Vk
	{
		void Bootstrap()
		{
			Global::instance = new Global::Instance();
			Global::surface = new Global::Surface();
			Global::device = new Global::Device();
			Global::swapChain = new Global::SwapChain();
			Global::commandPool = new CommandPool();
			Global::constantInterpolationSampler = new Sampler(VK_FILTER_NEAREST);
			Global::linearInterpolationSampler = new Sampler(VK_FILTER_LINEAR);
		}

		void Shutdown()
		{
			delete Global::constantInterpolationSampler;
			delete Global::linearInterpolationSampler;
			delete Global::commandPool;
			delete Global::swapChain;
			delete Global::device;
			delete Global::surface;
			delete Global::instance;
		}
	}
}