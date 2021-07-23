#include "map.h"

#include "FastNoiseLite/FastNoiseLite.h"

#include "renderer/atlas/texture_atlas.h"
#include "renderer/world/map/tiles.h"

#include <future>
#include <execution>

struct BlockRepresentation
{		
	BlockType type;
	glm::vec2 tile;
};

BlockType WhatBlockType(float noise_value, TilePos tile_pos)
{
	BlockType type = BlockType::Empty;

	if (tile_pos == TilePos::Foreground)
	{
		if (noise_value + settings.bias0 > 0.75f) 
			type = BlockType::Empty;

		if (noise_value + settings.bias0 >= 0.3f && noise_value < 0.75f)
			type = BlockType::Grass;

		if (noise_value + settings.bias0 < 0.3f)
			type = BlockType::Stone;
	}
	else
	{
		if (noise_value + settings.bias1 > 0.75f) 
			type = BlockType::Grass;

		if (noise_value + settings.bias1 >= 0.3f && noise_value < 0.75f)
			type = BlockType::Grass;

		if (noise_value + settings.bias1 < 0.3f)
			type = BlockType::Stone;
	}

	return type;
}

Map::Map()
{
	DetermineDimensionsInBlocks();
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
}

Map::~Map()
{

}

void Map::CalculateVisibleChunks(glm::vec2 view_position)
{
	static glm::vec2 correction = glm::vec2(-16, 16);
	visibleChunks.start.x = (view_position.x + correction.x - Engine::window->GetSize().x / 2.0f) / 16.0f / CHUNK_SIZE;
	visibleChunks.end.x = visibleChunks.start.x + Engine::window->GetSize().x / (CHUNK_SIZE * BLOCK_SIZE) + 3;
	visibleChunks.start.y = (view_position.y + correction.y - Engine::window->GetSize().y / 2.0f) / 16.0f / CHUNK_SIZE;
	visibleChunks.end.y = visibleChunks.start.y + Engine::window->GetSize().y / (CHUNK_SIZE * BLOCK_SIZE) + 2;

	visibleChunks.start.y -= 2;
	visibleChunks.start.x -= 0;
}

void Map::DetermineDimensionsInBlocks()
{
	CalculateVisibleChunks(glm::vec2 { 0 });

	glm::ivec2 dimensions;
	
	dimensions.x = (visibleChunks.end.x - visibleChunks.start.x) * CHUNK_SIZE;
	dimensions.y = (visibleChunks.end.y - visibleChunks.start.y) * CHUNK_SIZE;

	blocks.resize(dimensions.x);

	for (int i = 0; i < dimensions.x; i++)
	{
		blocks[i].resize(dimensions.y);
	}

	VT_VAR_OUT(dimensions.x * dimensions.y);
}

void Map::Async_PopulateBlocks(int start, int end)
{
	static int height_variation = 5000;
	static int horizon = 0;

	glm::ivec2 chunk;
	glm::ivec2 block_in_chunk;
	glm::ivec2 block_in_grid;
	glm:: vec2 block_position;
	glm::ivec2 block_indices;

	for (chunk.x = start; chunk.x < end; chunk.x++)
	{
		for (block_in_chunk.x = 0; block_in_chunk.x < CHUNK_SIZE; block_in_chunk.x++)
		{
			block_in_grid.x = chunk.x * CHUNK_SIZE + block_in_chunk.x;
			block_position.x = block_in_grid.x * BLOCK_SIZE;
			block_indices.x = (chunk.x - visibleChunks.start.x) * static_cast<int>(CHUNK_SIZE) + block_in_chunk.x;

			int height_in_this_area = height_variation * noise.GetNoise(block_position.x * settings.size1, 0.0f);
			float height_noise_at_x = noise.GetNoise(block_position.x * settings.size2, 0.0f);

			for (chunk.y = visibleChunks.start.y; chunk.y < visibleChunks.end.y; chunk.y++)
			{
				for (block_in_chunk.y = 0; block_in_chunk.y < CHUNK_SIZE; block_in_chunk.y++)
				{
					block_in_grid.y = chunk.y * CHUNK_SIZE + block_in_chunk.y;
					block_position.y = block_in_grid.y * BLOCK_SIZE;
					block_indices.y = (chunk.y - visibleChunks.start.y) * static_cast<int>(CHUNK_SIZE) + block_in_chunk.y;

					if (block_position.y > horizon + height_in_this_area * height_noise_at_x)
					{
						float noise_value = noise.GetNoise(block_position.x * settings.size0, block_position.y * settings.size0) * 0.5f + 0.5f;
						BlockType type = WhatBlockType(noise_value, TilePos::Foreground);
						blocks[block_indices.x][block_indices.y].type = type;
					}
					else
					{
						blocks[block_indices.x][block_indices.y].type = BlockType::Empty;
					}

					blocks[block_indices.x][block_indices.y].worldPosition = block_position;
				}
			}
		}
	}
}

void Map::PopulateBlocks(glm::vec2 view_position)
{	
	VT_PROFILER_SCOPE();

	CalculateVisibleChunks(view_position);

	static int cores_to_use = Engine::Util::CPU::AMOUNT_OF_CORES * 2;
	static int task_length = cores_to_use - 1;	// TODO: Process the case when the value equals to 0.
	int length = visibleChunks.end.x - visibleChunks.start.x;
	int full_fraction = (length - (length % task_length)) / task_length;
	int last_fraction = length - task_length * full_fraction;

	std::vector<glm::vec2> intervals;

	for (int i = 0; i < task_length; i++)
	{
		int start = visibleChunks.start.x + i * full_fraction;
		int end = start + full_fraction;
		intervals.emplace_back(start, end);
	}

	int start = visibleChunks.start.x + task_length * full_fraction;
	int end = start + last_fraction;
	intervals.emplace_back(start, end);

	std::for_each(
		std::execution::par, 
		std::begin(intervals), 
		std::end(intervals), 
		[&](glm::vec2& interval) 
		{
			Async_PopulateBlocks(interval.x, interval.y);
		}
	);
}

int Map::GetAmountOfBlocks() const
{
	return blocks.size() * blocks[0].size();
}

Blocks_t& Map::GetBlocks()
{
	return blocks;
}