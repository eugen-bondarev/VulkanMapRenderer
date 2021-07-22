#include "map_renderer.h"

#include <engine/engine.h>

#include "tiles.h"
#include "../../atlas/texture_atlas.h"

#include <future>
#include <execution>

namespace MapRenderer
{
	Offscreen* offscreen;

	Offscreen::Offscreen(const std::shared_ptr<Game>& game, Engine::Vk::DescriptorPool* descriptor_pool) : game { game }
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

	void Offscreen::WriteToCmd(Engine::Vk::CommandBuffer* cmd)
	{
		cmd->BeginRenderPass(pipeline->GetRenderPass(), framebuffer.get());
			cmd->BindPipeline(pipeline.get());
				cmd->BindVertexBuffers({ block.vertexBuffer.get(), dynamicVertexBuffer.get() }, { 0, 0 });
				cmd->BindIndexBuffer(block.indexBuffer.get());
					cmd->BindDescriptorSets(pipeline.get(), 1, &descriptorSet->GetVkDescriptorSet());
					cmd->DrawIndexed(block.indexBuffer->GetAmountOfElements(), game->map->GetAmountOfBlocks(), 0, 0, 0);
		cmd->EndRenderPass();
	}

	void Offscreen::UpdateBlocks()
	{
		std::vector<glm::vec4> render_data;
		GetRenderData(game->map.get(), game->camera.GetPosition(), render_data);

		VT_PROFILER_NAMED_SCOPE("Update buffer");

		dynamicVertexBuffer->Update(render_data.data(), static_cast<uint32_t>(sizeof(glm::vec4) * game->map->GetAmountOfBlocks()));
	}

	void Offscreen::UpdateSpace()
	{
		ubo.perScene.buffer->Update(&game->camera.GetProjectionViewMatrix());
	}

	VkDescriptorImageInfo& Offscreen::GetOutputDescriptorImageInfo()
	{
		return texture->GetImageView()->GetDescriptor();
	}

	void Async_GetRenderData(Map* map, glm::vec2 view_position, std::vector<glm::vec4>& data, BlocksTileMap* tile_map, int start, int end)
	{
		const auto& blocks = map->GetBlocks();

		BlockType last_type = BlockType::Empty;
		TileFunction function;
		glm::vec2 base_tile;

		for (int x = start; x < end; x++)
		{
			for (int y = 0; y < blocks[0].size(); y++)
			{
				if (blocks[x][y].type != BlockType::Empty)
				{
					if (blocks[x][y].type != last_type)
					{
						function = PickTileFunction(blocks[x][y].type);
						base_tile = tile_map->Get(blocks[x][y].type);
					}

					const glm::vec2 block_texture_tile = base_tile + function(blocks, x, y);

					data.emplace_back(blocks[x][y].worldPosition, block_texture_tile);

					last_type = blocks[x][y].type;
				}
			}
		}
	}

	/*
	* Note: parallelism doesn't work well here
	*/
	void GetRenderData(Map* map, glm::vec2 view_position, std::vector<glm::vec4>& data)
	{
		VT_PROFILER_SCOPE();

		BlocksTileMap *blocksTileMap = TextureAtlas::Get<BlocksTileMap>(TextureAtlasType::Map);

		const auto& blocks = map->GetBlocks();

		data.reserve(blocks.size() * blocks[0].size());

// #define ASYNC

#ifdef ASYNC
		int length = blocks.size();
		static int cores_to_use = 256;
		int task_length = cores_to_use - 1;	// TODO: Process the case when the value equals to 0.
		int full_fraction = (length - (length % task_length)) / task_length;
		int last_fraction = length - task_length * full_fraction;

		std::vector<glm::vec2> intervals;

		for (int i = 0; i < task_length; i++)
		{
			int start = i * full_fraction;
			int end = start + full_fraction;
			intervals.emplace_back(start, end);
		}

		int start = task_length * full_fraction;
		int end = start + last_fraction;
		intervals.emplace_back(start, end);

		std::for_each(
			std::execution::par_unseq,
			intervals.begin(),
			intervals.end(),
			[&](glm::vec2& interval) 
			{
				Async_GetRenderData(map, view_position, data, blocksTileMap, interval.x, interval.y);
			}
		);
#else
		Async_GetRenderData(map, view_position, data, blocksTileMap, 0, blocks.size());
#endif
	}
}