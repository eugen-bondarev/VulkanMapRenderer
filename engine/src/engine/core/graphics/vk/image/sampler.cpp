#include "sampler.h"

#include "../device/device.h"

namespace Engine
{
	namespace Vk
	{
		namespace Global
		{
			Sampler* constantInterpolationSampler;
			Sampler* linearInterpolationSampler;
		}

		Sampler::Sampler(VkFilter filter)
		{
			VkSamplerCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			create_info.magFilter = filter;
			create_info.minFilter = filter;
			create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			create_info.anisotropyEnable = VK_TRUE;
			create_info.maxAnisotropy = Global::device->properties.limits.maxSamplerAnisotropy;
			create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

			create_info.unnormalizedCoordinates = VK_FALSE;

			create_info.compareEnable = VK_FALSE;
			create_info.compareOp = VK_COMPARE_OP_ALWAYS;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			create_info.mipLodBias = 0.0f;
			create_info.minLod = 0.0f;
			create_info.maxLod = 0.0f;

			VT_CHECK(vkCreateSampler(Global::device->GetVkDevice(), &create_info, nullptr, &vkSampler));
			VT_TRACE();
		}

		Sampler::~Sampler()
		{
    		vkDestroySampler(Global::device->GetVkDevice(), vkSampler, nullptr);
			VT_TRACE();
		}

		VkSampler& Sampler::GetVkSampler()
		{
			return vkSampler;
		}
	}
}