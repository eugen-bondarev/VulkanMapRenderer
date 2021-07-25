#include "light_pass.h"

namespace Offscreen
{	
	LightPass::LightPass(Map* map, Camera* camera, Engine::Vk::DescriptorPool* descriptor_pool) : map { map }, camera { camera }
	{
		using namespace Engine;

		// Creating descriptor set layout (for the pipeline)
		std::vector<VkDescriptorSetLayoutBinding> bindings = 
		{
			Vk::CreateBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
			Vk::CreateBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayout = std::make_shared<Vk::DescriptorSetLayout>(bindings);

		const Assets::Text vs_code("assets/shaders/light_pass.vert.spv");
		const Assets::Text fs_code("assets/shaders/light_pass.frag.spv");

		Vk::AttachmentDescriptions attachments = { Vk::Util::CreateAttachment(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) };
		Vk::BindingDescriptions binding_descriptors = Util::Vector::Merge(Vk::Vertex::GetBindingDescriptions(), Vk::PerInstanceVertex2D::GetBindingDescriptions());
		Vk::AttributeDescriptions attribute_descriptors = Util::Vector::Merge(Vk::Vertex::GetAttributeDescriptions(), Vk::PerInstanceVertex2D::GetAttributeDescriptions());
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts = { descriptorSetLayout->GetVkDescriptorSetLayout() };

		glm::vec2 light_pass_size = window->GetSize() / 16.0f;

		pipeline = std::make_shared<Vk::Pipeline>(
			vs_code.GetContent(), fs_code.GetContent(),
			light_pass_size,
			attachments,
			binding_descriptors, 
			attribute_descriptors,
			descriptor_set_layouts
		);

		texture = std::make_shared<Vk::Texture2D>(
			light_pass_size,
			4, 
			nullptr, 
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
		);

		texture->GetImageView()->SetupDescriptor(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Vk::Global::linearInterpolationSampler->GetVkSampler());

		framebuffer = std::make_shared<Vk::Framebuffer>(
			texture->GetImageView()->GetVkImageView(), 
			pipeline->GetRenderPass()->GetVkRenderPass(), 
			light_pass_size
		);

		// Creating a uniform buffer (Scene scope)
		ubo.perScene.buffer = std::make_shared<Vk::Buffer>(
			sizeof(UBOScene1),
			1,
			&ubo.perScene.data,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
		);

		// Load image
		const Assets::Image map_texture("assets/textures/light_mask_64.png");

		light.texture = std::make_shared<Vk::Texture2D>(
			map_texture.GetSize(),
			map_texture.GetAmountOfChannels(),
			map_texture.GetData()
		);

		light.texture->GetImageView()->SetupDescriptor(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, Vk::Global::linearInterpolationSampler->GetVkSampler());

		// Creating descriptor set
		descriptorSet = std::make_shared<Vk::DescriptorSet>(
			descriptor_pool, 
			std::vector<VkDescriptorSetLayout> { descriptorSetLayout->GetVkDescriptorSetLayout() }
		);

		std::vector<VkWriteDescriptorSet> offscreen_write_descriptor_sets = 
		{
			Vk::CreateWriteDescriptorSet(descriptorSet.get(), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &ubo.perScene.buffer->GetDescriptor()),
			Vk::CreateWriteDescriptorSet(descriptorSet.get(), 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &light.texture->GetImageView()->GetDescriptor())
		};

		descriptorSet->Update(offscreen_write_descriptor_sets);
		
		// Dynamic buffer for blocks' positions
		dynamicVertexBuffer = std::make_shared<Vk::Buffer>(
			sizeof(glm::vec2), 
			300, 
			nullptr, 
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);

		const std::vector<Vk::Vertex> vertices = 
		{
			{{-128.0f, -128.0f}, { 0.0f, 0.0f }},
			{{ 128.0f, -128.0f}, { 1.0f, 0.0f }},
			{{ 128.0f,  128.0f}, { 1.0f, 1.0f }},
			{{-128.0f,  128.0f}, { 0.0f, 1.0f }}
		};

		{			
			Vk::Buffer staging_buffer(vertices);
			light.vertexBuffer = std::make_shared<Vk::Buffer>(&staging_buffer);
		}

		const std::vector<uint16_t> indices = 
		{
			0, 1, 2, 2, 3, 0
		};

		{
			Vk::Buffer staging_buffer(indices);
			light.indexBuffer = std::make_shared<Vk::Buffer>(&staging_buffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
		}
	}	

	void LightPass::WriteToCmd(Engine::Vk::CommandBuffer* cmd, int instances)
	{
		cmd->BeginRenderPass(pipeline->GetRenderPass(), framebuffer.get());
			cmd->BindPipeline(pipeline.get());
				cmd->BindVertexBuffers({ light.vertexBuffer.get(), dynamicVertexBuffer.get() }, { 0, 0 });
				cmd->BindIndexBuffer(light.indexBuffer.get());
					cmd->BindDescriptorSets(pipeline.get(), 1, &descriptorSet->GetVkDescriptorSet());
					cmd->DrawIndexed(light.indexBuffer->GetAmountOfElements(), instances, 0, 0, 0);
		cmd->EndRenderPass();
	}

	void LightPass::UpdateBlocks(const std::vector<glm::vec2>& render_data)
	{
		VT_PROFILER_NAMED_SCOPE("Update buffer");
		dynamicVertexBuffer->Update(render_data.data(), static_cast<uint32_t>(sizeof(glm::vec2)) * render_data.size());
	}

	void LightPass::UpdateSpace()
	{
		ubo.perScene.buffer->Update(&camera->GetProjectionViewMatrix());
	}
	
	VkDescriptorImageInfo& LightPass::GetOutputDescriptorImageInfo()
	{
		return texture->GetImageView()->GetDescriptor();
	}
}