#include "vk.h"

namespace Engine
{
	namespace Vk
	{
		void Bootstrap()
		{
			Global::instance = new Global::Instance();
			// Global::device = new Global::Device();
		}

		void Shutdown()
		{
			delete Global::instance;
		}
	}
}