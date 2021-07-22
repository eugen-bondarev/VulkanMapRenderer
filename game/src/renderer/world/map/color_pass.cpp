#include "color_pass.h"

namespace Offscreen
{	
	ColorPass::ColorPass(const std::shared_ptr<Game>& game, Engine::Vk::DescriptorPool* descriptor_pool) : game { game }
	{
		using namespace Engine;

		// Creating descriptor set layout (for the pipeline)
		std::vector<VkDescriptorSetLayoutBinding> bindings = 
		{
			Vk::CreateBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER),
			Vk::CreateBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		};

		descriptorSetLayout = std::make_shared<Vk::DescriptorSetLayout>(bindings);

		const Assets::Text vs_code("assets/shaders/default.vert.spv");
		const Assets::Text fs_code("assets/shaders/default.frag.spv");

		Vk::AttachmentDescriptions attachments = { Vk::Util::CreateAttachment(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) };
		Vk::BindingDescriptions binding_descriptors = Util::Vector::Merge(Vk::Vertex::GetBindingDescriptions(), Vk::PerInstanceVertex::GetBindingDescriptions());
		Vk::AttributeDescriptions attribute_descriptors = Util::Vector::Merge(Vk::Vertex::GetAttributeDescriptions(), Vk::PerInstanceVertex::GetAttributeDescriptions());
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts = { descriptorSetLayout->GetVkDescriptorSetLayout() };

		pipeline = std::make_shared<Vk::Pipeline>(
			vs_code.GetContent(), fs_code.GetContent(), 
			Util::Math::ExtentToVec2(Vk::Global::swapChain->GetExtent()),
			attachments,
			binding_descriptors, 
			attribute_descriptors,
			descriptor_set_layouts
		);

		texture = std::make_shared<Vk::Texture2D>(
			window->GetSize(), 
			4, 
			nullptr, 
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
		);

		framebuffer = std::make_shared<Vk::Framebuffer>(
			texture->GetImageView()->GetVkImageView(), 
			pipeline->GetRenderPass()->GetVkRenderPass(), 
			window->GetSize()
		);

		// Creating a uniform buffer (Scene scope)
		ubo.perScene.buffer = std::make_shared<Vk::Buffer>(
			sizeof(UBOScene),
			1,
			&ubo.perScene.data,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
		);

		// Load image
		const Assets::Image map_texture("assets/textures/map.png");

		block.tileMap = TextureAtlas::Add<BlocksTileMap>(TextureAtlasType::Map, std::make_shared<BlocksTileMap>(
			glm::vec2(8.0f),
			map_texture.GetSize(),
			map_texture.GetAmountOfChannels(),
			map_texture.GetData()
		));

		block.tileMap->Add(BlockType::Dirt, glm::vec2(1, 1));
		block.tileMap->Add(BlockType::Grass, glm::vec2(1, 7));
		block.tileMap->Add(BlockType::Stone, glm::vec2(7, 1));
		block.tileMap->Add(BlockType::Wood, glm::vec2(13, 1));

		// Creating descriptor set
		descriptorSet = std::make_shared<Vk::DescriptorSet>(
			descriptor_pool, 
			std::vector<VkDescriptorSetLayout> { descriptorSetLayout->GetVkDescriptorSetLayout() }
		);

		std::vector<VkWriteDescriptorSet> offscreen_write_descriptor_sets = 
		{
			Vk::CreateWriteDescriptorSet(descriptorSet.get(), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, &ubo.perScene.buffer->GetDescriptor()),
			Vk::CreateWriteDescriptorSet(descriptorSet.get(), 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, &block.tileMap->GetImageView()->GetDescriptor())
		};

		descriptorSet->Update(offscreen_write_descriptor_sets);
		
		// Dynamic buffer for blocks' positions
		dynamicVertexBuffer = std::make_shared<Vk::Buffer>(
			sizeof(Vk::PerInstanceVertex), 
			game->map->GetAmountOfBlocks(), 
			nullptr, 
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);

		{			
			const std::vector<Vk::Vertex> vertices = 
			{
				{{-8.0f, -8.0f}, { 0.0f, 0.0f }},
				{{ 8.0f, -8.0f}, { 1.0f, 0.0f }},
				{{ 8.0f,  8.0f}, { 1.0f, 1.0f }},
				{{-8.0f,  8.0f}, { 0.0f, 1.0f }}
			};

			Vk::Buffer staging_buffer(vertices);
			block.vertexBuffer = std::make_shared<Vk::Buffer>(&staging_buffer);
		}
		{
			const std::vector<uint16_t> indices = 
			{
				0, 1, 2, 2, 3, 0
			};

			Vk::Buffer staging_buffer(indices);
			block.indexBuffer = std::make_shared<Vk::Buffer>(&staging_buffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
		}
	}	

	void ColorPass::WriteToCmd(Engine::Vk::CommandBuffer* cmd)
	{
		cmd->BeginRenderPass(pipeline->GetRenderPass(), framebuffer.get());
			cmd->BindPipeline(pipeline.get());
				cmd->BindVertexBuffers({ block.vertexBuffer.get(), dynamicVertexBuffer.get() }, { 0, 0 });
				cmd->BindIndexBuffer(block.indexBuffer.get());
					cmd->BindDescriptorSets(pipeline.get(), 1, &descriptorSet->GetVkDescriptorSet());
					cmd->DrawIndexed(block.indexBuffer->GetAmountOfElements(), game->map->GetAmountOfBlocks(), 0, 0, 0);
		cmd->EndRenderPass();
	}

	void ColorPass::UpdateBlocks(const std::vector<glm::vec4>& render_data)
	{
		VT_PROFILER_NAMED_SCOPE("Update buffer");

		dynamicVertexBuffer->Update(render_data.data(), static_cast<uint32_t>(sizeof(glm::vec4) * game->map->GetAmountOfBlocks()));
	}

	void ColorPass::UpdateSpace()
	{
		ubo.perScene.buffer->Update(&game->camera.GetProjectionViewMatrix());
	}
	
	VkDescriptorImageInfo& ColorPass::GetOutputDescriptorImageInfo()
	{
		return texture->GetImageView()->GetDescriptor();
	}
}