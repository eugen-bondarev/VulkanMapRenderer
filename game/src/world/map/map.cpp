#include "map.h"

#include "FastNoiseLite/FastNoiseLite.h"

namespace Gameplay
{
	Map::Map()
	{
		DetermineDimensionsInBlocks();
		noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	}

	Map::~Map()
	{

	}

	void Map::CalculateVisibleBlocks(glm::vec2 view_position)
	{
		static glm::vec2 correction = glm::vec2(-16, 16);
		visibleBlocks.start.x = (view_position.x + correction.x - Engine::window->GetSize().x / 2.0f) / 16.0f / 1;
		visibleBlocks.end.x = visibleBlocks.start.x + Engine::window->GetSize().x / BLOCK_SIZE + 4;
		visibleBlocks.start.y = (view_position.y + correction.y - Engine::window->GetSize().y / 2.0f) / 16.0f / 1;
		visibleBlocks.end.y = visibleBlocks.start.y + Engine::window->GetSize().y / BLOCK_SIZE + 2;
		visibleBlocks.start.y -= 2;
	}

	void Map::DetermineDimensionsInBlocks()
	{
		CalculateVisibleBlocks(glm::vec2 { 0 });
		
		int length = visibleBlocks.end.x - visibleBlocks.start.x;
		int height = visibleBlocks.end.y - visibleBlocks.start.y;

		blocks.resize(length);

		for (int i = 0; i < length; i++)
		{
			blocks[i].resize(height);
		}
	}

	void Map::PopulateBlocks(glm::vec2 view_position)
	{		
		CalculateVisibleBlocks(view_position);

		static int height_variation = 5000;
		static int horizon = 0;

		for (int x = visibleBlocks.start.x; x < visibleBlocks.end.x; x++)
		{
			glm::vec2 block_position;
			block_position.x = x * BLOCK_SIZE;

			int height_in_this_area = height_variation * noise.GetNoise(block_position.x * settings.size1, 0.0f);

			for (int y = visibleBlocks.start.y; y < visibleBlocks.end.y; y++)
			{
				block_position.y = y * BLOCK_SIZE;

				glm::ivec2 normalized_indices = glm::ivec2(x, y) - visibleBlocks.start;

				normalized_indices.x = std::min<int>(normalized_indices.x, blocks.size() - 1);
				normalized_indices.y = std::min<int>(normalized_indices.y, blocks[0].size() - 1);

				if (block_position.y > horizon + height_in_this_area * noise.GetNoise(block_position.x * settings.size2, 0.0f))
				{
					blocks[normalized_indices.x][normalized_indices.y].type = BlockType::Dirt;
				}
				else
				{
					blocks[normalized_indices.x][normalized_indices.y].type = BlockType::Empty;
				}

				blocks[normalized_indices.x][normalized_indices.y].worldPosition = block_position;
			}
		}
	}

	Blocks_t& Map::GetBlocks()
	{
		return blocks;
	}
}