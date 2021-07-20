#pragma once

#include <thread>

namespace Engine
{
	namespace Util
	{
		namespace CPU
		{
			inline static const int AMOUNT_OF_CORES = std::thread::hardware_concurrency();
		}
	}
}