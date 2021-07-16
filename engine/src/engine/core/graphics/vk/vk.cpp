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
		}

		void Shutdown()
		{
			delete Global::device;
			delete Global::surface;
			delete Global::instance;
		}
	}
}