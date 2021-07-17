#include "pipeline.h"

#include "../device/device.h"
#include "../swap_chain/swap_chain.h"

namespace Engine
{
	namespace Vk
	{
		Pipeline::Pipeline(
			const std::string &vs_code,
			const std::string &fs_code,
			const glm::vec2 &viewport_size,
			VkFormat image_format,
			const BindingDescriptions &binding_descriptions,
			const AttributeDescriptions &attribute_descriptions,
			const SetLayouts &set_layouts)
		{
			shader = new Shader(vs_code, fs_code);

			VkPipelineVertexInputStateCreateInfo vertex_input_info{};
			vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

			vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(binding_descriptions.size());
			vertex_input_info.pVertexBindingDescriptions = binding_descriptions.data();

			vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
			vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

			VkPipelineInputAssemblyStateCreateInfo input_assembly{};
			input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			input_assembly.primitiveRestartEnable = VK_FALSE;

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = viewport_size.x;
			viewport.height = viewport_size.y;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor{};
			scissor.offset = {0, 0};
			scissor.extent = {static_cast<uint32_t>(viewport_size.x), static_cast<uint32_t>(viewport_size.y)};

			VkPipelineViewportStateCreateInfo viewport_state{};
			viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewport_state.viewportCount = 1;
			viewport_state.pViewports = &viewport;
			viewport_state.scissorCount = 1;
			viewport_state.pScissors = &scissor;

			VkPipelineRasterizationStateCreateInfo rasterizer{};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;
			rasterizer.depthBiasConstantFactor = 0.0f; // Optional
			rasterizer.depthBiasClamp = 0.0f;		   // Optional
			rasterizer.depthBiasSlopeFactor = 0.0f;	   // Optional

			VkPipelineMultisampleStateCreateInfo multisampling{};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampling.minSampleShading = 1.0f;			// Optional
			multisampling.pSampleMask = nullptr;			// Optional
			multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
			multisampling.alphaToOneEnable = VK_FALSE;		// Optional

			VkPipelineColorBlendAttachmentState color_blend_attachment{};
			color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			color_blend_attachment.blendEnable = VK_FALSE;
			color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
			color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;			   // Optional
			color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
			color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;			   // Optional

			VkPipelineColorBlendStateCreateInfo color_blending{};
			color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			color_blending.logicOpEnable = VK_FALSE;
			color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
			color_blending.attachmentCount = 1;
			color_blending.pAttachments = &color_blend_attachment;
			color_blending.blendConstants[0] = 0.0f; // Optional
			color_blending.blendConstants[1] = 0.0f; // Optional
			color_blending.blendConstants[2] = 0.0f; // Optional
			color_blending.blendConstants[3] = 0.0f; // Optional

			VkDynamicState dynamicStates[] = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_LINE_WIDTH};

			VkPipelineDynamicStateCreateInfo dynamic_state{};
			dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamic_state.dynamicStateCount = 2;
			dynamic_state.pDynamicStates = dynamicStates;

			VkPipelineLayoutCreateInfo pipeline_layout_info{};
			pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size()); // Optional
			pipeline_layout_info.pSetLayouts = set_layouts.data();							 // Optional
			pipeline_layout_info.pushConstantRangeCount = 0;								 // Optional
			pipeline_layout_info.pPushConstantRanges = nullptr;								 // Optional

			VK_CHECK(vkCreatePipelineLayout(Global::device->GetVkDevice(), &pipeline_layout_info, nullptr, &vkPipelineLayout), "Failed to create pipeline layout.");

			renderPass = new RenderPass(image_format);

			VkGraphicsPipelineCreateInfo pipeline_info{};
			pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipeline_info.stageCount = static_cast<uint32_t>(shader->GetStages().size());
			pipeline_info.pStages = shader->GetStages().data();
			pipeline_info.pVertexInputState = &vertex_input_info;
			pipeline_info.pInputAssemblyState = &input_assembly;
			pipeline_info.pViewportState = &viewport_state;
			pipeline_info.pRasterizationState = &rasterizer;
			pipeline_info.pMultisampleState = &multisampling;
			pipeline_info.pDepthStencilState = nullptr; // Optional
			pipeline_info.pColorBlendState = &color_blending;
			pipeline_info.pDynamicState = nullptr; // Optional
			pipeline_info.layout = vkPipelineLayout;
			pipeline_info.renderPass = renderPass->GetVkRenderPass();
			pipeline_info.subpass = 0;
			pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
			pipeline_info.basePipelineIndex = -1;			   // Optional

			VK_CHECK(vkCreateGraphicsPipelines(Global::device->GetVkDevice(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &vkPipeline), "Failed to create graphics pipeline.");

			TRACE();
		}

		Pipeline::~Pipeline()
		{
			vkDestroyPipeline(Global::device->GetVkDevice(), vkPipeline, nullptr);
			delete renderPass;
			vkDestroyPipelineLayout(Global::device->GetVkDevice(), vkPipelineLayout, nullptr);
			delete shader;

			TRACE();
		}

		void Pipeline::SetAsOutput()
		{
			Global::swapChain->InitFramebuffers(renderPass->GetVkRenderPass());
		}

		VkPipelineLayout Pipeline::GetVkPipelineLayout() const
		{
			return vkPipelineLayout;
		}

		VkPipeline Pipeline::GetVkPipeline() const
		{
			return vkPipeline;
		}

		RenderPass *Pipeline::GetRenderPass()
		{
			return renderPass;
		}
	}
}