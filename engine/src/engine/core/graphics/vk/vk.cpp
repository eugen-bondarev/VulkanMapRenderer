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
		}

		void Shutdown()
		{
			delete Global::swapChain;
			delete Global::device;
			delete Global::surface;
			delete Global::instance;
		}
	}
}