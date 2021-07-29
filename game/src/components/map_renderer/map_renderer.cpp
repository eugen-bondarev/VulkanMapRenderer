#include "map_renderer.h"

#include <engine/engine.h>

#include "renderer/atlas/texture_atlas.h"
#include "tiles.h"

#include <future>
#include <execution>

#include <glm/gtx/norm.hpp>

#include "sync/sync.h"

using namespace Engine;

MapRenderer::MapRenderer(Camera* camera, Map* map) : camera { camera }, map { map }
{
	colorPass = new Offscreen::ColorPass(map, camera, Vk::Global::descriptorPool);
	lightPass = new Offscreen::LightPass(map, camera, Vk::Global::descriptorPool);

	composition = new Composition(
		Vk::Global::descriptorPool, 
		colorPass->GetOutputDescriptorImageInfo(),
		lightPass->GetOutputDescriptorImageInfo()
	);
	
	for (int i = 0; i < Vk::Global::swapChain->GetImageViews().size(); i++)
	{
		Vk::CommandPool* pool = new Vk::CommandPool();
		commandBuffers.push_back(new Vk::CommandBuffer(pool));
		commandPools.push_back(pool);
	}

	GetRenderData(map);

	colorPass->UpdateBlocks(blocks_to_render);
	colorPass->UpdateWalls(walls_to_render);
	lightPass->UpdateBlocks(lights_to_render);

	for (int i = 0; i < Vk::Global::swapChain->GetImageViews().size(); i++)
	{
		Vk::CommandPool* pool = commandPools[i];
		Vk::CommandBuffer* cmd = commandBuffers[i];
		Vk::Framebuffer* framebuffer = Vk::Global::swapChain->GetFramebuffers()[i];

		pool->Reset();
			cmd->Begin();
				colorPass->WriteToCmd(cmd, walls_to_render.size(), blocks_to_render.size());
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

void MapRenderer::UpdateRenderData()
{
	walls_to_render.clear();
	blocks_to_render.clear();
	lights_to_render.clear();

	GetRenderData(map);

	colorPass->UpdateWalls(walls_to_render);
	colorPass->UpdateBlocks(blocks_to_render);	
	lightPass->UpdateBlocks(lights_to_render);
}

void MapRenderer::UpdateSpace()
{
	colorPass->UpdateSpace();
	lightPass->UpdateSpace();
}

void Async_GetRenderData(Map* map, std::vector<glm::vec4>& walls_to_render, std::vector<glm::vec4>& blocks_to_render, std::vector<glm::vec2>& lights_to_render, BlocksTileMap* tile_map, int start, int end)
{
	const auto& blocks = map->GetBlocks();
	const auto& walls = map->GetWalls();

	TileFunction function;
	glm::vec2 base_tile;

	for (int x = start; x < end; x++)
	{
		for (int y = 0; y < blocks[0].size(); y++)
		{
			if (blocks[x][y].type == BlockType::Empty)
			{
				if (walls[x][y].type != WallType::Empty)
				{
					if (y - 6 >= 0 && blocks[x][y - 6].type == BlockType::Empty && walls[x][y - 6].type == WallType::Empty)
					{
						lights_to_render.push_back(blocks[x][y].worldPosition);
					}				
				}
				else
				{
					if (y > 0 && (blocks[x][y + 1].type != BlockType::Empty || walls[x][y + 1].type != BlockType::Empty))
					{
						lights_to_render.push_back(blocks[x][y].worldPosition);
					}
				}
			}
		}
	}

	static glm::vec2 offset = {-4.0f, 4.0f};

	for (int x = start; x < end; x++)
	{
		for (int y = 0; y < blocks[0].size(); y++)
		{
			if (blocks[x][y].type != BlockType::Empty)
			{
				function = PickTileFunction(blocks[x][y].type);
				base_tile = tile_map->Get(blocks[x][y].type);

				const glm::vec2 block_texture_tile = base_tile + function(blocks, x, y);
				blocks_to_render.emplace_back(blocks[x][y].worldPosition, block_texture_tile);

				if (x + 1 < blocks.size() && x > 0 && y > 0 && y + 1 < blocks[0].size())
				{
					if (walls[x][y].type != WallType::Empty)
					{
						if (
							blocks[x][y].type != blocks[x - 1][y].type ||
							blocks[x][y].type != blocks[x][y + 1].type ||
							blocks[x][y].type != blocks[x - 1][y + 1].type
						)
						{
							function = PickTileFunction(walls[x][y].type);
							base_tile = tile_map->Get(walls[x][y].type == WallType::Grass ? WallType::Dirt : walls[x][y].type);							
							const glm::vec2 wall_texture_tile = base_tile + PickRightAngularWall(walls, x, y) + glm::vec2(3, 0);
							walls_to_render.emplace_back(walls[x][y].worldPosition + offset, wall_texture_tile);
						}
					}
				}				
			}
			else
			{
				if (walls[x][y].type != WallType::Empty)
				{
					function = PickTileFunction(walls[x][y].type);
					base_tile = tile_map->Get(walls[x][y].type == WallType::Grass ? WallType::Dirt : walls[x][y].type);

					const glm::vec2 wall_texture_tile = base_tile + PickRightAngularWall(walls, x, y) + glm::vec2(3, 0);
					walls_to_render.emplace_back(walls[x][y].worldPosition + offset, wall_texture_tile);
				}
			}
		}
	}
}

/*
* Note: 
*	Parallelism doesn't work well here
*/
void MapRenderer::GetRenderData(Map* map)
{
	VT_PROFILER_SCOPE();

	BlocksTileMap* blocks_tile_map = TextureAtlas::Get<BlocksTileMap>(TextureAtlasType::Map);

	const auto& walls = map->GetWalls();
	const auto& blocks = map->GetBlocks();

	walls_to_render.reserve(walls.size() * walls[0].size());
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
			Async_GetRenderData(map, walls_to_render, blocks_to_render, lights_to_render, blocks_tile_map, interval.x, interval.y);
		}
	);
#else
	Async_GetRenderData(map, walls_to_render, blocks_to_render, lights_to_render, blocks_tile_map, 0, blocks.size());
#endif
}

