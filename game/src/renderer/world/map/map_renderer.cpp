#include "map_renderer.h"

#include <engine/engine.h>

namespace Gameplay
{
	namespace MapRenderer
	{		
		std::vector<glm::vec4> GetRenderData(Map* map, glm::vec2 view_position)
		{
			const auto& blocks = map->GetBlocks();

			std::vector<glm::vec4> data;
			data.reserve(blocks.size() * blocks[0].size());

			for (int x = 0; x < blocks.size(); x++)
			{
				for (int y = 0; y < blocks[0].size(); y++)
				{
					if (blocks[x][y].type != BlockType::Empty)
					{
						data.emplace_back(blocks[x][y].worldPosition, 0, 0);
					}
					else
					{
						data.emplace_back(blocks[x][y].worldPosition, 2, 4);
					}
				}
			}
			
			return data;
		}
	}
}