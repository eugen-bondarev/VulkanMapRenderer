#pragma once

#include "instance/instance.h"
#include "surface/surface.h"
#include "device/device.h"
#include "swap_chain/swap_chain.h"
#include "commands/command_pool.h"
#include "image/sampler.h"
#include "descriptors/descriptor_pool.h"

namespace Engine
{
	namespace Vk
	{
		void Bootstrap();
		void Shutdown();
	}
}