void MapRenderer::Update()
{
	if (camera->GetEvents() & CameraEvents_PositionChanged)
	{		
		UpdateSpace();

		map->CalculateVisibleChunks(camera->GetPosition());
		UpdateCmdBuffers();
		if (map->lastVisibleChunks.start != map->visibleChunks.start || map->lastVisibleChunks.end != map->visibleChunks.end)
		{
			map->PopulateBlocks();
			UpdateRenderData();

			map->lastVisibleChunks = map->visibleChunks;
		}
	}
}

void MapRenderer::Render(Vk::Frame* frame)
{
	VkSemaphore* wait = &frame->GetSemaphore(FrameSemaphore_ImageAvailable);
	VkSemaphore* signal = &frame->GetSemaphore(FrameSemaphore_MapRenderFinished);

	VkFence fence = frame->GetInFlightFence();

	if (updateCmdBuffers)
	{
		VT_PROFILER_NAMED_SCOPE("Overwrite cmd buffers");

		for (int i = 0; i < commandBuffers.size(); i++)
		{
			Vk::Framebuffer* framebuffer = Vk::Global::swapChain->GetFramebuffers()[i];
			Vk::CommandPool* pool = commandPools[i];
			Vk::CommandBuffer* cmd = commandBuffers[i];

			pool->Reset();
				cmd->Begin();
					colorPass->WriteToCmd(cmd, walls_to_render.size(), blocks_to_render.size());
					lightPass->WriteToCmd(cmd, lights_to_render.size());
					composition->WriteToCmd(cmd, framebuffer);
				cmd->End();
		}

		updateCmdBuffers = false;
	}

	VT_PROFILER_SCOPE();

	vkResetFences(Vk::Global::device->GetVkDevice(), 1, &fence);
	GetCurrentCmdBuffer()->SubmitToQueue(Vk::Global::Queues::graphicsQueue, wait, signal, fence);
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