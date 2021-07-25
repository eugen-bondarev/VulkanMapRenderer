#include "map_renderer.h"

#include <engine/engine.h>

#include "renderer/atlas/texture_atlas.h"
#include "tiles.h"

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
	lightPass = new Offscreen::LightPass(map, camera, descriptor_pool);
	composition = new Composition(
		descriptor_pool, 
		colorPass->GetOutputDescriptorImageInfo(),
		lightPass->GetOutputDescriptorImageInfo()
	);
	
	for (int i = 0; i < Vk::Global::swapChain->GetImageViews().size(); i++)
	{
		Vk::CommandPool* pool = new Vk::CommandPool();
		commandBuffers.push_back(new Vk::CommandBuffer(pool));
		commandPools.push_back(pool);
	}

	GetRenderData(map, camera->GetPosition(), blocks_to_render, lights_to_render);

	colorPass->UpdateBlocks(blocks_to_render);
	lightPass->UpdateBlocks(lights_to_render);

	for (int i = 0; i < Vk::Global::swapChain->GetImageViews().size(); i++)
	{
		Vk::CommandPool* pool = commandPools[i];
		Vk::CommandBuffer* cmd = commandBuffers[i];
		Vk::Framebuffer* framebuffer = Vk::Global::swapChain->GetFramebuffers()[i];

		pool->Reset();
			cmd->Begin();
				colorPass->WriteToCmd(cmd, blocks_to_render.size());
				lightPass->WriteToCmd(cmd, lights_to_render.size());
				composition->WriteToCmd(cmd, framebuffer);
			cmd->End();
	}
}

MapRenderer::~MapRenderer()
{
	delete composition;
	delete lightPass;
	delete colorPass;

	for (int i = 0; i < commandBuffers.size(); i++)
	{
		delete commandBuffers[i];
		delete commandPools[i];
	}
}

void MapRenderer::FillCommandBuffers()
{
	// for (int i = 0; i < commandBuffers.size(); i++)
	// {
	// 	Vk::CommandBuffer* cmd = commandBuffers[i];
	// 	Vk::Framebuffer* framebuffer = Vk::Global::swapChain->GetFramebuffers()[i];

	// 	cmd->Begin();
	// 		colorPass->WriteToCmd(cmd);
	// 		lightPass->WriteToCmd(cmd);
	// 		composition->WriteToCmd(cmd, framebuffer);
	// 	cmd->End();
	// }
}

void MapRenderer::Update()
{
	blocks_to_render.clear();
	lights_to_render.clear();

	GetRenderData(map, camera->GetPosition(), blocks_to_render, lights_to_render);

	colorPass->UpdateBlocks(blocks_to_render);	
	lightPass->UpdateBlocks(lights_to_render);
}

void MapRenderer::UpdateSpace()
{
	colorPass->UpdateSpace();
	lightPass->UpdateSpace();
}

void Async_GetRenderData(Map* map, glm::vec2 view_position, std::vector<glm::vec4>& blocks_to_render, std::vector<glm::vec2>& lights_to_render, BlocksTileMap* tile_map, int start, int end)
{
	const auto& blocks = map->GetBlocks();

	BlockType last_type = BlockType::Empty;
	TileFunction function;
	glm::vec2 base_tile;

	for (int x = start; x < end; x++)
	{
		for (int y = 0; y < blocks[0].size(); y++)
		{
			if (blocks[x][y].type == BlockType::Empty)
			{
 				if ((y + 1 < blocks[0].size() && blocks[x][y + 1].type != BlockType::Empty) || (y - 1 >= 0 && blocks[x][y - 1].type != BlockType::Empty))
				{
					lights_to_render.push_back(blocks[x][y].worldPosition);
				}
			}
		}
	}

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

				blocks_to_render.emplace_back(blocks[x][y].worldPosition, block_texture_tile);

				last_type = blocks[x][y].type;
			}
		}
	}
}

/*
* Note: 
*	Parallelism doesn't work well here
*/
void MapRenderer::GetRenderData(Map* map, glm::vec2 view_position, std::vector<glm::vec4>& blocks_to_render, std::vector<glm::vec2>& lights_to_render)
{
	VT_PROFILER_SCOPE();

	BlocksTileMap* blocks_tile_map = TextureAtlas::Get<BlocksTileMap>(TextureAtlasType::Map);

	const auto& blocks = map->GetBlocks();

	blocks_to_render.reserve(blocks.size() * blocks[0].size());
	lights_to_render.reserve(300);

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
		std::execution::par,
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
	Vk::CommandBuffer* cmd = GetCurrentCmdBuffer();

	if (updateCmdBuffers)
	{
		Vk::Framebuffer* framebuffer = Vk::Global::swapChain->GetCurrentScreenFramebuffer();
		Vk::CommandPool* pool = GetCurrentCmdPool();

		pool->Reset();
			cmd->Begin();
				colorPass->WriteToCmd(cmd, blocks_to_render.size());
				lightPass->WriteToCmd(cmd, lights_to_render.size());
				composition->WriteToCmd(cmd, framebuffer);
			cmd->End();

		updateCmdBuffers = false;
	}

	vkResetFences(Vk::Global::device->GetVkDevice(), 1, &fence);
	cmd->SubmitToQueue(Vk::Global::Queues::graphicsQueue, wait, signal, fence);
}

Engine::Vk::CommandPool* MapRenderer::GetCurrentCmdPool()
{
	uint32_t image_index = Vk::Global::swapChain->GetCurrentImageIndex(); 
	return commandPools[image_index];
}

Engine::Vk::CommandBuffer* MapRenderer::GetCurrentCmdBuffer()
{
	uint32_t image_index = Vk::Global::swapChain->GetCurrentImageIndex(); 
	return commandBuffers[image_index];
}

void MapRenderer::UpdateCmdBuffers()
{
	updateCmdBuffers = true;
}

std::vector<Vk::CommandBuffer*>& MapRenderer::GetCommandBuffers()
{
	return commandBuffers;
}