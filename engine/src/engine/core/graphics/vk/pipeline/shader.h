#pragma once

#include "../../../../common.h"

namespace Engine
{
	namespace Vk
	{
		namespace Util
		{
			VkShaderModule CreateShaderModule(const std::string& code);
		}

		class Shader
		{
		public:
			Shader(const std::string& vs_code, const std::string& fs_code);
			~Shader();

			const std::array<VkShaderModule, 2>& GetModules() const;
			const std::array<VkPipelineShaderStageCreateInfo, 2>& GetStages() const;

		private:
			std::array<VkShaderModule, 2> modules;
			std::array<VkPipelineShaderStageCreateInfo, 2> stages;

			Shader(const Shader&) = delete;
			Shader& operator=(const Shader&) = delete;
		};
	}
}