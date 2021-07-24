#include "map_renderer.h"

#include <engine/engine.h>

#include "renderer/world/map/tiles.h"
#include "renderer/atlas/texture_atlas.h"

#include <future>
#include <execution>

#include <glm/gtx/norm.hpp>

using namespace Engine;

MapRenderer::MapRenderer()
{

}

void MapRenderer::Init(Map* map, Camera* camera, Vk::DescriptorPool* descriptor_pool)
{
	this->map = map;
	this->camera = camera;

	colorPass = new Offscreen::ColorPass(map, camera, descriptor_pool);
	composition = new Composition(descriptor_pool, colorPass->GetOutputDescriptorImageInfo());
	
	for (int i = 0; i < Vk::Global::swapChain->GetImageViews().size(); i++)
		commandBuffers.push_back(new Vk::CommandBuffer(Vk::Global::commandPool));
}

MapRenderer::~MapRenderer()
{
	delete composition;
	delete colorPass;

	for (int i = 0; i < commandBuffers.size(); i++)
		delete commandBuffers[i];
}

void MapRenderer::FillCommandBuffers()
{	
	for (int i = 0; i < commandBuffers.size(); i++)
	{
		Vk::CommandBuffer* cmd = commandBuffers[i];
		Vk::Framebuffer* framebuffer = Vk::Global::swapChain->GetFramebuffers()[i];

		cmd->Begin();
			colorPass->WriteToCmd(cmd);
			composition->WriteToCmd(cmd, framebuffer);
		cmd->End();
	}
}

void MapRenderer::Update()
{
	std::vector<glm::vec4> blocks_to_render;
	std::vector<glm::vec2> lights_to_render;
	GetRenderData(map, camera->GetPosition(), blocks_to_render, lights_to_render);
	colorPass->UpdateBlocks(blocks_to_render);
}

void MapRenderer::UpdateSpace()
{
	colorPass->UpdateSpace();
}

static bool BlockIsLit(glm::vec2 block_position, const std::vector<glm::vec2>& lights)
{
	for (int i = 0; i < lights.size(); i++)
	{
		if (glm::distance2(block_position, lights[i]) < 5000.0f)
		{
			return true;
		}
	}

	return false;
}

static bool dist(glm::vec2 l_0, glm::vec2 l_1)
{
	return glm::distance2(l_0, l_1) < 10.0f;
}

static bool dist1(glm::vec2 l_0, std::vector<glm::vec2>& lights)
{
	for (const auto light : lights)
	{
		if (dist(l_0, light))
			return false;
	}

	return true;
}

void Async_GetRenderData(Map* map, glm::vec2 view_position, std::vector<glm::vec4>& blocks_to_render, std::vector<glm::vec2>& lights_to_render, BlocksTileMap* tile_map, int start, int end)
{
	const auto& blocks = map->GetBlocks();

	BlockType last_type = BlockType::Empty;
	TileFunction function;
	glm::vec2 base_tile;

	std::vector<glm::vec2> light_indices;

	for (int x = start; x < end; x++)
	{
		for (int y = 0; y < blocks[0].size(); y++)
		{
			if (blocks[x][y].type == BlockType::Empty)
			{
 				if ((y + 1 < blocks[0].size() && blocks[x][y + 1].type != BlockType::Empty) || 
				 	(y - 1 >= 0 && blocks[x][y - 1].type != BlockType::Empty))
				{
					if (light_indices.size() == 0 || dist1(glm::vec2(x, y), light_indices))
					{
						lights_to_render.push_back(blocks[x][y].worldPosition);
						light_indices.emplace_back(x, y);
					}
				}
			}
		}
	}
	
	// for (int i = 0; i < light_indices.size(); i++)
	// {
	// 	glm::vec2 index = light_indices[i];

	// 	for (int _x = -5; _x < 5; _x++)
	// 	{
	// 		for (int _y = -5; _y < 5; _y++)
	// 		{
	// 			int x = std::max<int>(index.x + _x, 0);
	// 			int y = std::max<int>(index.y + _y, 0);
	// 			x = std::min<int>(x, blocks.size() - 1);
	// 			y = std::min<int>(y, blocks[0].size() - 1);

	// 			if (blocks[x][y].type != BlockType::Empty)
	// 			{
	// 				if (blocks[x][y].type != last_type)
	// 				{
	// 					function = PickTileFunction(blocks[x][y].type);
	// 					base_tile = tile_map->Get(blocks[x][y].type);
	// 				}

	// 				const glm::vec2 block_texture_tile = base_tile + function(blocks, x, y);

	// 				blocks_to_render.emplace_back(blocks[x][y].worldPosition, block_texture_tile);

	// 				last_type = blocks[x][y].type;
	// 			}
	// 		}
	// 	}
	// }

	for (int x = start; x < end; x++)
	{
		for (int y = 0; y < blocks[0].size(); y++)
		{
			if (blocks[x][y].type != BlockType::Empty)
			{
				// if (BlockIsLit(blocks[x][y].worldPosition, lights_to_render))
				{
					if (blocks[x][y].type != last_type)
					{
						function = PickTileFunction(blocks[x][y].type);
						base_tile = tile_map->Get(blocks[x][y].type);
					}

					const glm::vec2 block_texture_tile = base_tile + function(blocks, x, y);

					blocks_to_render.emplace_back(blocks[x][y].worldPosition, block_texture_tile);

					last_type = blocks[x][y].type;
				}
			}
		}
	}
}

/*
* Note: parallelism doesn't work well here
*/
void MapRenderer::GetRenderData(Map* map, glm::vec2 view_position, std::vector<glm::vec4>& blocks_to_render, std::vector<glm::vec2>& lights_to_render)
{
	VT_PROFILER_SCOPE();

	BlocksTileMap* blocks_tile_map = TextureAtlas::Get<BlocksTileMap>(TextureAtlasType::Map);

	const auto& blocks = map->GetBlocks();

	blocks_to_render.reserve(blocks.size() * blocks[0].size());

// #define ASYNC

#ifdef ASYNC
	int length = blocks.size();
	static int cores_to_use = 128;
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
			Async_GetRenderData(map, view_position, blocks_to_render, lights_to_render, blocks_tile_map, interval.x, interval.y);
		}
	);
#else
	Async_GetRenderData(map, view_position, blocks_to_render, lights_to_render, blocks_tile_map, 0, blocks.size());
#endif
}

void MapRenderer::Render(Vk::Frame* frame)
{	
	VT_PROFILER_SCOPE();

	VkSemaphore* wait = &frame->GetImageAvailableSemaphore();
	VkSemaphore* signal = &frame->GetRenderFinishedSemaphore();
	VkFence fence = frame->GetInFlightFence();
	Vk::CommandBuffer* cmd = GetCurrentCmd();

	vkResetFences(Vk::Global::device->GetVkDevice(), 1, &fence);
	cmd->SubmitToQueue(Vk::Global::Queues::graphicsQueue, wait, signal, fence);
}

std::vector<Vk::CommandBuffer*>& MapRenderer::GetCommandBuffers()
{
	return commandBuffers;
}

Engine::Vk::CommandBuffer* MapRenderer::GetCurrentCmd()
{
	uint32_t image_index = Vk::Global::swapChain->GetCurrentImageIndex(); 
	return commandBuffers[image_index];
}