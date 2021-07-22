#include "composition.h"

#include "color_pass.h"

Composition::Composition(Engine::Vk::DescriptorPool* descriptor_pool, VkDescriptorImageInfo& color_pass_output_descriptor_info)
{
	using namespace Engine;

	// Creating descriptor set layout (for the pipeline)
	std::vector<VkDescriptorSetLayoutBinding> bindings = 
	{
		Vk::CreateBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	};

	descriptorSetLayout = std::make_shared<Vk::DescriptorSetLayout>(bindings);

	Vk::BindingDescriptions vertex_buffer_binding_descriptors = Vk::Vertex::GetBindingDescriptions();
	Vk::AttributeDescriptions vertex_buffer_attribute_descriptors = Vk::Vertex::GetAttributeDescriptions();

	// Creating pipeline
	const Assets::Text vs_code("assets/shaders/composition.vert.spv");
	const Assets::Text fs_code("assets/shaders/composition.frag.spv");

	pipeline = std::make_shared<Vk::Pipeline>(
		vs_code.GetContent(), fs_code.GetContent(), 
		Util::Math::ExtentToVec2(Vk::Global::swapChain->GetExtent()),
		std::vector<VkAttachmentDescription> { Vk::Util::CreateAttachment(
			Vk::Global::swapChain->GetImageFormat(), 
			VK_IMAGE_LAYOUT_UNDEFINED, 
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			/**
			 * NOTE: Get rid of this if I don't render ImGui above.
			 */
		) },
		vertex_buffer_binding_descriptors, vertex_buffer_attribute_descriptors,
		std::vector<VkDescriptorSetLayout> { descriptorSetLayout->GetVkDescriptorSetLayout() }
	);

	// Swap chain's framebuffers will use its output
	pipeline->SetAsOutput();

	// Loading the block's vertex & index buffers. I created a scope in order for it to free the staging buffer automatically.
	{			
		const std::vector<Vk::Vertex> vertices = 
		{
			{{-1.0f, -1.0f}, { 0.0f, 0.0f }},
			{{ 1.0f, -1.0f}, { 1.0f, 0.0f }},
			{{ 1.0f,  1.0f}, { 1.0f, 1.0f }},
			{{-1.0f,  1.0f}, { 0.0f, 1.0f }}
		};

		Vk::Buffer staging_buffer(vertices);
		canvas.vertexBuffer = std::make_shared<Vk::Buffer>(&staging_buffer);
	}
	{
		const std::vector<uint16_t> indices = 
		{
			0, 1, 2, 2, 3, 0
		};

		Vk::Buffer staging_buffer(indices);
		canvas.indexBuffer = std::make_shared<Vk::Buffer>(&staging_buffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	}

	descriptorSet = std::make_shared<Vk::DescriptorSet>(
		descriptor_pool, 
		std::vector<VkDescriptorSetLayout> { descriptorSetLayout->GetVkDescriptorSetLayout() }
	);

	std::vector<VkWriteDescriptorSet> composition_write_descriptor_sets = 
	{
		Vk::CreateWriteDescriptorSet(
			descriptorSet.get(), 
			0, 
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER , 
			&color_pass_output_descriptor_info
		)
	};

	descriptorSet->Update(composition_write_descriptor_sets);		
}

void Composition::WriteToCmd(Engine::Vk::CommandBuffer* cmd, Engine::Vk::Framebuffer* framebuffer)
{
	cmd->BeginRenderPass(pipeline->GetRenderPass(), framebuffer);
		cmd->BindPipeline(pipeline.get());
			cmd->BindVertexBuffers({ canvas.vertexBuffer.get() }, { 0 });
			cmd->BindIndexBuffer(canvas.indexBuffer.get());
				cmd->BindDescriptorSets(pipeline.get(), 1, &descriptorSet->GetVkDescriptorSet());
				cmd->DrawIndexed(canvas.indexBuffer->GetAmountOfElements(), 1, 0, 0, 0);
	cmd->EndRenderPass();
}