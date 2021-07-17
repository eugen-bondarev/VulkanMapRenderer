#pragma once

#include "../../common.h"

#include "shader.h"
#include "render_pass.h"

#include "vertex_layouts/layouts.h"

namespace Engine
{
	namespace Vk
	{
		// std::vector<VkVertexInputBindingDescription>
		// std::vector<VkVertexInputAttributeDescription>

		using BindingDescriptions = std::vector<VkVertexInputBindingDescription>;
		using AttributeDescriptions = std::vector<VkVertexInputAttributeDescription>;
		using SetLayouts = std::vector<VkDescriptorSetLayout>;

		class Pipeline
		{
		public:
			Pipeline(
				const std::string &vs_code,
				const std::string &fs_code,
				const glm::vec2 &viewport_size,
				VkFormat image_format,
				const BindingDescriptions &binding_descriptions = {},
				const AttributeDescriptions &attribute_descriptions = {},
				const SetLayouts &set_layouts = {});

			~Pipeline();

			VkPipelineLayout GetVkPipelineLayout() const;
			VkPipeline GetVkPipeline() const;
			const RenderPass *GetRenderPass() const;

		private:
			Shader *shader;
			RenderPass *renderPass;

			VkPipelineLayout vkPipelineLayout;
			VkPipeline vkPipeline;

			Pipeline(const Pipeline &) = delete;
			Pipeline &operator=(const Pipeline &) = delete;
		};
	}
}