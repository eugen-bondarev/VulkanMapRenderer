#include "map_renderer.h"

#include <engine/engine.h>

#include "tiles.h"
#include "../../atlas/texture_atlas.h"

#include <future>
#include <execution>

namespace MapRenderer
{
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